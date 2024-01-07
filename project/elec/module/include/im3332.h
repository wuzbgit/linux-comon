#ifndef _IM3332_H_
#define _IM3332_H_

#ifdef __cplusplus
extern "C" {
#endif

enum im333x_registers_32bits
{
    IM333X_REG32_AE_T  = 0x0000,
    IM333X_REG32_PAE_T = 0x0002,
    IM333X_REG32_NAE_T = 0x0004,

    IM333X_REG32_AP_T = 0x2000,
    IM333X_REG32_RP_T = 0x2002,
    // IM333X_REG32_APP_T = 0x2004,

    IM333X_REG32_CUR_A = 0x2008,
    IM333X_REG32_AP_A  = 0x200A,
    IM333X_REG32_RP_A  = 0x200C,
    // IM333X_REG32_APP_A = 0x200E,

    IM333X_REG32_CUR_B = 0x2015,
    IM333X_REG32_AP_B  = 0x2017,
    IM333X_REG32_RP_B  = 0x2019,
    // IM333X_REG32_APP_B = 0x201B,

    IM333X_REG32_CUR_C = 0x2022,
    IM333X_REG32_AP_C  = 0x2024,
    IM333X_REG32_RP_C  = 0x2026,
    // IM333X_REG32_APP_C = 0x2028,
};

enum im333x_registers_16bits
{
    IM333X_REG_PF_T   = 0x2006,
    IM333X_REG_VOL_A  = 0x2007,
    IM333X_REG_PF_A   = 0x2010,
    IM333X_REG_VOL_B  = 0x2014,
    IM333X_REG_PF_B   = 0x201D,
    IM333X_REG_VOL_C  = 0x2021,
    IM333X_REG_PF_C   = 0x202A,
    IM333X_REG_TEMP   = 0x202F,
    IM333X_REG_VOL1_A = 0x1201,
    IM333X_REG_VOL1_B = 0x1231,
    IM333X_REG_VOL1_C = 0x1261,
    IM333X_REG_CUR1_A = 0x1291,
    IM333X_REG_CUR1_B = 0x12C1,
    IM333X_REG_CUR1_C = 0x12F1,

    IM333X_REG_ADDR     = 0x4000,
    IM333X_REG_VT_RATIO = 0x4001,
    IM333X_REG_CT_RATIO = 0x4002,
    IM333X_REG_BAUD     = 0x4003,
    IM333X_REG_PARITY   = 0x4006,
    IM333X_REG_MB_VER   = 0x4009,
    IM333X_REG_WIRING   = 0x400A, /* 接线方式 */
    IM333X_REG_SOFT_VER = 0x4012,
    IM333X_REG_MODEL    = 0x4013,

    IM333X_REG_STATUS   = 0x401C,
    IM333X_REG_VOL_RANGE= 0x401D,
    IM333X_REG_EVENT_LOSSVOL_VOLTRIGGER_UP_LIMIT = 0x4041,
    IM333X_REG_EVENT_LOSSVOL_VOLRECOVER_DOWN_LIMIT = 0x4042,
    IM333X_REG_EVENT_LOSSVOL_CURTRIGGER_DOWN_LIMIT1 = 0x4043,
    IM333X_REG_EVENT_LOSSVOL_CURTRIGGER_DOWN_LIMIT2 = 0x4044,
    IM333X_REG_EVENT_LOSSVOL_DELAY_TIME = 0x4045,
    IM333X_REG_EVENT_UNDERVOL_VOLTRIGGER_UP_LIMIT = 0x4046,
    IM333X_REG_EVENT_UNDERVOL_DELAY_TIME  = 0x4047,
    IM333X_REG_EVENT_OVERVOL_VOLTRIGGER_DOWN_LIMIT = 0x4048,
    IM333X_REG_EVENT_OVERVOL_DELAY_TIME = 0x4049,
    IM333X_REG_EVENT_OPENPHASE_VOLTRIGGER_UP_LIMIT = 0x404A,
    IM333X_REG_EVENT_OPENPHASE_CURTRIGGER_UP_LIMIT1 = 0x404B,
    IM333X_REG_EVENT_OPENPHASE_CURTRIGGER_UP_LIMIT2 = 0x404C,
    IM333X_REG_EVENT_OPENPHASE_DELAY_TIME = 0x404D,
    IM333X_REG_EVENT_VOLUNBALANCE_LIMIT = 0x404E,
    IM333X_REG_EVENT_VOLUNBALANCE_DELAY_TIME = 0x404F,
    IM333X_REG_EVENT_CURUNBALANCE_LIMIT = 0x4050,
    IM333X_REG_EVENT_CURUNBALANCE_DELAY_TIME = 0x4051,   
    IM333X_REG_EVENT_LOSSCUR_VOLTRIGGER_DOWN_LIMIT = 0x4052,  
    IM333X_REG_EVENT_LOSSCUR_CURTRIGGER_UP_LIMIT1 = 0x4053,
    IM333X_REG_EVENT_LOSSCUR_CURTRIGGER_UP_LIMIT2 = 0x4054,    
    IM333X_REG_EVENT_LOSSCUR_CURTRIGGER_DOWN_LIMIT1 = 0x4055,  
    IM333X_REG_EVENT_LOSSCUR_CURTRIGGER_DOWN_LIMIT2 = 0x4056, 
    IM333X_REG_EVENT_LOSSCUR_DELAY_TIME = 0x4057,  
    IM333X_REG_EVENT_OVERCUR_CURTRIGGER_DOWN_LIMIT = 0x4058,  
    IM333X_REG_EVENT_OVERCUR_DELAY_TIME = 0x4059,    
    IM333X_REG_EVENT_OPENCUR_VOLTRIGGER_DOWN_LIMIT = 0x405A,  
    IM333X_REG_EVENT_OPENCUR_CURTRIGGER_UP_LIMIT1 = 0x405B, 
    IM333X_REG_EVENT_OPENCUR_CURTRIGGER_UP_LIMIT2 = 0x405C,
    IM333X_REG_EVENT_OPENCUR_DELAY_TIME = 0x405D,     
    IM333X_REG_EVENT_OVERLOAD_ACTIVEPOWERTRIGGER_DOWN_LIMIT1 = 0x405E,
    IM333X_REG_EVENT_OVERLOAD_ACTIVEPOWERTRIGGER_DOWN_LIMIT2 = 0x405F, 
    IM333X_REG_EVENT_OVERLOAD_DELAY_TIME = 0x4060,      
    IM333X_REG_EVENT_TOTALPOWER_FACTOR_DOWN_LIMIT = 0x4061, 
    IM333X_REG_EVENT_TOTALPOWER_FACTOR_DELAY_TIME = 0x4062,     

    IM333X_REG_CLEAR_ALL   = 0x6000,   
    IM333X_REG_CLEAR_EVENT = 0x6001, 
    IM333X_REG_INIT_PARAM = 0x6002,
    IM333X_REG_CLEAR_ELEC_ENERGY = 0x6003,
    IM333X_REG_CLEAR_INIT_PARAM = 0x6004,
    IM333X_REG_FACTORY  = 0x6005,

    IM333X_REG_SAMPLE_ENABLE  = 0x7000,
    IM333X_REG_SAMPLE_POINTS_NUM  = 0x7001,
    IM333X_REG_SAMPLE_PHASE_TYPE  = 0x7002,
    IM333X_REG_SAMPLE_ADC_RES  = 0x7003,
    IM333X_REG_SAMPLE_SCALE_RATIO  = 0x7004,

    IM333X_REG_SAMPLE_VOL_A_RATIO_LOW  = 0x7005,
    IM333X_REG_SAMPLE_VOL_A_RATIO_HIGH  = 0x7006,
    IM333X_REG_SAMPLE_CUR_A_RATIO_LOW  = 0x7007,
    IM333X_REG_SAMPLE_CUR_A_RATIO_HIGH  = 0x7008,
    IM333X_REG_SAMPLE_PHASE_A_RATIO_HIGH  = 0x7009,
    IM333X_REG_SAMPLE_PHASE_A_RATIO_LOW  = 0x700A,

    IM333X_REG_SAMPLE_VOL_B_RATIO_LOW  = 0x700B,
    IM333X_REG_SAMPLE_VOL_B_RATIO_HIGH  = 0x700C,
    IM333X_REG_SAMPLE_CUR_B_RATIO_LOW  = 0x700D,
    IM333X_REG_SAMPLE_CUR_B_RATIO_HIGH  = 0x700E,
    IM333X_REG_SAMPLE_PHASE_B_RATIO_HIGH  = 0x700F,
    IM333X_REG_SAMPLE_PHASE_B_RATIO_LOW  = 0x7010,

    IM333X_REG_SAMPLE_VOL_C_RATIO_LOW  = 0x7011,
    IM333X_REG_SAMPLE_VOL_C_RATIO_HIGH  = 0x7012,
    IM333X_REG_SAMPLE_CUR_C_RATIO_LOW  = 0x7013,
    IM333X_REG_SAMPLE_CUR_C_RATIO_HIGH  = 0x7014,
    IM333X_REG_SAMPLE_PHASE_C_RATIO_HIGH  = 0x7015,
    IM333X_REG_SAMPLE_PHASE_C_RATIO_LOW  = 0x7016,
};

typedef enum {
	MODE_THREEPHASE_FOURWIRE = 0,
	MODE_THREEPHASE_THREEWIRE = 1,
}IM3331_MODE;

typedef enum {
    TYPE_SINGAL_PHASE = 0x1,
    TYPE_THREE_PHASE = 0x3
}RAWDATA_PHASE_Type;

typedef enum {
    ADC_8BITS = 0x1,
    ADC_16BITS = 0x2,
    ADC_24BITS = 0X3
}RAWDATA_ADC_RESOLUTION;

typedef enum 
{
    PHASE_A = 0,
    PHASE_B = 1,
    PHASE_C = 2,
}IM333x_PHASE_E;

typedef struct 
{
    uint32_t voltage_factor;
    uint32_t current_factor;
    uint32_t phase_factor;
}IM333x_RawData_PhaseConf;

typedef enum {
    SAMPLE_POINTS_32 = 32,
    SAMPLE_POINTS_64 = 64,
    SAMPLE_POINTS_128 = 128,
    SAMPLE_POINTS_256 = 256
}IM333x_RawData_SamplePoints;

typedef struct 
{
    /* data */
    uint16_t VolLoss_VolTriggerUpLimit;
    uint16_t VolLoss_VolRecoverDownLimit;
    uint32_t VolLoss_CurTriggerDownimit;
    uint16_t VolUnder_VolTriggerUpLimit;
    uint16_t VolOver_VolTriggerDownLimit;
    uint16_t CurLoss_VolTriggerDownLimit;
    uint32_t CurLoss_CurTriggerUpLimit;
    uint32_t CurLoss_CurTriggerDownLimit;
    uint16_t CurOver_CurTriggerDownLimit;   
}SigalPhaseModuleAlarmEvent_t;

typedef struct 
{
    /* data */
    uint16_t TotalPowerFactorDownimit;
    uint16_t CurrentUnbalanceLimit;
    uint16_t VoltageUnbalanceLimit;  
}ConPhaseModuleAlarmEvent_t;

int im3332_write_regs(uint16_t reg,uint16_t *buf,unsigned len);
int im3332_read_regs(uint16_t reg,uint16_t *buf,unsigned len);
int im333x_read_regs32(uint16_t reg, uint32_t *val, uint32_t len);
int im3332_get_conphase_alarmevent(ConPhaseModuleAlarmEvent_t *con);
int im3332_set_conphase_alarmevent(ConPhaseModuleAlarmEvent_t *con);
int im3332_get_sigalphase_alarmevent(SigalPhaseModuleAlarmEvent_t *sigal);
int im3332_set_sigalphase_alarmevent(SigalPhaseModuleAlarmEvent_t *sigal);
int im3332_get_model(void *handle,uint16_t *model);
int im3332_get_sw_version(void *handle,uint16_t *version);
int im3332_enter_factory(void *handle);
int im3332_exit_factory(void *handle);
int im3331_set_mode(void *handle,IM3331_MODE mode);
int im3331_get_mode(void *handle,IM3331_MODE *mode);
int im3332_get_negative_act_energy(uint32_t *val);
int im3332_get_positive_act_energy(uint32_t *val);
int im3332_get_con_act_energy(uint32_t *val);
int im333x_get_error();
void* im3332_init();
void im3332_deinit(void *hanlde);
int im3332_sample_type_get(void *handle,RAWDATA_PHASE_Type *phase_type);
int im3332_sample_points_get(void *handle,IM333x_RawData_SamplePoints *num);
int im3332_sample_adcres_get(void *handle,RAWDATA_ADC_RESOLUTION  *res);
int im3332_sample_scale_get(void *handle,uint16_t *scale_expend);
int im3332_sample_state(void *handle,uint16_t *enable);
int im3332_sample_points_conf(void *handle,IM333x_RawData_SamplePoints num);
int im3332_sample_read(char *buf,unsigned bufsize);
int im3332_sample_enable(void *handle);
int im3332_sample_disable(void *handle);
int im3332_sample_init(void *handle);
void im3332_sample_deinit(void *handle);
#ifdef __cplusplus
}
#endif

#endif