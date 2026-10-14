/*
 * otto_pca.c - Otto 六舵机协同动作控制实现(PCA9685 用户态 SDK 版)
 * 移植自内核版 mytest/pca9685/otto_pca.c,参考 xiaozhi-esp32 otto-robot 的
 * otto_movements.cc/oscillator.cc;动作表/振荡引擎/参数与内核版逐行一致,
 * 仅平台层三处替换:
 *   1. LOS_TaskDelay    -> PcaDelayMs(nanosleep, pca_port.h)
 *   2. LOS_TickCountGet -> PcaTickMs(CLOCK_MONOTONIC, pca_port.h)
 *   3. LOS_TaskCreate   -> pthread_create(detached, musl 默认栈)
 * 下发方式:逐通道 I2cTransfer 写 PWM 计数(用户态经 HDF I2C manager,
 * 一次 6 通道传输总耗时百微秒级,同拍偏差可接受)。
 *
 * 红线:动作在后台 otto 任务里执行(异步投递时);OttoPcaStop 置停止位,
 * 振荡/插值/延时里逐拍检查,中止后自动归位。动作期间不要再写通道,
 * 会与编排抢拍。阻塞式 API(OttoPcaRun/逐动作接口)在调用者线程里执行。
 */
#include "otto_pca.h"
#include "pca9685.h"
#include "pca_port.h"
#include <pthread.h>
#include "string.h"

#define OTTO_LIMB_N        6              /* 肢体数 = 舵机台数 */
#define OTTO_PAR_N         4              /* 每动作最多参数个数 */
#define OTTO_TRIM_MAX      50             /* trim 限幅(度) */
#define OTTO_HOME_TIME_MIN 500            /* 归位最短 ms(参考固件同值) */
#define OTTO_HOME_TIME_MAX 1700           /* 归位最长 ms */
#define OTTO_HOME_RATE     9              /* 归位时间 = 500 + 最大位移*9 */

/* sin(0..90°)*10000 查表,负值/镜像按象限折算,内核不用浮点 */
static const int16_t g_sinTab[91] = {
    0, 175, 349, 523, 698, 872, 1045, 1219, 1392, 1564,
    1736, 1908, 2079, 2250, 2419, 2588, 2756, 2924, 3090, 3256,
    3420, 3584, 3746, 3907, 4067, 4226, 4384, 4540, 4695, 4848,
    5000, 5150, 5299, 5446, 5592, 5736, 5878, 6018, 6157, 6293,
    6428, 6561, 6691, 6820, 6947, 7071, 7193, 7314, 7431, 7547,
    7660, 7771, 7880, 7986, 8090, 8192, 8290, 8387, 8480, 8572,
    8660, 8746, 8829, 8910, 8988, 9063, 9135, 9205, 9272, 9336,
    9397, 9455, 9511, 9563, 9613, 9659, 9703, 9744, 9781, 9816,
    9848, 9877, 9903, 9925, 9945, 9962, 9976, 9986, 9994, 9998,
    10000
};

/* ---- 运行期配置(OttoPcaStart 按 OTTO_LIMB_*_TAB 宏初始化) ---- */
static uint8_t g_limbCh[OTTO_LIMB_N];          /* 肢体 -> PCA9685 通道 */
static int32_t g_trim[OTTO_LIMB_N];            /* 肢体微调(度) */
static uint8_t g_rev[OTTO_LIMB_N];             /* 肢体方向反转 */
static const char *g_limbName[OTTO_LIMB_N] = {"ll", "rl", "lf", "rf", "lh", "rh"};
static int32_t g_cur[OTTO_LIMB_N];             /* 各肢体当前逻辑角(otto 角) */

/* ---- 任务/停止 ---- */
static volatile int32_t g_stopReq;             /* 1=请求停止并归位 */
static volatile int32_t g_pend;                /* 1=有动作待执行 */
static volatile int32_t g_homed;               /* 上电后是否已归过位 */
static volatile int32_t g_taskOn;              /* 任务已创建 */
static pthread_t g_taskId;                     /* 后台动作任务句柄 */
struct OttoAct;
static const struct OttoAct *g_act;            /* 待执行动作 */
static int32_t g_par[OTTO_PAR_N];              /* 待执行动作参数 */

static int32_t OttoHomeCore(void);
static void *OttoTaskLoop(void *arg);

/* ================= 基础层 ================= */
static int OttoSinDeg(int deg)
{
    int s = deg % 360;

    if (s < 0) {
        s += 360;
    }
    if (s <= 90) {
        return g_sinTab[s];
    }
    if (s <= 180) {
        return g_sinTab[180 - s];
    }
    if (s <= 270) {
        return -g_sinTab[s - 180];
    }
    return -g_sinTab[360 - s];
}

/* otto 逻辑角 -> 舵机角:加 trim -> 裁 0~180 -> 可选镜像 -> 区间映射 */
static int32_t OttoDeg(int limb, int ang)
{
    int32_t d = ang + g_trim[limb];

    if (d < 0) {
        d = 0;
    }
    if (d > 180) {
        d = 180;
    }
    if (g_rev[limb] != 0) {
        d = 180 - d;
    }
    return OTTO_ANG_MAP_MIN + d * (OTTO_ANG_MAP_MAX - OTTO_ANG_MAP_MIN) / 180;
}

/* 6 肢体同步下发目标(不等待),tgt 为 otto 逻辑角;
 * 总线舵机版走 RegWrite+广播 Action,这里逐通道写(I2C 快,同拍微秒级偏差) */
static int32_t OttoSyncTgt(const int tgt[OTTO_LIMB_N])
{
    int32_t deg[OTTO_LIMB_N];
    int32_t i;
    int32_t ret = 0;

    for (i = 0; i < OTTO_LIMB_N; i++) {
        deg[i] = OttoDeg(i, tgt[i]);
    }
    for (i = 0; i < OTTO_LIMB_N; i++) {
        if (PcaServoAngle(g_limbCh[i], (uint32_t)deg[i]) != HDF_SUCCESS) {
            ret = -1;
            break;
        }
    }
    if (ret == 0) {
        for (i = 0; i < OTTO_LIMB_N; i++) {
            g_cur[i] = tgt[i];
        }
    }
    return ret;
}

/* 同步移动到目标位并等待到位:PWM 舵机没有"定时到达"硬件插值,
 * 恢复参考固件 otto_movements.cc 的 MoveServos:EaseOutCubic 10ms 一拍 */
