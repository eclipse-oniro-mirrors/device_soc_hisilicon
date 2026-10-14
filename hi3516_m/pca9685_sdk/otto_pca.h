/*
 * otto_pca.h - Otto 双足机器人六舵机协同动作控制(PCA9685 用户态 SDK 版)
 * 与内核版(mytest/pca9685/otto_pca.h)同名同 API;动作表/参数/语义一致,
 * 参考固件为 xiaozhi-esp32 otto-robot:
 *   [0]=ll 左腿 [1]=rl 右腿 [2]=lf 左脚 [3]=rf 右脚 [4]=lh 左手 [5]=rh 右手
 * 动作角度语义沿用参考固件:腿/脚 90=直立,手 45/135=垂手;左右镜像已内含在
 * 各动作参数里,装反时用 OTTO_LIMB_REV_TAB 翻转,不用改动作表。
 *
 * 与总线舵机版(bridge)的功能差异(PWM 舵机没有的能力,其余逐一对齐):
 *   1. 无"定时到达"寄存器 -> OttoMove 恢复参考固件原版 EaseOutCubic 软件 10ms 插值;
 *   2. 无位置反馈 -> otto pose 只显示逻辑角与映射角,无 real 列;
 *   3. 无扭矩开关 -> 上电即有力,断力用 pca stop(全通道无脉冲)。
 *
 * 接线:6 个舵机插 PCA9685 CH0..CH5(顺序 ll rl lf rf lh rh,见 OTTO_LIMB_CH_TAB),
 * 舵机电源全部并到 V+(外部 5V 供电,共地);业务代码先 PcaInit() 确认芯片在线。
 *
 * 用法(C API):
 *   OttoPcaWalk();                    无参 = 默认参数,阻塞到做完
 *   OttoPcaWalkEx(4, 1000, 1, 0);     Ex = 全参数
 *   通用接口:OttoPcaRun("walk", par, npar)(npar 不满自动补默认值)
 *   异步投后台任务:OttoPcaPost("walk", NULL, 0)(常驻进程里用,进程退出动作即断)
 *
 * OttoPcaShellCmd(argc, argv):argv 风格动作入口(与内核版 pca otto ... 参数一致),
 * 供交互式程序复用;内部动作走 OttoPcaPost。
 */
#ifndef OTTO_PCA_SDK_H
#define OTTO_PCA_SDK_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 肢体下标(数组/trim/rev 表都用这个顺序) ---- */
#define OTTO_LL              0           /* 左腿 */
#define OTTO_RL              1           /* 右腿 */
#define OTTO_LF              2           /* 左脚 */
#define OTTO_RF              3           /* 右脚 */
#define OTTO_LH              4           /* 左手 */
#define OTTO_RH              5           /* 右手 */

/* ---- 可调配置(改完重新编译生效) ----
 * 肢体 -> PCA9685 通道映射,按实际接线改;如左手接在 CH5:
 * 第 5 个数就是 5(顺序 ll rl lf rf lh rh) */
#define OTTO_LIMB_CH_TAB     { 0, 1, 2, 3, 4, 5 }

/* 每肢体微调(度,-50~50):输出角 = 逻辑角 + trim,先于 0~180 裁剪 */
#define OTTO_LIMB_TRIM_TAB   { 0, 0, 0, 0, 0, 0 }

/* 方向反转:某肢体安装与参考镜像时置 1(输出 = 180 - 角度) */
#define OTTO_LIMB_REV_TAB    { 0, 0, 0, 0, 0, 0 }

/* otto 0~180° -> 舵机角度区间映射(默认 1:1;映射结果超出 0~180 时
 * 底层 PcaServoAngle 会裁到 0~180,PWM 舵机行程就这么多) */
#define OTTO_ANG_MAP_MIN     0
#define OTTO_ANG_MAP_MAX     180

