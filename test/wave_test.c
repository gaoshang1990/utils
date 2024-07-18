
#include "utils_log.h"


//! AD通道定义
enum {
    ID_AD_UA,
    ID_AD_UB,
    ID_AD_UC,
    // ID_AD_UZ,
    ID_AD_IA,
    ID_AD_IB,
    ID_AD_IC,
    // ID_AD_IZ,

    NUM_AD_COUNT // 模拟量通道数
};


enum {
    ID_COMMAND_START = 1, // 启动
    ID_COMMAND_STOP,      // 停止
    ID_COMMAND_UI,        // UI手动测试
    ID_COMMAND_HARM,      // 谐波测试
    ID_COMMAND_STATE,     // 状态序列
    ID_COMMAND_WAVE_PALY, // 波形回放

    ID_COMMAND_IMPACT, /* 冲击负荷 */
    ID_COMMAND_EXP,    /* 指数负荷 */
    ID_COMMAND_OSCI,   /* 振荡负荷 */
    ID_COMMAND_NOISE,  /* 重噪声负荷 */
    ID_COMMAND_DC,     /* 直流偏磁负荷 */
};


//! AD通道配置
typedef struct {
    float fAmplitude; // 幅度值
    float fPhase;     // 初始相角
    float fFrequency; // 频率
    int   bIsDC;      // 是否直流通道
} T_ADChannelConfig;


//! 源打开
typedef struct {
    // IN
    int nMode; // 指定当前测试模式, 0xFF表示不改变当前模式

    // OUT
    char szErrorCode[64];
} TPTS_Start;


//! 源施加电压电流
typedef struct {
    // IN
    T_ADChannelConfig adChannels[NUM_AD_COUNT]; // 当前通道配置
    int bIsDirectOutput; // 是否直接输出，如果0控制参数将先存储，通过PTS_Start生效

    // OUT
    char szErrorCode[64];
} TPTS_SetUI;


#define MAX_HAR_COUNT 300 // 最大谐波个数
typedef struct {
    int               nHarmCount;             // 谐波次数
    T_ADChannelConfig adHarms[MAX_HAR_COUNT]; // 谐波配置
} THarmADItem;


//! 源施加谐波
typedef struct {
    // IN
    THarmADItem adChannels[NUM_AD_COUNT]; // 当前通道配置
    int bIsDirectOutput; // 是否直接输出，如果0控制参数将先存储，通过PTS_Start生效

    // OUT
    char szErrorCode[64];
} TPTS_SetHarm;


//! 源配置状态序列
typedef struct {
    // IN


    // OUT
    char szErrorCode[64];
} TPTS_SetStateSequence;


//! 启动波形回放
typedef struct {
    // IN
    char szWaveFileName[64]; // 录波文件名
    uint16_t nReplayCount; // 回放次数，0xFFFF表示无限次，其他回放完后自动停止
    int   channel[6]; // Ua Ub Uc Ia Ib Ic对应的comtrade文件中的通道号
    float ratio[6];   // Ua Ub Uc Ia Ib Ic对应的比例系数

    // OUT
    char szErrorCode[64];
} TPTS_StartWaveReplay;


// 冲击item
typedef struct _T_TPTS_SetFh_Item {
    // 冲击类型 1:单频  2：混频  3.文件
    int nType;
    // 点数
    int nTotoalPointCount;
    // 单频
    TPTS_SetUI adChannel;
    // 混频
    TPTS_SetHarm adHarm;
    // TODO文件

} TPTS_SetFh_Item;

// 冲击负荷
typedef struct _T_TPTS_SetCj {
    // 循环周期 次数  500次  600次 不是时间
    float fCycleTimes;
    // 系数
    float ratio;
    // 冲击负荷状态数量
    int nCjCount;
    // 冲击负荷
    TPTS_SetFh_Item cjItems[4];

    // OUT
    char szErrorCode[64];
} TPTS_SetCj;


// 指数负荷
typedef struct _T_TPTS_SetZs {
    // 指数负荷
    TPTS_SetFh_Item cjItems[2];
    // 指数函数底数
    float fa;
    // 持续时间
    float fHoldTime;

    // OUT
    char szErrorCode[64];
} TPTS_SetZs;


// 震荡负荷
typedef struct _T_TPTS_SetZd {
    // 震荡负荷
    /**
     * xu: 此处只配置了一个item, 那么什么时候开始振荡?
     *    1. 一直振荡?
     *    2. 开始输出时就振荡一次?
     */
    TPTS_SetFh_Item cjItems;
    // 幅值调制深度
    /* xu:是否每通道都要配置? */
    float fXm;
    // 调制频率
    float fm;
    // 幅值调制初相角
    float fPhase;
    // 持续时间
    float fHoldTime;

    // OUT
    char szErrorCode[64];
} TPTS_SetZd;


// 重噪声型负荷
typedef struct _T_TPTS_SetZzs {
    // 重噪声型负荷
    TPTS_SetFh_Item cjItems;
    // 配置噪声的幅值
    float fNm;

    // OUT
    char szErrorCode[64];
} TPTS_SetZzs;