static int32_t OttoMove(int32_t timeMs, const int tgt[OTTO_LIMB_N])
{
    int32_t start[OTTO_LIMB_N];
    int32_t nStep;
    int32_t k;
    int32_t i;

    if (g_stopReq != 0) {
        return -1;
    }
    if (timeMs <= 10) {
        return OttoSyncTgt(tgt);
    }
    for (i = 0; i < OTTO_LIMB_N; i++) {
        start[i] = g_cur[i];
    }
    nStep = timeMs / 10;
    for (k = 1; k <= nStep; k++) {
        int32_t x = k * 1000 / nStep;                  /* 进度 0~1000 */
        int32_t e = 1000 - (1000 - x) * (1000 - x) * (1000 - x) / 1000000;
        int tgtStep[OTTO_LIMB_N];

        if (g_stopReq != 0) {
            return -1;
        }
        for (i = 0; i < OTTO_LIMB_N; i++) {
            tgtStep[i] = start[i] + (tgt[i] - start[i]) * e / 1000;
        }
        (void)OttoSyncTgt(tgtStep);
        PcaDelayMs(10);
    }
    return 0;
}

/* 带停止检查的延时;返回 -1=期间被请求停止 */
static int32_t OttoDelay(int32_t ms)
{
    while (ms > 0) {
        if (g_stopReq != 0) {
            return -1;
        }
        if (ms > 20) {
            PcaDelayMs(20);
            ms -= 20;
        } else {
            PcaDelayMs(ms);
            ms = 0;
        }
    }
    return 0;
}

/* 归位:腿脚 90,手 45/135;位移越大时间越长(参考 Home 公式) */
static int32_t OttoHomeCore(void)
{
    int homes[OTTO_LIMB_N] = {90, 90, 90, 90, OTTO_HAND_HOME,
                              180 - OTTO_HAND_HOME};
    int32_t maxd = 0;
    int32_t i;
    int32_t t;
    int32_t ret;

    for (i = 0; i < OTTO_LIMB_N; i++) {
        int32_t d = homes[i] - g_cur[i];

        if (d < 0) {
            d = -d;
        }
        if (d > maxd) {
            maxd = d;
        }
    }
    t = OTTO_HOME_TIME_MIN + maxd * OTTO_HOME_RATE;
    if (t < OTTO_HOME_TIME_MIN) {
        t = OTTO_HOME_TIME_MIN;
    }
    if (t > OTTO_HOME_TIME_MAX) {
        t = OTTO_HOME_TIME_MAX;
    }
    ret = OttoMove(t, homes);                  /* PWM 舵机上电即有力,无需开扭矩 */
    g_homed = 1;
    return ret;
}

/* ================= 振荡引擎 =================
 * 逻辑:angle_i = A_i*sin(ph_i + 2π*t/T) + O_i + 90(O 为相对 90° 的偏移,
 * 与参考 Execute 一致);按 OTTO_SAMPLE_MS 采样写 PWM,舵机自行走到位 */
static int32_t OttoOscOnce(const int A[OTTO_LIMB_N], const int O[OTTO_LIMB_N],
                           int period, const int ph[OTTO_LIMB_N], int32_t msLen)
{
    uint32_t start;
    int32_t nStp;
    int32_t lastStp = -1;

    if ((msLen <= 0) || (g_stopReq != 0)) {
        return -1;
    }
    nStp = period / OTTO_SAMPLE_MS;
    if (nStp < 1) {
        nStp = 1;
    }
    start = PcaTickMs();                /* tick=1ms */
    for (;;) {
        uint32_t el;
        int32_t step;
        int tgt[OTTO_LIMB_N];

        if (g_stopReq != 0) {
            return -1;
        }
        el = PcaTickMs() - start;
        if (el >= (uint32_t)msLen) {
            break;
        }
        step = (int32_t)(el / OTTO_SAMPLE_MS);
        if (step != lastStp) {
            int32_t i;
            int32_t ret;
            int base = (36000 * step) / nStp;  /* 相位基值,单位 0.01° */

            lastStp = step;
            for (i = 0; i < OTTO_LIMB_N; i++) {
                int s = ((base + ph[i] * 100) % 36000 + 36000) % 36000;
                int pos = (A[i] * OttoSinDeg((s + 50) / 100)) / 10000 +
                          O[i] + 90;

                tgt[i] = pos;
            }
            ret = OttoSyncTgt(tgt);
            if (ret != 0) {
                return ret;
            }
        }
        PcaDelayMs(2);
    }
    return 0;
}

/* 完整周期 + 尾部小数周期(stepsX10=步数*10,支持 0.5 步) */
static int32_t OttoExecOsc(const int A[OTTO_LIMB_N], const int O[OTTO_LIMB_N],
                           int period, const int ph[OTTO_LIMB_N], int32_t stepsX10)
{
    int32_t full = stepsX10 / 10;
    int32_t rem = stepsX10 % 10;
    int32_t ret = 0;
    int32_t i;

    for (i = 0; (i < full) && (ret == 0); i++) {
        ret = OttoOscOnce(A, O, period, ph, period);
    }
    if ((ret == 0) && (rem > 0)) {
        ret = OttoOscOnce(A, O, period, ph, period * rem / 10);
    }
    PcaDelayMs(10);
    return ret;
}

/* 同上,但给绝对中心角 C(与参考 Execute2 一致) */
static int32_t OttoExecOsc2(const int A[OTTO_LIMB_N], const int C[OTTO_LIMB_N],
                            int period, const int ph[OTTO_LIMB_N], int32_t stepsX10)
{
    int O[OTTO_LIMB_N];
    int32_t i;

    for (i = 0; i < OTTO_LIMB_N; i++) {
        O[i] = C[i] - 90;
    }
    return OttoExecOsc(A, O, period, ph, stepsX10);
}