#define OTTO_HAND_HOME       45          /* 手 HOME 角(参考固件同值) */
#define OTTO_SAMPLE_MS       30          /* 振荡采样周期 ms(参考固件同值) */
#define OTTO_TASK_PRIO       25          /* otto 动作任务优先级(用户态 pthread 不生效,保留给内核版) */
#define OTTO_TASK_STACK      0x2000      /* 动作任务栈 8KB(用户态 pthread 用默认栈,保留给内核版) */

/* ---- 初始化/任务 ----
 * OttoPcaStart:确保 PCA9685 就绪(幂等)+ 按宏初始化映射/trim 表 + 建后台动作任务;
 * 异步接口 OttoPcaPost 前会自动调用;
 * 业务用"逐动作便捷接口"(阻塞直调)时可以不建后台任务,不必调它 */
int32_t OttoPcaStart(void);

/* ---- 通用动作执行(名字驱动) ----
 * name=动作名(见 OttoPcaList 输出),par 参数个数 npar 可少于该动作参数数,
 * 缺省用默认值;阻塞执行,返回 0=完成,-1=中途 stop/参数未知 */
int32_t OttoPcaRun(const char *name, const int *par, int npar);

/* 投递给后台 otto 任务异步执行(立即返回;同一时刻只缓存 1 个待执行动作,
 * 重复投递会覆盖上一个未开始的)。仅适合常驻进程:进程退出动作即断 */
int32_t OttoPcaPost(const char *name, const int *par, int npar);

/* 请求停止:当前动作尽快中止 -> 自动归位;空闲时仅归位(本进程内有效) */
void OttoPcaStop(void);

/* ---- 逐动作便捷接口(业务推荐入口) ----
 * 约定:
 *   OttoPcaXxx(void)    无参 = 动作默认参数,一行调用;
 *   OttoPcaXxxEx(...)   全参数版,形参顺序与 OttoPcaList 输出一致。
 * 全部阻塞执行:直接在调用者任务里跑,动作结束自动归位(sit/handsup/
 * showcase/home 除外);返回 0=完成,-1=中途被 OttoPcaStop 中止。
 * 行注释为默认参数值。 */