// 直流偏磁型负荷
typedef struct _T_TPTS_SetZlpc {
    // 直流偏磁型负荷
    TPTS_SetFh_Item cjItems;
    // 配置直流分量的幅值
    float fD;
    // 幅值直流分量波动的大小
    float fXm;
    // 配置直流分量波动的快慢
    float fm;

    // OUT
    char szErrorCode[64];
} TPTS_SetZlpc;


// ZYNQ运行内存
typedef struct {
    // 下发的控制命令参数
    int                   nCommandID;
    TPTS_Start            theArg_TPTS_Start;
    TPTS_SetUI            theArg_TPTS_SetUI;
    TPTS_SetHarm          theArg_TPTS_SetHarm;
    TPTS_SetStateSequence theArg_TPTS_SetStateSequence;
    TPTS_StartWaveReplay  theArg_TPTS_StartWaveReplay;

    TPTS_SetCj   theArg_TPTS_SetCj;
    TPTS_SetZs   theArg_TPTS_SetZs;
    TPTS_SetZd   theArg_TPTS_SetZd;
    TPTS_SetZzs  theArg_TPTS_SetZzs;
    TPTS_SetZlpc theArg_TPTS_SetZlpc;
} TPTS_RUN_RAM;


typedef struct _LineParam_t_ {
    double k; /* 斜率 */
    double b; /* 截距 */
} LineParam;


/* 宽频负荷校正系数 */
typedef struct {
    LineParam calib;
    double    hz;
} WideCalib;


typedef struct _CalibCfg_t_bak_ {
    size_t    harm_num; /* 应小于等于64 */
    WideCalib wide[NUM_AD_COUNT][64];
} CalibCfg_bak;

typedef struct _CalibCfg_t_ {
    LineParam base;
    LineParam hz_1k;
    LineParam hz_5k;
    LineParam hz_10k;
    LineParam hz_15k;
} CalibCfg;


static CalibCfg_bak _calib_cfg_bak = {
    .harm_num = 5,
    .wide     = {{
                 /* UA */
                 {{0.9999569, -0.000367637}, 50},
                 {{1.00053, 0}, 1000},
                 {{1.01101, 0}, 5000},
                 {{1.04447, 0}, 10000},
                 {{1.10060, 0}, 15000},
             }, {
                 /* UB */
                 {{0.9999372, -0.000438018}, 50},
                 {{1.00233, 0}, 1000},
                 {{1.01559, 0}, 5000},
                 {{1.05001, 0}, 10000},
                 {{1.10403, 0}, 15000},
             }, {
                 /* UC */
                 {{0.9999229, -0.000471022}, 50},
                 {{1.00161, 0}, 1000},
                 {{1.01484, 0}, 5000},
                 {{1.05575, 0}, 10000},
                 {{1.12372, 0}, 15000},
             }, {
                 /* IA */
                 {{0.9999706, -0.000231264}, 50},
                 {{0.99735, 0}, 1000},
                 {{0.96710, 0}, 5000},
                 {{0.87295, 0}, 10000},
                 {{0.75905, 0}, 15000},
             }, {
                 /* IB */
                 {{1.0000126, -0.00052432}, 50},
                 {{0.99633, 0}, 1000},
                 {{0.96547, 0}, 5000},
                 {{0.86372, 0}, 10000},
                 {{0.74031, 0}, 15000},
             }, {
                 /* IC */
                 {{1.0000140, -0.000593133}, 50},
                 {{0.99645, 0}, 1000},
                 {{0.96339, 0}, 5000},
                 {{0.84851, 0}, 10000},
                 {{0.64030, 0}, 15000},
             }}
};


/* 各通道, 各频率的校正系数配置 */
static const CalibCfg _calib_cfg[NUM_AD_COUNT] = {
    {{0.9999569, -0.000367637}, {1.00053, 0}, {1.01101, 0}, {1.04447, 0}, {1.10060, 0}},
    {{0.9999372, -0.000438018}, {1.00233, 0}, {1.01559, 0}, {1.05001, 0}, {1.10403, 0}},
    {{0.9999229, -0.000471022}, {1.00161, 0}, {1.01484, 0}, {1.05575, 0}, {1.12372, 0}},
    {{0.9999706, -0.000231264}, {0.99735, 0}, {0.96710, 0}, {0.87295, 0}, {0.75905, 0}},
    {{1.0000126, -0.00052432},  {0.99633, 0}, {0.96547, 0}, {0.86372, 0}, {0.74031, 0}},
    {{1.0000140, -0.000593133}, {0.99645, 0}, {0.96339, 0}, {0.84851, 0}, {0.64030, 0}}
};

static LineParam _amp_calib[NUM_AD_COUNT][64]; /* 幅值校正系数, 最多支持64次谐波 */


/**
 * @brief   按通道获取幅值校正系数配置
 * @param   channel 通道号, 0~5: UA, UB, UC, IA, IB, IC
 * @return  const CalibCfg* 指向通道幅值校正系数的指针
 */