/* ================= 动作集(参数顺序与 OttoPcaList 输出一致) ================= */
/* walk <steps> <periodMs> <dir 1前/-1后> <armSwing 0不摆> */
static int32_t ActWalk(const int *p)
{
    int A[6] = {30, 30, 30, 30, 0, 0};
    int O[6] = {0, 0, 5, -5, OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {0, 0, -90 * p[2], -90 * p[2], 0, 0};

    if (p[3] > 0) {
        A[OTTO_LH] = p[3];
        A[OTTO_RH] = p[3];
        ph[OTTO_LH] = ph[OTTO_RL];             /* 左手与右腿同相,自然摆臂 */
        ph[OTTO_RH] = ph[OTTO_LL];             /* 右手与左腿同相 */
    }
    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* turn <steps> <periodMs> <dir 1左/-1右> <armSwing> */
static int32_t ActTurn(const int *p)
{
    int A[6] = {30, 30, 30, 30, 0, 0};
    int O[6] = {0, 0, 5, -5, OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {0, 0, -90, -90, 0, 0};

    if (p[2] == 1) {
        A[OTTO_LL] = 30;                       /* 只振左腿,走左弧 */
        A[OTTO_RL] = 0;
    } else {
        A[OTTO_LL] = 0;
        A[OTTO_RL] = 30;
    }
    if (p[3] > 0) {
        A[OTTO_LH] = p[3];
        A[OTTO_RH] = p[3];
        ph[OTTO_LH] = ph[OTTO_LL];             /* 手与同侧腿同相增强转向 */
        ph[OTTO_RH] = ph[OTTO_RL];
    }
    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* jump <times> <periodMs>:蹲(脚 150/30) -> 站 */
static int32_t ActJump(const int *p)
{
    int up[6] = {90, 90, 150, 30, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int down[6] = {90, 90, 90, 90, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int32_t i;
    int32_t ret = 0;

    for (i = 0; (i < p[0]) && (ret == 0); i++) {
        ret = OttoMove(p[1], up);
        if (ret == 0) {
            ret = OttoMove(p[1], down);
        }
    }
    return ret;
}

/* bend <steps> <periodMs> <dir 1左/-1右>:固定 T2=800 侧弯 */
static int32_t ActBend(const int *p)
{
    int b1[6] = {90, 90, 62, 35, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int b2[6] = {90, 90, 62, 105, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int homes[6] = {90, 90, 90, 90, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int32_t i;
    int32_t ret = 0;

    if (p[2] == -1) {                          /* 镜像弯另一侧 */
        b1[OTTO_LF] = 180 - 35;
        b1[OTTO_RF] = 180 - 60;                /* 参考注释:65 会不平衡 */
        b2[OTTO_LF] = 180 - 105;
        b2[OTTO_RF] = 180 - 60;
    }
    for (i = 0; (i < p[0]) && (ret == 0); i++) {
        ret = OttoMove(400, b1);
        if (ret == 0) {
            ret = OttoMove(400, b2);
        }
        if (ret == 0) {
            ret = OttoDelay(p[1] * 8 / 10);
        }
        if (ret == 0) {
            ret = OttoMove(500, homes);
        }
    }
    return ret;
}

/* shakeleg <steps> <periodMs> <dir 1左腿/-1右腿> */
static int32_t ActShakeLeg(const int *p)
{
    int s1[6] = {90, 90, 58, 35, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int s2[6] = {90, 90, 58, 120, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int s3[6] = {90, 90, 58, 60, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int homes[6] = {90, 90, 90, 90, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int32_t per = p[1] - 1000;
    int32_t i;
    int32_t j;
    int32_t ret = 0;

    if (p[2] == 1) {                           /* 摇左腿,镜像 */
        s1[OTTO_LF] = 180 - 35;
        s1[OTTO_RF] = 180 - 58;
        s2[OTTO_LF] = 180 - 120;
        s2[OTTO_RF] = 180 - 58;
        s3[OTTO_LF] = 180 - 60;
        s3[OTTO_RF] = 180 - 58;
    }
    if (per < 400) {
        per = 400;
    }
    for (i = 0; (i < p[0]) && (ret == 0); i++) {
        ret = OttoMove(500, s1);
        if (ret == 0) {
            ret = OttoMove(500, s2);
        }
        for (j = 0; (j < 2) && (ret == 0); j++) {
            ret = OttoMove(per / 4, s3);
            if (ret == 0) {
                ret = OttoMove(per / 4, s2);
            }
        }
        if (ret == 0) {
            ret = OttoMove(500, homes);
        }
        if (ret == 0) {
            ret = OttoDelay(per);
        }
    }
    return ret;
}

/* sit:坐下(120,60,0,180),不自动归位 */
static int32_t ActSit(const int *p)
{
    int tgt[6] = {120, 60, 0, 180, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};

    (void)p;
    return OttoMove(600, tgt);
}

/* updown <steps> <periodMs> <h>:两脚反相,身体一起一落 */
static int32_t ActUpDown(const int *p)
{
    int A[6] = {0, 0, p[2], p[2], 0, 0};
    int O[6] = {0, 0, p[2], -p[2], OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {0, 0, -90, 90, 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* swing <steps> <periodMs> <h>:两脚同相,重心左右摆 */
static int32_t ActSwing(const int *p)
{
    int A[6] = {0, 0, p[2], p[2], 0, 0};
    int O[6] = {0, 0, p[2] / 2, -p[2] / 2, OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {0, 0, 0, 0, 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* tiptoe <steps> <periodMs> <h>:踮脚摆(offset=振幅,不落跟) */
static int32_t ActTiptoeSwing(const int *p)
{
    int A[6] = {0, 0, p[2], p[2], 0, 0};
    int O[6] = {0, 0, p[2], -p[2], OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {0, 0, 0, 0, 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* jitter <steps> <periodMs> <h<=25>:两腿反相小幅抖动 */
static int32_t ActJitter(const int *p)
{
    int h = (p[2] > 25) ? 25 : p[2];
    int A[6] = {h, h, 0, 0, 0, 0};
    int O[6] = {0, 0, 0, 0, OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {-90, 90, 0, 0, 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* ascend <steps> <periodMs> <h<=13>:抖腿+上下(螺旋升天) */
static int32_t ActAscendingTurn(const int *p)
{
    int h = (p[2] > 13) ? 13 : p[2];
    int A[6] = {h, h, h, h, 0, 0};
    int O[6] = {0, 0, h + 4, -h + 4, OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {-90, 90, -90, 90, 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* crusaito <steps> <periodMs> <h> <dir 1/-1>:十字步(moonwalk+walk 合体) */
static int32_t ActCrusaito(const int *p)
{
    int A[6] = {25, 25, p[2], p[2], 0, 0};
    int O[6] = {0, 0, p[2] / 2 + 4, -p[2] / 2 - 4,
                OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {90, 90, 0, -60 * p[3], 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* flapping <steps> <periodMs> <h> <dir 1前/-1后>:拍翅 */
static int32_t ActFlapping(const int *p)
{
    int A[6] = {12, 12, p[2], p[2], 0, 0};
    int O[6] = {0, 0, p[2] - 10, -p[2] + 10,
                OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int ph[6] = {0, 180, -90 * p[3], 90 * p[3], 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* moonwalk <steps> <periodMs> <h> <dir 1左/-1右>:太空步 */
static int32_t ActMoonwalker(const int *p)
{
    int A[6] = {0, 0, p[2], p[2], 0, 0};
    int O[6] = {0, 0, p[2] / 2 + 2, -p[2] / 2 - 2,
                OTTO_HAND_HOME - 90, OTTO_HAND_HOME};
    int phi = -p[3] * 90;
    int ph[6] = {0, 0, phi, -60 * p[3] + phi, 0, 0};

    return OttoExecOsc(A, O, p[1], ph, p[0] * 10);
}

/* whirlwind <steps> <periodMs> <amp>:旋风腿(先摆姿态 1s 再左腿+左手振荡) */
static int32_t ActWhirlwindLeg(const int *p)
{
    int tgt[6] = {90, 90, 180, 90, OTTO_HAND_HOME, 20};
    int C[6] = {90, 90, 180, 160, OTTO_HAND_HOME, 20};
    int A[6] = {p[2], 0, 0, 0, p[2], 0};
    int ph[6] = {20, 0, 0, 0, 20, 0};
    int32_t ret;

    ret = OttoMove(100, tgt);
    if (ret == 0) {
        tgt[OTTO_RF] = 160;
        ret = OttoMove(500, tgt);
    }
    if (ret == 0) {
        ret = OttoDelay(1000);
    }
    if (ret == 0) {
        ret = OttoExecOsc2(A, C, p[1], ph, p[0] * 10);
    }
    return ret;
}

/* magic:爱的魔力转圈圈(腿脚+双手同场振荡,700ms×40) */
static int32_t ActMagicCircle(const int *p)
{
    int A[6] = {30, 30, 30, 30, 50, 50};
    int O[6] = {0, 0, 5, -5, 0, 0};
    int ph[6] = {0, 0, -90, -90, -90, 90};

    (void)p;
    return OttoExecOsc(A, O, 700, ph, 40 * 10);
}

/* handsup <periodMs> <dir 0双手/1左/-1右>:举手,结束不自动归位 */
static int32_t ActHandsUp(const int *p)
{
    int tgt[6] = {90, 90, 90, 90, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};

    if (p[1] == 0) {
        tgt[OTTO_LH] = 170;
        tgt[OTTO_RH] = 10;
    } else if (p[1] == 1) {
        tgt[OTTO_LH] = 170;
        tgt[OTTO_RH] = g_cur[OTTO_RH];         /* 另一只手保持当前位置 */
    } else {
        tgt[OTTO_RH] = 10;
        tgt[OTTO_LH] = g_cur[OTTO_LH];
    }
    return OttoMove(p[0], tgt);
}

/* handsdown <periodMs> <dir 0双手/1左/-1右>:回到垂手 */
static int32_t ActHandsDown(const int *p)
{
    int tgt[6] = {90, 90, 90, 90, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};

    if (p[1] == 1) {
        tgt[OTTO_RH] = g_cur[OTTO_RH];
    } else if (p[1] == -1) {
        tgt[OTTO_LH] = g_cur[OTTO_LH];
    }
    return OttoMove(p[0], tgt);
}

/* handwave <dir 0双手/1左/-1右>:挥手(300ms×5) */
static int32_t ActHandWave(const int *p)
{
    int C[6] = {90, 90, 90, 90, 160, 135};
    int A[6] = {0, 0, 0, 0, 20, 0};
    int ph[6] = {0, 0, 0, 0, 90, 0};

    if (p[0] == 1) {                           /* 左手 */
        C[OTTO_LH] = 160;
        A[OTTO_LH] = 20;
        ph[OTTO_LH] = 90;
    } else if (p[0] == -1) {                   /* 右手 */
        C[OTTO_LH] = OTTO_HAND_HOME;
        C[OTTO_RH] = 20;
        A[OTTO_LH] = 0;
        A[OTTO_RH] = 20;
        ph[OTTO_RH] = 90;
    } else {                                   /* 双手 */
        C[OTTO_LH] = 160;
        C[OTTO_RH] = 20;
        A[OTTO_LH] = 20;
        A[OTTO_RH] = 20;
        ph[OTTO_LH] = 90;
        ph[OTTO_RH] = 90;
    }
    return OttoExecOsc2(A, C, 300, ph, 5 * 10);
}

/* windmill <steps> <periodMs> <amp>:大风车(双手中心 90 同相画圈) */
static int32_t ActWindmill(const int *p)
{
    int C[6] = {90, 90, 90, 90, 90, 90};
    int A[6] = {0, 0, 0, 0, p[2], p[2]};
    int ph[6] = {0, 0, 0, 0, 90, 90};

    return OttoExecOsc2(A, C, p[1], ph, p[0] * 10);
}

/* takeoff <steps> <periodMs> <amp>:起飞(先归位,双手反相扑扇) */
static int32_t ActTakeoff(const int *p)
{
    int C[6] = {90, 90, 90, 90, 90, 90};
    int A[6] = {0, 0, 0, 0, p[2], p[2]};
    int ph[6] = {0, 0, 0, 0, 90, -90};
    int32_t ret;

    ret = OttoHomeCore();
    if (ret == 0) {
        ret = OttoExecOsc2(A, C, p[1], ph, p[0] * 10);
    }
    return ret;
}

/* fitness <steps> <periodMs> <amp>:健身(左脚翘起,右手举铃振荡) */
static int32_t ActFitness(const int *p)
{
    int tgt[6] = {90, 90, 90, 0, 160, 135};
    int C[6] = {90, 90, 20, 90, 160, 135};
    int A[6] = {0, 0, 0, 0, 0, p[2]};
    int ph[6] = {0};
    int32_t ret;

    ret = OttoMove(100, tgt);
    if (ret == 0) {
        tgt[OTTO_LF] = 20;
        ret = OttoMove(400, tgt);
    }
    if (ret == 0) {
        ret = OttoDelay(2000);
    }
    if (ret == 0) {
        ret = OttoExecOsc2(A, C, p[1], ph, p[0] * 10);
    }
    return ret;
}

/* greeting <dir 1左/-1右> <steps>:打招呼 */
static int32_t ActGreeting(const int *p)
{
    int tgt[6] = {90, 90, 150, 150, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int C[6] = {90, 90, 150, 150, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int A[6] = {0};
    int ph[6] = {0};
    int32_t ret;

    if (p[0] == 1) {
        tgt[OTTO_LF] = 150;                    /* 面向左侧 */
        tgt[OTTO_RF] = 150;
        C[OTTO_LH] = 160;
        A[OTTO_LH] = 20;
    } else {
        tgt[OTTO_LF] = 30;                     /* 面向右侧 */
        tgt[OTTO_RF] = 30;
        C[OTTO_LF] = 30;
        C[OTTO_RF] = 30;
        C[OTTO_RH] = 20;
        A[OTTO_RH] = 20;
    }
    ret = OttoMove(400, tgt);
    if (ret == 0) {
        ret = OttoExecOsc2(A, C, 300, ph, p[1] * 10);
    }
    return ret;
}

/* shy <dir 1左/-1右> <steps>:害羞(脚摆位+手小幅振荡) */
static int32_t ActShy(const int *p)
{
    int tgt[6] = {90, 90, 150, 150, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int C[6] = {90, 90, 150, 150, OTTO_HAND_HOME, 180 - OTTO_HAND_HOME};
    int A[6] = {0, 0, 0, 0, 20, 20};
    int ph[6] = {0, 0, 0, 0, 90, -90};
    int32_t ret;

    if (p[0] == -1) {                          /* 面向右侧 */
        tgt[OTTO_LF] = 30;
        tgt[OTTO_RF] = 30;
        C[OTTO_LF] = 30;
        C[OTTO_RF] = 30;
        A[OTTO_LH] = 0;
    }
    ret = OttoMove(400, tgt);
    if (ret == 0) {
        ret = OttoExecOsc2(A, C, 300, ph, p[1] * 10);
    }
    return ret;
}

/* calisthenics:广播体操 4 节(手反相 -> 脚 -> 手 -> 手,1000ms×8) */
static int32_t ActCalisthenics(const int *p)
{
    int C1[6] = {90, 90, 90, 90, 145, 45};
    int A1[6] = {0, 0, 0, 0, 45, 45};
    int ph1[6] = {0, 0, 0, 0, 90, -90};
    int C2[6] = {90, 90, 115, 65, 90, 90};
    int A2[6] = {0, 0, 25, 25, 0, 0};
    int ph2[6] = {0, 0, 90, -90, 0, 0};
    int C3[6] = {90, 90, 130, 130, 90, 90};
    int A3[6] = {0, 0, 0, 0, 20, 0};
    int C4[6] = {90, 90, 50, 50, 90, 90};
    int A4[6] = {0, 0, 0, 0, 0, 20};
    int ph0[6] = {0, 0, 0, 0, 0, 0};
    int32_t ret;

    (void)p;
    ret = OttoExecOsc2(A1, C1, 1000, ph1, 8 * 10);
    if (ret == 0) {
        ret = OttoExecOsc2(A2, C2, 1000, ph2, 8 * 10);
    }
    if (ret == 0) {
        ret = OttoExecOsc2(A3, C3, 1000, ph0, 8 * 10);
    }
    if (ret == 0) {
        ret = OttoExecOsc2(A4, C4, 1000, ph0, 8 * 10);
    }
    return ret;
}

/* showcase:展示串(Walk->HandWave->Calisthenics->Moonwalk->Swing->
 * Takeoff->Fitness->Walk),不自动归位 */
static int32_t ActShowcase(const int *p)
{
    int w1[4] = {3, 1000, 1, 50};
    int w2[4] = {3, 1000, -1, 50};
    int hw[1] = {1};
    int mw[4] = {3, 900, 25, 1};
    int sw[3] = {3, 1000, 30};
    int tk[3] = {5, 300, 40};
    int ft[3] = {5, 1000, 25};
    int32_t ret;

    (void)p;
    ret = ActWalk(w1);
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActHandWave(hw);
    }
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActCalisthenics(NULL);
    }
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActMoonwalker(mw);
    }
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActSwing(sw);
    }
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActTakeoff(tk);
    }
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActFitness(ft);
    }
    if (ret == 0) {
        ret = OttoDelay(500);
    }
    if (ret == 0) {
        ret = ActWalk(w2);
    }
    return ret;
}

/* home:立即归位(腿脚 90,手 45/135) */
static int32_t ActHome(const int *p)
{
    (void)p;
    return OttoHomeCore();
}

/* ================= 动作表(name/参数个数/默认值/结束归位/实现/说明) ================= */
struct OttoAct {
    const char *name;
    int32_t npar;
    int32_t dflt[OTTO_PAR_N];
    int32_t noHome;                            /* 1=结束不自动归位 */
    int32_t (*fn)(const int *p);
    const char *desc;
};

#define OTTO_ACT_END  { NULL, 0, {0}, 0, NULL, NULL }
static const struct OttoAct g_acts[] = {
    {"walk",        4, {4, 1000, 1, 0},      0, ActWalk,
     "walk <steps> <periodMs> <dir 1前/-1后> <arm 0不摆>"},
    {"turn",        4, {4, 1000, 1, 0},      0, ActTurn,
     "turn <steps> <periodMs> <dir 1左/-1右> <arm>"},
    {"jump",        2, {1, 1000, 0, 0},      0, ActJump,
     "jump <times> <periodMs>"},
    {"bend",        3, {3, 1000, 1, 0},      0, ActBend,
     "bend <steps> <periodMs> <dir 1左/-1右>"},
    {"shakeleg",    3, {1, 2000, 1, 0},      0, ActShakeLeg,
     "shakeleg <steps> <periodMs(>1000)> <dir 1左腿/-1右腿>"},
    {"shake_leg",   3, {1, 2000, 1, 0},      0, ActShakeLeg, NULL},
    {"sit",         0, {0, 0, 0, 0},         1, ActSit, "sit(坐下,不归位)"},
    {"updown",      3, {4, 1000, 30, 0},     0, ActUpDown,
     "updown <steps> <periodMs> <h 0-90>"},
    {"swing",       3, {4, 1000, 30, 0},     0, ActSwing,
     "swing <steps> <periodMs> <h 0-50>"},
    {"tiptoe",      3, {4, 1000, 30, 0},     0, ActTiptoeSwing,
     "tiptoe <steps> <periodMs> <h>"},
    {"tiptoeswing", 3, {4, 1000, 30, 0},     0, ActTiptoeSwing, NULL},
    {"jitter",      3, {4, 1000, 20, 0},     0, ActJitter,
     "jitter <steps> <periodMs> <h<=25>"},
    {"ascend",      3, {4, 1000, 13, 0},     0, ActAscendingTurn,
     "ascend <steps> <periodMs> <h<=13>"},
    {"crusaito",    4, {4, 1000, 25, 1},     0, ActCrusaito,
     "crusaito <steps> <periodMs> <h> <dir 1/-1>"},
    {"flapping",    4, {4, 1000, 25, 1},     0, ActFlapping,
     "flapping <steps> <periodMs> <h> <dir 1前/-1后>"},
    {"moonwalk",    4, {3, 900, 25, 1},      0, ActMoonwalker,
     "moonwalk <steps> <periodMs> <h> <dir 1左/-1右>"},
    {"moonwalker",  4, {3, 900, 25, 1},      0, ActMoonwalker, NULL},
    {"whirlwind",   3, {2, 700, 30, 0},      0, ActWhirlwindLeg,
     "whirlwind <steps> <periodMs> <amp>"},
    {"whirlwind_leg", 3, {2, 700, 30, 0},    0, ActWhirlwindLeg, NULL},
    {"magic",       0, {0, 0, 0, 0},         0, ActMagicCircle,
     "magic(转圈圈 700ms*40)"},
    {"magic_circle", 0, {0, 0, 0, 0},        0, ActMagicCircle, NULL},
    {"handsup",     2, {800, 0, 0, 0},       1, ActHandsUp,
     "handsup <periodMs> <dir 0双手/1左/-1右>(不归位)"},
    {"hands_up",    2, {800, 0, 0, 0},       1, ActHandsUp, NULL},
    {"handsdown",   2, {800, 0, 0, 0},       0, ActHandsDown,
     "handsdown <periodMs> <dir 0双手/1左/-1右>"},
    {"hands_down",  2, {800, 0, 0, 0},       0, ActHandsDown, NULL},
    {"handwave",    1, {1, 0, 0, 0},         0, ActHandWave,
     "handwave <dir 0双手/1左/-1右>"},
    {"hand_wave",   1, {1, 0, 0, 0},         0, ActHandWave, NULL},
    {"windmill",    3, {5, 300, 40, 0},      0, ActWindmill,
     "windmill <steps> <periodMs> <amp>"},
    {"takeoff",     3, {5, 300, 40, 0},      0, ActTakeoff,
     "takeoff <steps> <periodMs> <amp>"},
    {"fitness",     3, {5, 1000, 25, 0},     0, ActFitness,
     "fitness <steps> <periodMs> <amp>"},
    {"greeting",    2, {1, 5, 0, 0},         0, ActGreeting,
     "greeting <dir 1左/-1右> <steps>"},
    {"shy",         2, {1, 5, 0, 0},         0, ActShy,
     "shy <dir 1左/-1右> <steps>"},
    {"calisthenics", 0, {0, 0, 0, 0},        0, ActCalisthenics,
     "calisthenics(广播体操)"},
    {"radio_calisthenics", 0, {0, 0, 0, 0}, 0, ActCalisthenics, NULL},
    {"showcase",    0, {0, 0, 0, 0},         1, ActShowcase,
     "showcase(动作串演,不归位)"},
    {"home",        0, {0, 0, 0, 0},         1, ActHome, "home(归位)"},
    OTTO_ACT_END,
};

static const struct OttoAct *OttoFind(const char *name)
{
    const struct OttoAct *a;

    for (a = g_acts; a->name != NULL; a++) {
        if (strcmp(a->name, name) == 0) {
            return a;
        }
    }
    return NULL;
}

/* 执行一个动作:(首次先归位)-> 动作 -> 归位
 * (总线舵机版的开扭矩/恢复定时在 PWM 舵机上不需要) */
static int32_t OttoExecAct(const struct OttoAct *act, const int *par)
{
    int32_t ret;

    if (g_homed == 0) {
        (void)OttoHomeCore();                  /* 首个动作前先到 HOME 姿态 */
    }
    ret = act->fn(par);
    if (g_stopReq != 0) {
        g_stopReq = 0;
        (void)OttoHomeCore();
        return -1;
    }
    if (act->noHome == 0) {
        (void)OttoHomeCore();                  /* 与参考一致:动作完自动归位 */
    }
    return ret;
}

/* ================= 公开 API ================= */
static void OttoInitTables(void)
{
    static const int limbChTab[OTTO_LIMB_N] = OTTO_LIMB_CH_TAB;
    static const int trimTab[OTTO_LIMB_N] = OTTO_LIMB_TRIM_TAB;
    static const int revTab[OTTO_LIMB_N] = OTTO_LIMB_REV_TAB;
    static int32_t inited = 0;
    int32_t i;

    if (inited != 0) {
        return;
    }
    for (i = 0; i < OTTO_LIMB_N; i++) {
        g_limbCh[i] = (uint8_t)limbChTab[i];
        g_trim[i] = trimTab[i];
        g_rev[i] = (revTab[i] != 0) ? 1 : 0;
        g_cur[i] = 90;                         /* 上电逻辑角按 HOME 假设 */
    }
    g_cur[OTTO_LH] = OTTO_HAND_HOME;
    g_cur[OTTO_RH] = 180 - OTTO_HAND_HOME;
    inited = 1;
}

int32_t OttoPcaStart(void)
{
    pthread_attr_t attr;
    uint8_t bus;
    uint8_t addr;
    int32_t ready;

    OttoInitTables();
    PcaGetStatus(&bus, &addr, &ready);         /* 沿用 init 用的总线/地址 */
    if (PcaEnsureReady(bus, addr, 0) != HDF_SUCCESS) {
        PRINTK("otto: pca9685 not ready (先 'pca_demo scan'/'pca_demo init' 排查)\n");
        return -1;
    }
    if (g_taskOn != 0) {
        return 0;
    }
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&g_taskId, &attr, OttoTaskLoop, NULL) != 0) {
        pthread_attr_destroy(&attr);
        PRINTK("otto: create task fail\n");
        return -1;
    }
    pthread_attr_destroy(&attr);
    g_taskOn = 1;
    return 0;
}

int32_t OttoPcaRun(const char *name, const int *par, int npar)
{
    const struct OttoAct *act = OttoFind(name);
    int32_t p[OTTO_PAR_N];
    int32_t i;

    if (act == NULL) {
        return -3;
    }
    if ((npar < 0) || (npar > OTTO_PAR_N)) {
        npar = 0;
    }
    for (i = 0; i < OTTO_PAR_N; i++) {
        p[i] = (i < npar) ? par[i] : act->dflt[i];
    }
    if (OttoPcaStart() != 0) {
        return -1;
    }
    g_stopReq = 0;
    return OttoExecAct(act, p);
}

int32_t OttoPcaPost(const char *name, const int *par, int npar)
{
    const struct OttoAct *act = OttoFind(name);
    int32_t i;

    if (act == NULL) {
        return -3;
    }
    if (OttoPcaStart() != 0) {
        return -1;
    }
    if ((npar < 0) || (npar > OTTO_PAR_N)) {
        npar = 0;
    }
    for (i = 0; i < OTTO_PAR_N; i++) {
        g_par[i] = (i < npar) ? par[i] : act->dflt[i];
    }
    g_act = act;
    g_pend = 1;                                /* 最后置位,任务侧先取参后清 */
    return 0;
}

void OttoPcaStop(void)
{
    g_stopReq = 1;
}

int32_t OttoPcaSetTrim(int limb, int val)
{
    if ((limb < 0) || (limb >= OTTO_LIMB_N)) {
        return -3;
    }
    if ((val < -OTTO_TRIM_MAX) || (val > OTTO_TRIM_MAX)) {
        return -3;
    }
    g_trim[limb] = val;
    return 0;
}

int32_t OttoPcaGetTrim(int limb, int *val)
{
    if ((limb < 0) || (limb >= OTTO_LIMB_N) || (val == NULL)) {
        return -3;
    }
    *val = g_trim[limb];
    return 0;
}

void OttoPcaList(void)
{
    const struct OttoAct *a;

    PRINTK("otto actions (pca otto <name> [par...], 缺省用默认值):\n");
    for (a = g_acts; a->name != NULL; a++) {
        if (a->desc != NULL) {
            PRINTK("  %-10s %s\n", a->name, a->desc);
        }
    }
    PRINTK("  %-10s %s\n", "stop", "停止当前动作并归位");
    PRINTK("  %-10s %s\n", "pose", "对照:最近otto角->应到通道角");
    PRINTK("  %-10s %s\n", "trim", "trim <ll|rl|lf|rf|lh|rh> [val -50~50]");
}

void OttoPcaMap(void)
{
    int32_t i;

    OttoInitTables();                          /* 未跑过动作时也按宏显示默认映射 */
    PRINTK("otto limb map (顺序 ll rl lf rf lh rh):\n");
    for (i = 0; i < OTTO_LIMB_N; i++) {
        PRINTK("  %s -> ch %u, trim %d, rev %u\n",
               g_limbName[i], g_limbCh[i], g_trim[i], g_rev[i]);
    }
    PRINTK("  angle map: otto 0..180 -> servo %d..%d deg\n",
           OTTO_ANG_MAP_MIN, OTTO_ANG_MAP_MAX);
    PRINTK("  task:%s homed:%d stop:%d\n",
           (g_taskOn != 0) ? "on" : "off", g_homed, g_stopReq);
}

/* 逐肢体对照:最近下发的 otto 逻辑角 -> 映射舵机角。
 * PWM 舵机无位置反馈,校准映射/trim/rev 时先 stop 或 sit 固定姿态,
 * 肉眼比对实际姿态与期望角 */
void OttoPcaPose(void)
{
    int32_t i;

    OttoInitTables();
    PRINTK("otto pose (肢体 最近otto角 -> 应到通道角,PWM舵机无反馈):\n");
    for (i = 0; i < OTTO_LIMB_N; i++) {
        int32_t want = OttoDeg(i, g_cur[i]);

        PRINTK("  %s  ch %u  otto %4d -> ch%u %4d deg\n",
               g_limbName[i], g_limbCh[i], g_cur[i], g_limbCh[i], want);
    }
    PRINTK("  (姿态不对时调 trim/rev/OTTO_LIMB_CH_TAB)\n");
}

/* ================= 逐动作便捷接口(业务推荐入口) =================
 * 约定:OttoPcaXxx(void) = 动作表默认参数,一行调用;
 *       OttoPcaXxxEx(...) = 全参数,形参顺序与 OttoPcaList 输出一致。
 * 全部阻塞在调用者任务里执行,结束自动归位(sit/handsup/showcase/home 除外);
 * 返回 0=完成,-1=中途被 stop;要异步投后台任务用 OttoPcaPost */
int32_t OttoPcaWalk(void) { return OttoPcaRun("walk", NULL, 0); }
int32_t OttoPcaTurn(void) { return OttoPcaRun("turn", NULL, 0); }
int32_t OttoPcaJump(void) { return OttoPcaRun("jump", NULL, 0); }
int32_t OttoPcaBend(void) { return OttoPcaRun("bend", NULL, 0); }
int32_t OttoPcaShakeleg(void) { return OttoPcaRun("shakeleg", NULL, 0); }
int32_t OttoPcaSit(void) { return OttoPcaRun("sit", NULL, 0); }
int32_t OttoPcaUpdown(void) { return OttoPcaRun("updown", NULL, 0); }
int32_t OttoPcaSwing(void) { return OttoPcaRun("swing", NULL, 0); }
int32_t OttoPcaTiptoe(void) { return OttoPcaRun("tiptoe", NULL, 0); }
int32_t OttoPcaJitter(void) { return OttoPcaRun("jitter", NULL, 0); }
int32_t OttoPcaAscend(void) { return OttoPcaRun("ascend", NULL, 0); }
int32_t OttoPcaCrusaito(void) { return OttoPcaRun("crusaito", NULL, 0); }
int32_t OttoPcaFlapping(void) { return OttoPcaRun("flapping", NULL, 0); }
int32_t OttoPcaMoonwalk(void) { return OttoPcaRun("moonwalk", NULL, 0); }
int32_t OttoPcaWhirlwind(void) { return OttoPcaRun("whirlwind", NULL, 0); }
int32_t OttoPcaMagic(void) { return OttoPcaRun("magic", NULL, 0); }
int32_t OttoPcaHandsup(void) { return OttoPcaRun("handsup", NULL, 0); }
int32_t OttoPcaHandsdown(void) { return OttoPcaRun("handsdown", NULL, 0); }
int32_t OttoPcaHandwave(void) { return OttoPcaRun("handwave", NULL, 0); }
int32_t OttoPcaWindmill(void) { return OttoPcaRun("windmill", NULL, 0); }
int32_t OttoPcaTakeoff(void) { return OttoPcaRun("takeoff", NULL, 0); }
int32_t OttoPcaFitness(void) { return OttoPcaRun("fitness", NULL, 0); }
int32_t OttoPcaGreeting(void) { return OttoPcaRun("greeting", NULL, 0); }
int32_t OttoPcaShy(void) { return OttoPcaRun("shy", NULL, 0); }
int32_t OttoPcaCalisthenics(void) { return OttoPcaRun("calisthenics", NULL, 0); }
int32_t OttoPcaShowcase(void) { return OttoPcaRun("showcase", NULL, 0); }
int32_t OttoPcaHome(void) { return OttoPcaRun("home", NULL, 0); }

int32_t OttoPcaWalkEx(int32_t steps, int32_t periodMs, int32_t dir, int32_t arm)
{
    int par[OTTO_PAR_N] = { steps, periodMs, dir, arm };
    return OttoPcaRun("walk", par, OTTO_PAR_N);
}

int32_t OttoPcaTurnEx(int32_t steps, int32_t periodMs, int32_t dir, int32_t arm)
{
    int par[OTTO_PAR_N] = { steps, periodMs, dir, arm };
    return OttoPcaRun("turn", par, OTTO_PAR_N);
}

int32_t OttoPcaJumpEx(int32_t times, int32_t periodMs)
{
    int par[OTTO_PAR_N] = { times, periodMs };
    return OttoPcaRun("jump", par, 2);
}

int32_t OttoPcaBendEx(int32_t steps, int32_t periodMs, int32_t dir)
{
    int par[OTTO_PAR_N] = { steps, periodMs, dir };
    return OttoPcaRun("bend", par, 3);
}

int32_t OttoPcaShakelegEx(int32_t steps, int32_t periodMs, int32_t dir)
{
    int par[OTTO_PAR_N] = { steps, periodMs, dir };
    return OttoPcaRun("shakeleg", par, 3);
}

int32_t OttoPcaUpdownEx(int32_t steps, int32_t periodMs, int32_t h)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h };
    return OttoPcaRun("updown", par, 3);
}

int32_t OttoPcaSwingEx(int32_t steps, int32_t periodMs, int32_t h)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h };
    return OttoPcaRun("swing", par, 3);
}

int32_t OttoPcaTiptoeEx(int32_t steps, int32_t periodMs, int32_t h)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h };
    return OttoPcaRun("tiptoe", par, 3);
}

int32_t OttoPcaJitterEx(int32_t steps, int32_t periodMs, int32_t h)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h };
    return OttoPcaRun("jitter", par, 3);
}

int32_t OttoPcaAscendEx(int32_t steps, int32_t periodMs, int32_t h)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h };
    return OttoPcaRun("ascend", par, 3);
}

int32_t OttoPcaCrusaitoEx(int32_t steps, int32_t periodMs, int32_t h, int32_t dir)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h, dir };
    return OttoPcaRun("crusaito", par, OTTO_PAR_N);
}

int32_t OttoPcaFlappingEx(int32_t steps, int32_t periodMs, int32_t h, int32_t dir)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h, dir };
    return OttoPcaRun("flapping", par, OTTO_PAR_N);
}

int32_t OttoPcaMoonwalkEx(int32_t steps, int32_t periodMs, int32_t h, int32_t dir)
{
    int par[OTTO_PAR_N] = { steps, periodMs, h, dir };
    return OttoPcaRun("moonwalk", par, OTTO_PAR_N);
}

int32_t OttoPcaWhirlwindEx(int32_t steps, int32_t periodMs, int32_t amp)
{
    int par[OTTO_PAR_N] = { steps, periodMs, amp };
    return OttoPcaRun("whirlwind", par, 3);
}

int32_t OttoPcaHandsupEx(int32_t periodMs, int32_t dir)
{
    int par[OTTO_PAR_N] = { periodMs, dir };
    return OttoPcaRun("handsup", par, 2);
}

int32_t OttoPcaHandsdownEx(int32_t periodMs, int32_t dir)
{
    int par[OTTO_PAR_N] = { periodMs, dir };
    return OttoPcaRun("handsdown", par, 2);
}

int32_t OttoPcaHandwaveEx(int32_t dir)
{
    int par[OTTO_PAR_N] = { dir };
    return OttoPcaRun("handwave", par, 1);
}

int32_t OttoPcaWindmillEx(int32_t steps, int32_t periodMs, int32_t amp)
{
    int par[OTTO_PAR_N] = { steps, periodMs, amp };
    return OttoPcaRun("windmill", par, 3);
}

int32_t OttoPcaTakeoffEx(int32_t steps, int32_t periodMs, int32_t amp)
{
    int par[OTTO_PAR_N] = { steps, periodMs, amp };
    return OttoPcaRun("takeoff", par, 3);
}

int32_t OttoPcaFitnessEx(int32_t steps, int32_t periodMs, int32_t amp)
{
    int par[OTTO_PAR_N] = { steps, periodMs, amp };
    return OttoPcaRun("fitness", par, 3);
}

int32_t OttoPcaGreetingEx(int32_t dir, int32_t steps)
{
    int par[OTTO_PAR_N] = { dir, steps };
    return OttoPcaRun("greeting", par, 2);
}

int32_t OttoPcaShyEx(int32_t dir, int32_t steps)
{
    int par[OTTO_PAR_N] = { dir, steps };
    return OttoPcaRun("shy", par, 2);
}

/* ================= 后台动作任务 ================= */
static int OttoParseInt(const char *s, int32_t *v)
{
    int32_t neg = 0;
    int32_t x = 0;

    if ((s == NULL) || (*s == '\0')) {
        return -1;
    }
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    if (*s == '\0') {
        return -1;
    }
    while (*s != '\0') {
        if ((*s < '0') || (*s > '9')) {
            return -1;
        }
        x = x * 10 + (*s - '0');
        s++;
    }
    *v = neg ? -x : x;
    return 0;
}

int32_t OttoPcaShellCmd(int argc, const char **argv)
{
    OttoInitTables();                          /* map/trim 查询入口也可能先到 */
    if (argc <= 0) {
        OttoPcaMap();
        OttoPcaList();
        return 0;
    }
    if (strcmp(argv[0], "stop") == 0) {
        OttoPcaStop();
        PRINTK("otto: stop requested (will home)\n");
        return 0;
    }
    if (strcmp(argv[0], "list") == 0) {
        OttoPcaList();
        return 0;
    }
    if (strcmp(argv[0], "map") == 0) {
        OttoPcaMap();
        return 0;
    }
    if (strcmp(argv[0], "pose") == 0) {
        OttoPcaPose();
        return 0;
    }
    if (strcmp(argv[0], "trim") == 0) {
        int32_t limb = -1;
        int32_t val;
        int32_t i;

        if (argc < 2) {
            PRINTK("usage: pca otto trim <ll|rl|lf|rf|lh|rh> [val -50~50]\n");
            return 0;
        }
        for (i = 0; i < OTTO_LIMB_N; i++) {
            if (strcmp(argv[1], g_limbName[i]) == 0) {
                limb = i;
            }
        }
        if (limb < 0) {
            PRINTK("otto: bad limb %s\n", argv[1]);
            return 0;
        }
        if (argc >= 3) {
            if ((OttoParseInt(argv[2], &val) != 0) ||
                (OttoPcaSetTrim(limb, val) != 0)) {
                PRINTK("otto: trim val must be -50~50\n");
                return 0;
            }
        }
        (void)OttoPcaGetTrim(limb, &val);
        PRINTK("otto: %s trim=%d (立即生效,重启丢失,固化改 OTTO_LIMB_TRIM_TAB)\n",
               g_limbName[limb], val);
        return 0;
    }
    /* 其余按动作名处理 */
    {
        const struct OttoAct *act = OttoFind(argv[0]);
        int32_t par[OTTO_PAR_N];
        int32_t i;

        if (act == NULL) {
            PRINTK("otto: unknown action '%s'\n", argv[0]);
            OttoPcaList();
            return 0;
        }
        for (i = 0; i < act->npar; i++) {
            if ((argc >= 2 + i) &&
                (OttoParseInt(argv[1 + i], &par[i]) == 0)) {
                continue;
            }
            par[i] = act->dflt[i];             /* 缺省参数 */
        }
        if (OttoPcaPost(argv[0], par, act->npar) != 0) {
            PRINTK("otto: post fail (pca9685 not ready?)\n");
            return 0;
        }
        PRINTK("otto: %s queued (par:", argv[0]);
        for (i = 0; i < act->npar; i++) {
            PRINTK(" %d", par[i]);
        }
        PRINTK("), run 'pca otto stop' to abort\n");
    }
    return 0;
}

/* 后台任务:取动作 -> 执行 -> 自动归位;空闲时响应 stop(仅归位) */
static void *OttoTaskLoop(void *arg)
{
    (void)arg;
    for (;;) {
        const struct OttoAct *act;
        int32_t par[OTTO_PAR_N];
        int32_t i;

        if (g_stopReq != 0) {                  /* 空闲期 stop = 仅归位 */
            g_stopReq = 0;
            (void)OttoHomeCore();
            continue;
        }
        if (g_pend == 0) {
            PcaDelayMs(20);
            continue;
        }
        act = g_act;
        for (i = 0; i < OTTO_PAR_N; i++) {
            par[i] = g_par[i];
        }
        g_pend = 0;
        g_stopReq = 0;
        (void)OttoExecAct(act, par);
    }
    return NULL;
}