int32_t OttoPcaWalk(void);                        /* 前走 4步 1000ms dir=1 不摆臂 */
int32_t OttoPcaWalkEx(int32_t steps, int32_t periodMs, int32_t dir, int32_t arm);
int32_t OttoPcaTurn(void);                        /* 转弯 4步 1000ms dir=1左 不摆臂 */
int32_t OttoPcaTurnEx(int32_t steps, int32_t periodMs, int32_t dir, int32_t arm);
int32_t OttoPcaJump(void);                        /* 蹲->站 1次 1000ms */
int32_t OttoPcaJumpEx(int32_t times, int32_t periodMs);
int32_t OttoPcaBend(void);                        /* 侧弯 3次 1000ms dir=1左 */
int32_t OttoPcaBendEx(int32_t steps, int32_t periodMs, int32_t dir);
int32_t OttoPcaShakeleg(void);                    /* 抬腿摇 1次 2000ms dir=1左腿 */
int32_t OttoPcaShakelegEx(int32_t steps, int32_t periodMs, int32_t dir);
int32_t OttoPcaSit(void);                         /* 坐下(不归位) */
int32_t OttoPcaUpdown(void);                      /* 一起一落 4步 1000ms h=30 */
int32_t OttoPcaUpdownEx(int32_t steps, int32_t periodMs, int32_t h);
int32_t OttoPcaSwing(void);                       /* 左右摇摆 4步 1000ms h=30 */
int32_t OttoPcaSwingEx(int32_t steps, int32_t periodMs, int32_t h);
int32_t OttoPcaTiptoe(void);                      /* 踮脚摆 4步 1000ms h=30 */
int32_t OttoPcaTiptoeEx(int32_t steps, int32_t periodMs, int32_t h);
int32_t OttoPcaJitter(void);                      /* 抖动 4步 1000ms h=20 */
int32_t OttoPcaJitterEx(int32_t steps, int32_t periodMs, int32_t h);
int32_t OttoPcaAscend(void);                      /* 螺旋升天 4步 1000ms h=13 */
int32_t OttoPcaAscendEx(int32_t steps, int32_t periodMs, int32_t h);
int32_t OttoPcaCrusaito(void);                    /* 十字步 4步 1000ms h=25 dir=1前 */
int32_t OttoPcaCrusaitoEx(int32_t steps, int32_t periodMs, int32_t h, int32_t dir);
int32_t OttoPcaFlapping(void);                    /* 拍翅 4步 1000ms h=25 dir=1前 */
int32_t OttoPcaFlappingEx(int32_t steps, int32_t periodMs, int32_t h, int32_t dir);
int32_t OttoPcaMoonwalk(void);                    /* 太空步 3步 900ms h=25 dir=1左 */
int32_t OttoPcaMoonwalkEx(int32_t steps, int32_t periodMs, int32_t h, int32_t dir);
int32_t OttoPcaWhirlwind(void);                   /* 旋风腿 2步 700ms amp=30 */
int32_t OttoPcaWhirlwindEx(int32_t steps, int32_t periodMs, int32_t amp);
int32_t OttoPcaMagic(void);                       /* 转圈圈 700ms*40(约28s) */
int32_t OttoPcaHandsup(void);                     /* 举手 800ms dir=0双手(不归位) */
int32_t OttoPcaHandsupEx(int32_t periodMs, int32_t dir);
int32_t OttoPcaHandsdown(void);                   /* 放手 800ms dir=0双手 */
int32_t OttoPcaHandsdownEx(int32_t periodMs, int32_t dir);
int32_t OttoPcaHandwave(void);                    /* 挥手 dir=1左手 5次 */
int32_t OttoPcaHandwaveEx(int32_t dir);
int32_t OttoPcaWindmill(void);                    /* 大风车 5次 300ms amp=40 */
int32_t OttoPcaWindmillEx(int32_t steps, int32_t periodMs, int32_t amp);
int32_t OttoPcaTakeoff(void);                     /* 起飞扑扇 5次 300ms amp=40 */
int32_t OttoPcaTakeoffEx(int32_t steps, int32_t periodMs, int32_t amp);
int32_t OttoPcaFitness(void);                     /* 健身 5次 1000ms amp=25 */
int32_t OttoPcaFitnessEx(int32_t steps, int32_t periodMs, int32_t amp);
int32_t OttoPcaGreeting(void);                    /* 打招呼 dir=1左 5次 */
int32_t OttoPcaGreetingEx(int32_t dir, int32_t steps);
int32_t OttoPcaShy(void);                         /* 害羞 dir=1左 5次 */
int32_t OttoPcaShyEx(int32_t dir, int32_t steps);
int32_t OttoPcaCalisthenics(void);                /* 广播体操 4节(约32s) */
int32_t OttoPcaShowcase(void);                    /* 动作串演(约2~3min,不归位) */
int32_t OttoPcaHome(void);                        /* 归位 [90 90 90 90 45 135] */

/* ---- trim(运行期微调,不落盘;想固化改 OTTO_LIMB_TRIM_TAB 重编) ---- */
int32_t OttoPcaSetTrim(int limb, int val);     /* val 超 -50~50 返回 -3 */
int32_t OttoPcaGetTrim(int limb, int *val);

/* ---- 信息打印 ---- */
void OttoPcaList(void);                        /* 动作表:名字/参数/默认值 */
void OttoPcaMap(void);                         /* 肢体->通道/trim/rev + 角度映射 */
void OttoPcaPose(void);                        /* 逐肢体:最近otto角->应到通道角 */

/* ---- argv 风格动作入口(与内核版 pca otto 之后的参数一致) ----
 * argc/argv 为动作参数,如 {"walk","4","1000"};内部处理 stop/list/map/pose/
 * trim/动作名;动作走 OttoPcaPost 后台任务(要求调用进程常驻) */
int32_t OttoPcaShellCmd(int argc, const char **argv);

#ifdef __cplusplus
}
#endif
#endif /* OTTO_PCA_SDK_H */
