/*
 * pca_demo.c - PCA9685/Otto SDK 一次性命令行测试程序(编译为 /bin/pca_demo)
 *
 * 参照 llm_sdk 交付的经验:一次性命令不读串口输入,参数全由 shell 传递,
 * 绕开 LiteOS shell 与用户程序抢控制台输入的坑。
 *
 * 串口0 用法:
 *   exec /bin/pca_demo scan [bus]           扫描 I2C 总线(默认 6)
 *   exec /bin/pca_demo init [bus] [addr]    初始化(默认 bus=6 addr=0x40)
 *   exec /bin/pca_demo angle <ch> <deg>     单舵机角度(自动先初始化)
 *   exec /bin/pca_demo us <ch> <us>         直接设脉宽 500~2500
 *   exec /bin/pca_demo off                  全通道停发脉冲(舵机松弛)
 *   exec /bin/pca_demo status               本进程就绪态
 *   exec /bin/pca_demo run <动作> [参数...] 阻塞执行动作(如 run walk 2 800 1 0)
 *   exec /bin/pca_demo stop                 全通道停发(等价 off;跨进程无法
 *                                           中止另一进程正在跑的动作)
 *   exec /bin/pca_demo list|map|pose        动作表/映射/对照
 *   exec /bin/pca_demo trim <limb> <val>    微调(仅本次进程,重启丢失)
 *
 * 说明:每次 exec 都是新进程,SDK 状态不跨进程共享;动作/角度命令都会
 * 自动先 PcaInit(幂等,几 ms),拿到命令行返回即动作已完成。
 * 常驻业务进程请直接用 C API(见 pca9685.h / otto_pca.h)。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pca9685.h"
#include "otto_pca.h"

static void PcaDemoUsage(void)
{
    printf("usage: pca_demo <cmd> [args...]\n");
    printf("  scan [bus]            scan i2c bus (default 6)\n");
    printf("  init [bus] [addr]     init chip (default bus=6 addr=0x40)\n");
    printf("  angle <ch> <deg>      servo angle 0~180 (auto init)\n");
    printf("  us <ch> <us>          pulse width 500~2500 (auto init)\n");
    printf("  off                   all channels off (servos relax)\n");
    printf("  status                show this-process status\n");
    printf("  run <action> [par...] run action, blocking (ex: run walk 2 800 1 0)\n");
    printf("  stop                  all off (same-process abort: use API)\n");
    printf("  list | map | pose     action list / limb map / pose check\n");
    printf("  trim <ll|rl|lf|rf|lh|rh> <-50~50>\n");
}

/* 解析整数(支持负数),失败返回 -1 */
static int PcaDemoParseInt(const char *s, int32_t *v)
{
    char *end = NULL;
    long x;

    if ((s == NULL) || (*s == '\0')) {
        return -1;
    }
    x = strtol(s, &end, 10);
    if ((end == NULL) || (*end != '\0')) {
        return -1;
    }
    *v = (int32_t)x;
    return 0;
}

/* 动作命令统一入口:自动补初始化 -> 阻塞执行 -> 打印结果 */
static int PcaDemoRunAction(const char *name, int32_t *par, int npar)
{
    int32_t p[4] = {0};
    int32_t i;
    int32_t ret;

    for (i = 0; i < npar; i++) {
        p[i] = par[i];
    }
    ret = OttoPcaRun(name, p, npar);
    if (ret == 0) {
        printf("pca_demo: %s done\n", name);
    } else {
        printf("pca_demo: %s ret=%d (0完成 -1中止/未就绪 -3动作名未知)\n", name, ret);
    }
    return (ret == 0) ? 0 : 1;
}

