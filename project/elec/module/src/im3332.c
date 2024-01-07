#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <modbus/modbus.h>
#include <pthread.h>
#include "im3332.h"

#define IO_CTL_ENABLE			0x2020
#define IO_CTL_DISABLE			0x2022
#define IO_CTRL_BUF_NUM         0x2024
#define IO_CTL_DQBUF            0x2026
#define IO_CTL_QBUF             0x2028

#define SLAVE_ID    0x01
#define PORT_BAUDRATE 9600
#define IM3332_DEV_PORT     "/dev/ttyS1"

static modbus_t *ctx = NULL;
static pthread_mutex_t im3332_mutex = PTHREAD_MUTEX_INITIALIZER;
static int im3332_write_reg(void *handle,uint16_t reg,uint16_t value)
{
	int rc;
	if (ctx == NULL) {
		return 0;
	}	
	pthread_mutex_lock(&im3332_mutex);
    rc = modbus_write_register((modbus_t*)handle, reg, value);
	pthread_mutex_unlock(&im3332_mutex);
	return rc;
}

static int im3332_read_reg(void *handle,uint16_t reg,uint16_t *value)
{
	int rc;
	if (ctx == NULL) {
		return 0;
	}	
	pthread_mutex_lock(&im3332_mutex);
	rc = modbus_read_registers((modbus_t*)handle, reg, 1, value);
	pthread_mutex_unlock(&im3332_mutex);
	return rc;
}

int im3332_write_regs(uint16_t reg,uint16_t *buf,uint32_t len)
{
	int rc;
	if (ctx == NULL) {
		return 0;
	}	
	pthread_mutex_lock(&im3332_mutex);
	rc = modbus_write_registers(ctx, reg, len, buf);
	pthread_mutex_lock(&im3332_mutex);
	return rc;
}

int im3332_read_regs(uint16_t reg,uint16_t *buf,uint32_t len)
{
	int rc;
	if (ctx == NULL) {
		return 0;
	}	
	pthread_mutex_lock(&im3332_mutex);
	rc = modbus_read_registers(ctx, reg, len, buf);
	pthread_mutex_unlock(&im3332_mutex);
	return rc;
}

int im3332_read_reg32(void *handle,uint16_t reg,uint32_t *val)
{
	int rc;
	if (ctx == NULL) {
		return 0;
	}	
	pthread_mutex_lock(&im3332_mutex);
	rc = modbus_read_registers((modbus_t*)handle, reg, 2, (uint16_t*)val);
	pthread_mutex_unlock(&im3332_mutex);
	return rc;
}

int im3332_write_reg32(void *handle,uint16_t reg,uint32_t val)
{
	int rc;
	if (ctx == NULL) {
		return 0;
	}
	pthread_mutex_lock(&im3332_mutex);
	rc = modbus_write_registers((modbus_t*)handle, reg, 2, (uint16_t*)&val);
	pthread_mutex_unlock(&im3332_mutex);
	return rc;
}

int im333x_read_regs32(uint16_t reg, uint32_t *val, uint32_t len)
{
    int rc = 0;
	if (ctx == NULL) {
		return 0;
	}

	pthread_mutex_lock(&im3332_mutex);
	rc = modbus_read_registers(ctx, reg, len * 2, (uint16_t *)val);
	pthread_mutex_unlock(&im3332_mutex);
    return rc;
}

int im3332_get_model(void *handle,uint16_t *model)
{
    int rc;

    rc = im3332_read_reg(ctx,IM333X_REG_MODEL,model);

    return (rc > 0) ? 0 : -1;
}

int im3332_get_sw_version(void *handle,uint16_t *version)
{
    int rc;
    
    rc = im3332_read_reg(ctx, IM333X_REG_SOFT_VER,version); 

    return (rc > 0) ? 0 : -1;    
}

int im3332_enter_factory(void *handle)
{
    int rc;

    rc = modbus_write_register(ctx, IM333X_REG_FACTORY, 0xAAAA);

    return (rc > 0) ? 0 : -1;   
}

int im3332_exit_factory(void *handle)
{
    int rc;

    rc = modbus_write_register(ctx, IM333X_REG_FACTORY, 0x0);

    return (rc > 0) ? 0 : -1;       
}