static const CalibCfg* _get_calib_cfg(int channel)
{
    return &_calib_cfg[channel];
}


/**
 * @brief   获取幅值校正系数
 * @param   channel 通道号, 0~5: UA, UB, UC, IA, IB, IC
 * @param   harm    谐波索引, 第几个谐波
 * @return  LineParam* 指向幅值校正系数的指针
 */
static LineParam* _get_amp_calib(int channel, int harm)
{
    return &_amp_calib[channel][harm];
}


static size_t _get_calib_harm_num()
{
    return _calib_cfg_bak.harm_num;
}


static WideCalib* _get_calib_chn_cfg(int channel)
{
    return &_calib_cfg_bak.wide[channel][0];
}


int premake_wave_harmonic(TPTS_SetHarm* harm_cfg, int item_id)
{
    SLOG_DEBUG("pre make harmonic wave:");

    for (int c = 0; c < ID_AD_UB; c++) { /* 只测试UA */

        for (int h = 0; h < harm_cfg->adChannels[c].nHarmCount; h++) {
            T_ADChannelConfig* harm  = &harm_cfg->adChannels[c].adHarms[h];
            const CalibCfg*    calib = _get_calib_cfg(c);

            float hz = harm->bIsDC ? 0 : harm->fFrequency;

            LineParam* line = _get_amp_calib(c, h);
#if 0
            /* 高频幅值校正 */
            if (hz <= 50) {
                line->k = 0;
                line->b = calib->base.k;
            }
            else if (hz <= 1000) {
                line->k = (calib->hz_1k.k - 1) / (1000 - 50); /* 50Hz时认为高频校正系数为1 */
                line->b = calib->hz_1k.k - 1000 * line->k;
            }
            else if (hz <= 5000) {
                line->k = (calib->hz_5k.k - calib->hz_1k.k) / (5000 - 1000);
                line->b = calib->hz_5k.k - 5000 * line->k;
            }
            else if (hz <= 10000) {
                line->k = (calib->hz_10k.k - calib->hz_5k.k) / (10000 - 5000);
                line->b = calib->hz_10k.k - 10000 * line->k;
            }
            else {
                line->k = (calib->hz_15k.k - calib->hz_10k.k) / (15000 - 10000);
                line->b = calib->hz_15k.k - 15000 * line->k;
            }
#else
            /* 替代方案 */
            const WideCalib* wide = _get_calib_chn_cfg(c);
            size_t           num  = _get_calib_harm_num();
            for (size_t i = 0; i < num; i++) {
                if (hz <= wide[i].hz) {
                    if (i == 0) { /* 小于等于配置的最小频率, 统一按照配置的系数进行校正,
                                     不作线性插值 */
                        line->k = 0;
                        line->b = wide[i].calib.k;
                    }
                    else {
                        line->k = (wide[i].calib.k - wide[i - 1].calib.k) /
                                  (wide[i].hz - wide[i - 1].hz);
                        line->b = wide[i].calib.k - wide[i].hz * line->k;
                    }
                    break;
                }
            }
#endif
            SLOG_DEBUG(
                "k[%d][%d] = %.8f, b[%d][%d] = %.8f", c, h, line->k, c, h, line->b);
        }
    }
    SLOG_DEBUG_RAW("\n");

    return 0;
}


int premake_test()
{
    TPTS_SetHarm set_harm;

    { /* 测试数据设置 */
        set_harm.adChannels[ID_AD_UA].nHarmCount = 10;

        set_harm.adChannels[ID_AD_UA].adHarms[0].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[0].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[0].fFrequency = 25;
        set_harm.adChannels[ID_AD_UA].adHarms[0].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[1].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[1].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[1].fFrequency = 50;
        set_harm.adChannels[ID_AD_UA].adHarms[1].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[2].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[2].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[2].fFrequency = 500;
        set_harm.adChannels[ID_AD_UA].adHarms[2].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[3].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[3].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[3].fFrequency = 1000;
        set_harm.adChannels[ID_AD_UA].adHarms[3].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[4].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[4].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[4].fFrequency = 2500;
        set_harm.adChannels[ID_AD_UA].adHarms[4].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[5].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[5].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[5].fFrequency = 5000;
        set_harm.adChannels[ID_AD_UA].adHarms[5].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[6].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[6].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[6].fFrequency = 7500;
        set_harm.adChannels[ID_AD_UA].adHarms[6].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[7].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[7].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[7].fFrequency = 10000;
        set_harm.adChannels[ID_AD_UA].adHarms[7].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[8].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[8].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[8].fFrequency = 12500;
        set_harm.adChannels[ID_AD_UA].adHarms[8].bIsDC      = 0;

        set_harm.adChannels[ID_AD_UA].adHarms[9].fAmplitude = 220;
        set_harm.adChannels[ID_AD_UA].adHarms[9].fPhase     = 0;
        set_harm.adChannels[ID_AD_UA].adHarms[9].fFrequency = 15000;
        set_harm.adChannels[ID_AD_UA].adHarms[9].bIsDC      = 0;
    }

    premake_wave_harmonic(&set_harm, 0);

    return 0;
}