int main(int argc, char **argv)
{
    int32_t v[4] = {0};
    int32_t npar = 0;
    int32_t i;

    if (argc < 2) {
        PcaDemoUsage();
        return 1;
    }

    if (strcmp(argv[1], "scan") == 0) {
        uint8_t bus = PCA_BUS_DEFAULT;

        if ((argc >= 3) && (PcaDemoParseInt(argv[2], &v[0]) == 0)) {
            bus = (uint8_t)v[0];
        }
        return (PcaScanBus(bus) >= 0) ? 0 : 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        uint8_t bus = PCA_BUS_DEFAULT;
        uint8_t addr = PCA_ADDR_DEFAULT;

        if (argc >= 3) {
            if ((PcaDemoParseInt(argv[2], &v[0]) != 0) || (v[0] < 0) || (v[0] > 0xFF)) {
                printf("pca_demo: bad bus\n");
                return 1;
            }
            bus = (uint8_t)v[0];
        }
        if (argc >= 4) {
            if ((PcaDemoParseInt(argv[3], &v[1]) != 0) || (v[1] < 0) || (v[1] > 0x7F)) {
                printf("pca_demo: bad addr (7bit, ex 0x40 = 64)\n");
                return 1;
            }
            addr = (uint8_t)v[1];
        }
        return (PcaInitEx(bus, addr) == HDF_SUCCESS) ? 0 : 1;
    }

    if (strcmp(argv[1], "angle") == 0) {
        if ((argc < 4) || (PcaDemoParseInt(argv[2], &v[0]) != 0) ||
            (PcaDemoParseInt(argv[3], &v[1]) != 0)) {
            printf("usage: pca_demo angle <ch 0~15> <deg 0~180>\n");
            return 1;
        }
        if (PcaInit() != HDF_SUCCESS) {
            printf("pca_demo: init fail (先 scan 排查接线)\n");
            return 1;
        }
        return (PcaServoAngle((uint8_t)v[0], (uint32_t)v[1]) == HDF_SUCCESS) ? 0 : 1;
    }

    if (strcmp(argv[1], "us") == 0) {
        if ((argc < 4) || (PcaDemoParseInt(argv[2], &v[0]) != 0) ||
            (PcaDemoParseInt(argv[3], &v[1]) != 0)) {
            printf("usage: pca_demo us <ch 0~15> <us 500~2500>\n");
            return 1;
        }
        if (PcaInit() != HDF_SUCCESS) {
            printf("pca_demo: init fail (先 scan 排查接线)\n");
            return 1;
        }
        return (PcaServoUs((uint8_t)v[0], (uint32_t)v[1]) == HDF_SUCCESS) ? 0 : 1;
    }

    if ((strcmp(argv[1], "off") == 0) || (strcmp(argv[1], "stop") == 0)) {
        if (PcaInit() != HDF_SUCCESS) {
            printf("pca_demo: init fail (先 scan 排查接线)\n");
            return 1;
        }
        (void)OttoPcaStop();                 /* 本进程若有动作,请求中止 */
        if (PcaAllChannelsOff() == HDF_SUCCESS) {
            printf("pca_demo: all channels off (舵机松弛;跨进程动作无法由此中止)\n");
            return 0;
        }
        return 1;
    }

    if (strcmp(argv[1], "status") == 0) {
        uint8_t bus;
        uint8_t addr;
        int32_t ready;

        PcaGetStatus(&bus, &addr, &ready);
        printf("pca_demo: bus=%u addr=0x%02x ready=%d (本进程状态,跨进程不共享)\n",
               bus, addr, ready);
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        OttoPcaList();
        return 0;
    }
    if (strcmp(argv[1], "map") == 0) {
        OttoPcaMap();
        return 0;
    }
    if (strcmp(argv[1], "pose") == 0) {
        OttoPcaPose();
        return 0;
    }

    if (strcmp(argv[1], "trim") == 0) {
        if (argc < 4) {
            printf("usage: pca_demo trim <ll|rl|lf|rf|lh|rh> <-50~50>\n");
            return 1;
        }
        for (i = 2; i < 4; i++) {
            if (PcaDemoParseInt(argv[i], &v[i - 2]) != 0) {
                printf("pca_demo: bad arg %s\n", argv[i]);
                return 1;
            }
        }
        if (OttoPcaSetTrim(v[0], v[1]) != 0) {
            printf("pca_demo: trim fail (limb 名字错或 val 超 -50~50)\n");
            return 1;
        }
        printf("pca_demo: trim ok (仅本次进程生效,固化改 OTTO_LIMB_TRIM_TAB 重编)\n");
        return 0;
    }

    if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            OttoPcaList();
            return 1;
        }
        for (i = 3; i < argc; i++) {
            if (npar >= 4) {
                printf("pca_demo: too many params (max 4)\n");
                return 1;
            }
            if (PcaDemoParseInt(argv[i], &v[npar]) != 0) {
                printf("pca_demo: bad param %s\n", argv[i]);
                return 1;
            }
            npar++;
        }
        return PcaDemoRunAction(argv[2], v, npar);
    }

    printf("pca_demo: unknown cmd '%s'\n", argv[1]);
    PcaDemoUsage();
    return 1;
}