int im3331_set_mode(void *handle,IM3331_MODE mode)
{
	int ret;
	uint8_t cmd = 0xAA;
	
	im3332_enter_factory((modbus_t*)handle);
	
	if(mode == MODE_THREEPHASE_FOURWIRE) {
		cmd = 0x55;
	}
	else if(mode == MODE_THREEPHASE_THREEWIRE) {
		cmd = 0xAA;
	}
	
	ret = im3332_write_reg((modbus_t*)handle,IM333X_REG_WIRING, cmd);
	if(ret < 0) {
		printf("can not set im3331 mode\r\n");
	}
	
	im3332_exit_factory(handle);
	return (ret > 0) ? 0 : -1;
}

int im3331_get_mode(void *handle,IM3331_MODE *mode)
{
	int rc = 2;
    uint16_t value = 0;

	if ((handle == NULL) || (mode == NULL)) {
		return -1;
	}

	rc = im3332_read_reg(handle, IM333X_REG_WIRING, &value);
    if (rc > 0) {
        *mode = (value == 0x55) ? MODE_THREEPHASE_FOURWIRE : MODE_THREEPHASE_THREEWIRE;
    }
	return (rc > 0) ? 0 : -1;    
}

int im3332_get_sigalphase_alarmevent(SigalPhaseModuleAlarmEvent_t *sigal)
{
	int rc = 0;

	if (ctx == NULL) {
		return -1;
	}

    rc = im3332_read_reg(ctx,IM333X_REG_EVENT_LOSSVOL_VOLTRIGGER_UP_LIMIT, &sigal->VolLoss_VolTriggerUpLimit);
    if (rc <= 0) {
		return -1;
	}

	rc = im3332_read_reg32(ctx,IM333X_REG_EVENT_LOSSVOL_CURTRIGGER_DOWN_LIMIT1, &sigal->VolLoss_CurTriggerDownimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_read_reg(ctx,IM333X_REG_EVENT_UNDERVOL_VOLTRIGGER_UP_LIMIT, &sigal->VolUnder_VolTriggerUpLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_read_reg(ctx,IM333X_REG_EVENT_OVERVOL_VOLTRIGGER_DOWN_LIMIT, &sigal->VolOver_VolTriggerDownLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_read_reg(ctx,IM333X_REG_EVENT_LOSSCUR_VOLTRIGGER_DOWN_LIMIT, &sigal->CurLoss_VolTriggerDownLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_read_reg32(ctx,IM333X_REG_EVENT_LOSSCUR_CURTRIGGER_UP_LIMIT1, &sigal->CurLoss_CurTriggerUpLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_read_reg(ctx,IM333X_REG_EVENT_OVERCUR_CURTRIGGER_DOWN_LIMIT, &sigal->CurOver_CurTriggerDownLimit);
	if (rc <= 0) {
		return -1;
	}

	return 0;
}

int im3332_get_conphase_alarmevent(ConPhaseModuleAlarmEvent_t *con)
{
	int rc = 0;

	if (ctx == NULL) {
		return -1;
	}
	rc = im3332_read_reg(ctx,IM333X_REG_EVENT_TOTALPOWER_FACTOR_DOWN_LIMIT, &con->TotalPowerFactorDownimit);
	if (rc <= 0) {
		return -1;
	}
    rc = im3332_read_reg(ctx,IM333X_REG_EVENT_CURUNBALANCE_LIMIT, &con->CurrentUnbalanceLimit);
	if (rc <= 0) {
		return -1;
	}
    rc = im3332_read_reg(ctx,IM333X_REG_EVENT_VOLUNBALANCE_LIMIT, &con->VoltageUnbalanceLimit);
	if (rc <= 0) {
		return -1;
	}	

	return 0;
}

int im3332_set_sigalphase_alarmevent(SigalPhaseModuleAlarmEvent_t *sigal)
{
	int rc = 0;

	if (ctx == NULL) {
		return -1;
	}

    rc = im3332_write_reg(ctx,IM333X_REG_EVENT_LOSSVOL_VOLTRIGGER_UP_LIMIT, sigal->VolLoss_VolTriggerUpLimit);
    if (rc <= 0) {
		return -1;
	}

	rc = im3332_write_reg32(ctx,IM333X_REG_EVENT_LOSSVOL_CURTRIGGER_DOWN_LIMIT1, sigal->VolLoss_CurTriggerDownimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_write_reg(ctx,IM333X_REG_EVENT_UNDERVOL_VOLTRIGGER_UP_LIMIT, sigal->VolUnder_VolTriggerUpLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_write_reg(ctx,IM333X_REG_EVENT_OVERVOL_VOLTRIGGER_DOWN_LIMIT, sigal->VolOver_VolTriggerDownLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_write_reg(ctx,IM333X_REG_EVENT_LOSSCUR_VOLTRIGGER_DOWN_LIMIT, sigal->CurLoss_VolTriggerDownLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_write_reg32(ctx,IM333X_REG_EVENT_LOSSCUR_CURTRIGGER_UP_LIMIT1, sigal->CurLoss_CurTriggerUpLimit);
    if (rc <= 0) {
		return -1;
	}
	rc = im3332_write_reg(ctx,IM333X_REG_EVENT_OVERCUR_CURTRIGGER_DOWN_LIMIT, sigal->CurOver_CurTriggerDownLimit);
	if (rc <= 0) {
		return -1;
	}

	return 0;
}

int im3332_set_conphase_alarmevent(ConPhaseModuleAlarmEvent_t *con)
{
	int rc = 0;

	if (ctx == NULL) {
		return -1;
	}
	rc = im3332_write_reg(ctx,IM333X_REG_EVENT_TOTALPOWER_FACTOR_DOWN_LIMIT, con->TotalPowerFactorDownimit);
	if (rc <= 0) {
		return -1;
	}
    rc = im3332_write_reg(ctx,IM333X_REG_EVENT_CURUNBALANCE_LIMIT, con->CurrentUnbalanceLimit);
	if (rc <= 0) {
		return -1;
	}
    rc = im3332_write_reg(ctx,IM333X_REG_EVENT_VOLUNBALANCE_LIMIT, con->VoltageUnbalanceLimit);
	if (rc <= 0) {
		return -1;
	}	

	return 0;
}

int im3332_get_con_act_energy(uint32_t *val)
{
	int rc = 0;
	rc = im3332_read_reg32(ctx,IM333X_REG32_AE_T, val);
    if (rc <= 0) {
		return -1;
	}	

	return 0;
}

int im3332_get_positive_act_energy(uint32_t *val)
{
	int rc = 0;
	rc = im3332_read_reg32(ctx,IM333X_REG32_PAE_T, val);
    if (rc <= 0) {
		return -1;
	}	

	return 0;
}

int im3332_get_negative_act_energy(uint32_t *val)
{
	int rc = 0;
	rc = im3332_read_reg32(ctx,IM333X_REG32_NAE_T, val);
    if (rc <= 0) {
		return -1;
	}	

	return 0;
} 

int im333x_get_error()
{
	return 0;
}

void* im3332_init()
{
    uint32_t new_response_to_sec;
    uint32_t new_response_to_usec;
    uint32_t old_response_to_sec;
    uint32_t old_response_to_usec;
	
	if (ctx != NULL) {
		return ctx;
	}

	ctx = modbus_new_rtu(IM3332_DEV_PORT, PORT_BAUDRATE, 'N', 8, 1);
	if (ctx == NULL) {
		fprintf(stderr, "new modbus rtu failed: %s\n", modbus_strerror(errno));

		return NULL;
	}

    modbus_set_debug(ctx, FALSE);
    modbus_set_error_recovery(ctx,
                              (modbus_error_recovery_mode)(MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL));

    modbus_set_slave(ctx, SLAVE_ID);

    modbus_get_response_timeout(ctx, &old_response_to_sec, &old_response_to_usec);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return NULL;
    }

    modbus_get_response_timeout(ctx, &new_response_to_sec, &new_response_to_usec);

	im3332_sample_disable(ctx);

	return ctx;
}

void im3332_deinit(void *hanlde)
{
	// im3332_sample_disable(ctx);
}

#include <sys/ioctl.h>

static int im3332_fd = -1;

#define IM3332_DEV_PATH			"/dev/spi1_im3332"

int im3332_sample_type_get(void *handle,RAWDATA_PHASE_Type *phase_type)
{
    int ret;

	ret = im3332_read_reg((modbus_t*)handle,IM333X_REG_SAMPLE_PHASE_TYPE, (uint16_t *)phase_type);
	if(ret < 0) {
		printf("can not get im3332 sample phase type\r\n");
	}  

    return (ret > 0) ? 0 : 1;  	
}

int im3332_sample_points_get(void *handle,IM333x_RawData_SamplePoints *num)
{
    int ret;

	ret = im3332_read_reg((modbus_t*)handle,IM333X_REG_SAMPLE_POINTS_NUM, (uint16_t *)num);
	if(ret < 0) {
		printf("can not get im3332 sample points number\r\n");
	}  

    return (ret > 0) ? 0 : 1;  	   
}

int im3332_sample_points_conf(void *handle,IM333x_RawData_SamplePoints num)
{
    int ret;

	ret = im3332_write_reg((modbus_t*)handle,IM333X_REG_SAMPLE_POINTS_NUM, num);
	if(ret < 0) {
		printf("can not set im3332 sample points number\r\n");
	}  

    return (ret > 0) ? 0 : 1;  	   
}

int im3332_sample_adcres_get(void *handle,RAWDATA_ADC_RESOLUTION  *res)
{
    int ret;

	ret = im3332_read_reg((modbus_t*)handle,IM333X_REG_SAMPLE_ADC_RES, (uint16_t *)res);
	if(ret < 0) {
		printf("can not set im3332 sample adc resolution\r\n");
	}  

    return (ret > 0) ? 0 : 1;  	  
}

int im3332_sample_scale_get(void *handle,uint16_t *scale_expend)
{
    int ret;

	ret = im3332_read_reg((modbus_t*)handle,IM333X_REG_SAMPLE_SCALE_RATIO, scale_expend);
	if(ret < 0) {
		printf("can not get im3332 sample adc scale\r\n");
	}  

	return (ret > 0) ? 0 : 1;  	
}

int im3332_sample_state(void *handle,uint16_t *enable)
{
    int ret;

	ret = im3332_read_reg((modbus_t*)handle,IM333X_REG_SAMPLE_ENABLE, enable);
	if(ret < 0) {
		printf("can not get im3332 sample enable state\r\n");
	}  

    return (ret > 0) ? 0 : 1; 	
}

int im3332_sample_enable(void *handle)
{
    int ret;

	ret = im3332_write_reg((modbus_t*)handle,IM333X_REG_SAMPLE_ENABLE, 0xAA);
	if(ret < 0) {
		printf("can not enable im3332 sample\r\n");
	}  

    return (ret > 0) ? 0 : 1;  	     
}

int im3332_sample_disable(void *handle)
{
    int ret;

	ret = im3332_write_reg((modbus_t*)handle,IM333X_REG_SAMPLE_ENABLE, 0x55);
	if(ret < 0) {
		printf("can not disable im3332 sample\r\n");
	}  

    return (ret > 0) ? 0 : 1;  	 
}

int im3332_sample_init(void *handle)
{
	uint32_t arg = 0;
	int ret;

	im3332_sample_disable(handle);
	
    im3332_fd = open(IM3332_DEV_PATH,O_RDWR);
    if (im3332_fd < 0) {
        printf("can not open dev:%s\r\n",IM3332_DEV_PATH);

        return -1;
    }

	ret = ioctl(im3332_fd,IO_CTL_ENABLE,arg);
	if (ret != 0) {
		printf("can not enable spi im3332\n");

		close(im3332_fd);
		return -1;
	}

	return 0;
}

void im3332_sample_deinit(void *handle)
{
	uint32_t arg = 0;

	im3332_sample_disable(handle);

	if (im3332_fd > 0) {
		ioctl(im3332_fd,IO_CTL_ENABLE,arg);

		close(im3332_fd);
		im3332_fd = 0;
	}
}

int im3332_sample_read(char *buf,unsigned bufsize)
{
	int len;

	if (im3332_fd <= 0) {
		printf("intialze im3332_sample mode first\n");

		return 0;
	}

	len = read(im3332_fd, buf, bufsize);
    if (len <= 0) {
        return 0;
    }

	return len;
}