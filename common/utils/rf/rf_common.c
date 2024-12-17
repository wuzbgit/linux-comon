#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <termios.h>
#include "rf_lynq.h"
#include "rf_quectel.h"
#include "rf_common.h"
#include "gpio/gpio.h"
#include "debug/debug.h"

static pthread_mutex_t g_rf_mutex = PTHREAD_MUTEX_INITIALIZER;

static int init_rf_gpio(int gpio)
{
	int ret = -1;
	char gpio_reset[64] = {0};
    struct stat sbuf;
	snprintf(gpio_reset,"/sys/class/gpio/gpio%d",gpio);
    if(0 == stat(gpio_reset, &sbuf))
		return ret = 0;
    
    ret = gpio_export(gpio);
    if(0 != ret) {
        LOG_ERROR("rf_main: export gpio RF_RESET failed");
    }
    else
    {
        ret = gpio_set_direction(gpio, GPIO_DIR_OUT);
        if(0 != ret)
		{
            LOG_ERROR("rf_main: direction gpio RF_RESET failed");
		}
    }
    return ret;
}

int poweron_rf(int gpio, int level)
{
#ifdef PROJECT_ELEC
	if (system("gpio -s PE1 1 1 0 0 > /dev/null")) {
		return -1;
	}
#endif

#ifdef PROJECT_IOT_GATEWAY
	if (system("gpio -s PG13 1 1 0 0 > /dev/null")) {
		return -1;
	}
#endif

#ifdef PROJECT_ISCANNER
	unsigned gpio_on = level;
	
    if( 0 == init_rf_gpio(gpio)) {
   		if(0 != gpio_set_level(gpio, gpio_on)) {
	 		LOG_ERROR("rf_main: failed to set gpio RF_RESET high\n");
	 		return -1;
    	}
	}
#endif

	return 0;
}

int poweroff_rf(int gpio, int level)
{
#ifdef PROJECT_IOT_GATEWAY
	if (system("gpio -s PG13 1 0 0 0 > /dev/null")) {
		return -1;
	}
#endif

#ifdef PROJECT_ELEC
	if (system("gpio -s PG13 1 0 0 0 > /dev/null")) {
		return -1;
	}
#endif

#ifdef PROJECT_ISCANNER

	unsigned gpio_off = level;
	
    if( 0 == init_rf_gpio())
    {
   		if(0 != gpio_set_level(gpio, gpio_off))
    	{
	 		LOG_ERROR("rf_main: failed to set gpio RF_RESET high\n");
	 		return -1;
    	}
	}
#endif

	return 0;
}

static int reset_rf(int gpio,int on_level)
{
	poweroff_rf(gpio, !on_level);

	sleep(2);

	poweron_rf(gpio, on_level);

    return 0;
}

int get_RF_moduleInfo(const char* rfdev_, RFINFO_T* pstrfinfo_, int rf_mnf_)
{
    int ret  = -1;

    if((NULL == pstrfinfo_) || (NULL == rfdev_))
		return ret;

    if(RF_LYNQ == rf_mnf_)
    {
		if(get_RF_DeviceInfo_LYNQ(rfdev_, pstrfinfo_))
	    	return ret;
    }
    else if(RF_QUECTEL == rf_mnf_)
    {
		ret= get_RF_DeviceInfo_QUECTEL(rfdev_, pstrfinfo_);
    }
	else
	{
		LOG_INFO("unkown rf module rf_mnf_:%d\n",rf_mnf_);
	}

    return ret;
}

static void calculate_RF_signalQos(int rf_mnf_,RFINFO_T*  pstrfinfo_,RF_NWMODE mode)
{
    int rsrp = 0;
    if(NULL == pstrfinfo_)
		return;

	if(rf_mnf_ == RF_LYNQ)
	{
		rsrp = pstrfinfo_->RFsignal.RSRP / 10;
		
		if((0 != rsrp) && (rsrp > -75))
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_BEST;
		else if((rsrp <= -75) && (rsrp > -85)) 
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_GOOD;
		else if((rsrp <= -85) && (rsrp > -95))
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_NORMAL;    
		else if((rsrp <= -95) && (rsrp > -105))
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_BAD;	
		else
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_WORST;

	}
	else if(rf_mnf_ == RF_QUECTEL)
	{
		int sig = -9999;
		
		switch(mode)
		{
			case RF_NWMODE_NONE:
			case RF_NWMODE_2G:
			case RF_NWMODE_3G:
				sig = pstrfinfo_->RFsignal.RSSI;
				break;
			case RF_NWMODE_4G:
				sig = pstrfinfo_->RFsignal.RSRP;
				break;
		}
		
		if(sig > -85)
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_BEST;		//-85dBm or greater
		else if(sig > -95)
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_GOOD;		//-95dBm --- -85dBm
		else if(sig > -105)					
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_NORMAL;  	//-105dBm ---- -95dBm
		else if(sig > -115)
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_BAD;		//-115dBm ---  -105dBm
		else
			pstrfinfo_->RFsignal.SignalQos = RF_Qos_WORST;		//-115dBm or less
	}
	else
	{
		pstrfinfo_->RFsignal.SignalQos = RF_Qos_WORST;	
	}		
}

int update_RF_signalInfo(const char* rfdev_, RFINFO_T* pstrfinfo_, int rf_mnf_)
{
    int ret  = -1;
    RF_NWMODE mode = RF_NWMODE_4G;
	
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
		return ret;
	
    pthread_mutex_lock(&g_rf_mutex);
	
    if(RF_LYNQ == rf_mnf_) 
    {
		memset(&pstrfinfo_->RFsignal, 0x00, sizeof(pstrfinfo_->RFsignal));
		ret = get_RF_Signal_LYNQ(rfdev_, pstrfinfo_);
		calculate_RF_signalQos(rf_mnf_,pstrfinfo_,mode);
    }
	else if(RF_QUECTEL == rf_mnf_)
	{
		memset(&pstrfinfo_->RFsignal, 0x00, sizeof(pstrfinfo_->RFsignal));
		get_nwmode_QUECTEL(rfdev_, &mode);
		get_RF_Signal_QUECTEL(rfdev_, pstrfinfo_);
		calculate_RF_signalQos(rf_mnf_,pstrfinfo_,mode);
	}

	pthread_mutex_unlock(&g_rf_mutex);
    return ret;
}

int update_RF_status(const char* rfdev_, RFINFO_T* pstrfinfo_, int rf_mnf_)
{
    int ret  = -1;

    if(NULL == pstrfinfo_)
		return ret;
    LOG_INFO("%s,%d\n",__func__,__LINE__);
    ret = get_RF_moduleInfo(rfdev_, pstrfinfo_, rf_mnf_);
	LOG_INFO("%s,%d\n",__func__,__LINE__);
    ret |= update_RF_signalInfo(rfdev_, pstrfinfo_, rf_mnf_);
    LOG_INFO("%s,%d\n",__func__,__LINE__);
    return ret;
}

int init_RF_info(RFINFO_T *RF_info)
{
	char default_str[8] = "--";
	
	strncpy(RF_info->Manufacture,default_str,strlen(default_str) + 1);
	strncpy(RF_info->PLMN,default_str,strlen(default_str) + 1);
	strncpy(RF_info->Model,default_str,strlen(default_str) + 1);
	strncpy(RF_info->Rev,default_str,strlen(default_str) + 1);
	strncpy(RF_info->SN,default_str,strlen(default_str) + 1);
	strncpy(RF_info->IMEI,default_str,strlen(default_str) + 1);
	strncpy(RF_info->CICCID,default_str,strlen(default_str) + 1);
	strncpy(RF_info->SIMStatus,default_str,strlen(default_str) + 1);
	strncpy(RF_info->RFsignal.Freqband,default_str,strlen(default_str) + 1);
	strncpy(RF_info->RFsignal.Opmode,default_str,strlen(default_str) + 1);
	strncpy(RF_info->RFsignal.Sysmode,default_str,strlen(default_str) + 1);
	strncpy(RF_info->RFsignal.Cpin,default_str,strlen(default_str) + 1);
	RF_info->RFsignal.RSRQ = 9999;
	RF_info->RFsignal.RSRP = 9999;
	RF_info->RFsignal.RSSI = 9999;
	RF_info->RFsignal.RSSNR = 9999;
	RF_info->RFsignal.SINR = 9999;
	RF_info->RFsignal.SignalQos = 0;
	RF_info->rfnetConnectUtctime = 0;

	return 0;
}

int rf_get_cfun(char* rfdev_, const int rf_manufacture_,int *cfun)
{
	int ret = -1;
	pthread_mutex_lock(&g_rf_mutex);
    if(rf_manufacture_ == RF_LYNQ)
    {
		ret = get_cfun_LYNQ(rfdev_,cfun);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_cfun_LYNQ err\r\n");
		}
    }
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = get_cfun_QUECTEL(rfdev_,cfun);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_cfun_QUECTEL err\r\n");
		}
    }

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
		
}

int rf_sim_detect(char *rfdev_,const int rf_manufacture_, bool enable, int level)
{
	int ret = -1;
	pthread_mutex_lock(&g_rf_mutex);
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = SimDetect_enable_QUECTEL(rfdev_,enable, level);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: %s sim card detect err\r\n",enable==true ? "enable" : "disable");
		}
    }

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;	
}
int rf_get_cpin(char* rfdev_, const int rf_manufacture_,int *cpin)
{
	int ret = -1;
	pthread_mutex_lock(&g_rf_mutex);
    if(rf_manufacture_ == RF_LYNQ)
    {
		ret = get_cpin_LYNQ(rfdev_,cpin);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_cfun_LYNQ err\r\n");
		}
    }
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = get_cpin_QUECTEL(rfdev_,cpin);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_cpin_QUECTEL err\r\n");
		}
    }

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
		
}

int rf_nw_scanmode(char* rfdev_, const int rf_manufacture_)
{
	int ret = -1;
	pthread_mutex_lock(&g_rf_mutex);
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = nwscanmode_QUECTEL(rfdev_);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: rf_nw_scanmode err\r\n");
		}
    }

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
		
}

int rf_get_nwmode(char* rfdev_, const int rf_manufacture_,RF_NWMODE *nwmode)
{
	int ret = -1;
	pthread_mutex_lock(&g_rf_mutex);
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = get_nwmode_QUECTEL(rfdev_,nwmode);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_nwmode_QUECTEL err\r\n");
		}
    }

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
		
}

int rf_get_creg(char* rfdev_, const int rf_manufacture_,int *creg)
{
	int ret = -1;
	pthread_mutex_lock(&g_rf_mutex);
    if(rf_manufacture_ == RF_LYNQ)
    {
		ret = get_creg_LYNQ(rfdev_,creg);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_creg_LYNQ err\r\n");
		}
    }
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = get_creg_QUECTEL(rfdev_,creg);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: get_creg_QUECTEL err\r\n");
		}
    }

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
		
}

int rf_at_test(char* rfdev_, const int rf_manufacture_)
{
	char response[256] = {0};
	int ret = -1;
	
	pthread_mutex_lock(&g_rf_mutex); 
	if(0 != parse_RFinfo_request(rfdev_, "AT+GMI=?\r\n", response, sizeof(response)))
	{
		LOG_ERROR("ERROR: rf can not  use AT cmd\n");
		ret = -1;
	}	
	else
	{
		if(strstr(response, "OK"))
		{
			ret = 0;
		}
		else
		{
			LOG_ERROR("ERROR: rf can not  use AT cmd\n");
		}	
	}

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
}

int rf_at_cmd(char* rfdev_,char *cmd)
{
	char response[256] = {0};
	int ret = -1;
	char at_cmd[70] = {0};

	if(strlen(cmd) > 64)
		return -1;

	sprintf(at_cmd,"%s\r\n",cmd);
	
	pthread_mutex_lock(&g_rf_mutex); 
	
	if(0 != parse_RFinfo_request(rfdev_, at_cmd, response, sizeof(response) - 1)) {
		printf("ERROR: rf can not  use AT cmd:%s\n",cmd);
		ret = -1;
	}	
	else {
		printf("cmd:%s response:%s\n",cmd,response);
		ret = 0;
	}

	pthread_mutex_unlock(&g_rf_mutex);

	return ret;
}

int find_RF_manufacture(const char* rfdev_)
{
    char response[256] = {0};
    char aticmd[] = "ATI\r\n";
    int ret = RF_NONE;
    int i = 0;
    int maxcnt = 1;
    
    if((NULL == rfdev_) || (strlen(rfdev_) == 0))
		return ret;
	
	pthread_mutex_lock(&g_rf_mutex); 
    
    for(i = 0; i < maxcnt; i++)
    {
		// sleep(1);
		if(0 != parse_RFinfo_request(rfdev_, aticmd, response, sizeof(response)))
		{
	    	LOG_ERROR("ERROR: read %s failed\n", aticmd);
	    	continue;
		}

		if(strstr(response, "Lynq")) {
			ret = RF_LYNQ;
		} 	
		else if(strstr(response, "Quectel")) {
			ret = RF_QUECTEL;
		}
	    	
		if(ret != RF_NONE)
	    	break;
    }

    if(ret == RF_NONE)
		LOG_ERROR("ERROR: Invalid RF Manufacture\n");
	
    pthread_mutex_unlock(&g_rf_mutex);
    return ret;
}

static int get_dev_maxnum()
{
    DIR *pDir;
    struct dirent* ent = NULL;
    int maxnum = 0;
    char dev_prefix[] = "ttyUSB";
    int devnum;
    
    if ((pDir = opendir("/dev")) == NULL)  
    {
        LOG_ERROR("Cannot open directory: /dev\n");
        return maxnum;
    }
    
    while ((ent = readdir(pDir)) != NULL) 
    {
        if (strncmp(ent->d_name,dev_prefix , strlen(dev_prefix)) == 0) 
	{
	    if(1 == sscanf(ent->d_name, "ttyUSB%d", &devnum))
		if(devnum > maxnum)
		    maxnum = devnum;
	}
    }
    
    closedir(pDir);
    
    return maxnum;
}

int access_dev_netpath(int num_)
{
    int res = -1;
    char net_path[64] = {0};
    
    if(num_ <= 0)
	return res;
    
    snprintf(net_path, sizeof(net_path) - 1, "/sys/class/tty/ttyUSB%d", num_);

    if(0 == access(net_path, R_OK))
	res  = 0;
    else
	LOG_ERROR("ERROR: failed to access %s\n",net_path);

    return res;
}
/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int RF_device_detect(char * dev_, int size_)
{
    int res = -1;
    int maxnum = 0;

    if(NULL == dev_)
		return res;
	
    pthread_mutex_lock(&g_rf_mutex);
	
    maxnum = get_dev_maxnum();
    
    if(maxnum > 0)
    {
		if(0 == access_dev_netpath(maxnum))
		{
		   snprintf(dev_, size_, "/dev/ttyUSB%d", maxnum); 
		   res	= 0;
		}
		else
		{
			if(0 == access_dev_netpath(maxnum - 1))
			{
				snprintf(dev_, size_, "/dev/ttyUSB%d", maxnum -1); 
				res  = 0;
			}
		}
    }
	
	pthread_mutex_unlock(&g_rf_mutex);
    return res;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int rf_soft_reset(char* rfdev_, const int rf_manufacture_)
{
	int ret = -1;

	pthread_mutex_lock(&g_rf_mutex);
	
    if(rf_manufacture_ == RF_LYNQ)
    {
		ret = soft_reset_LYNQ(rfdev_);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: soft_reset_LYNQ err\r\n");

			goto soft_reset_error;
		}
    }
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = soft_reset_QUECTEL(rfdev_);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: soft_reset_QUECTEL err\r\n");

			goto soft_reset_error;
		}
    }
	
	pthread_mutex_unlock(&g_rf_mutex);
    return ret;
soft_reset_error:
		pthread_mutex_unlock(&g_rf_mutex);
		LOG_ERROR("ERROR: rf_soft_reset err\r\n");
		return -1;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int rf_cfun_reset(char* rfdev_, const int rf_manufacture_)
{
	int ret = -1;

	pthread_mutex_lock(&g_rf_mutex);
	
    if(rf_manufacture_ == RF_LYNQ)
    {
		ret = reset_cfun_LYNQ(rfdev_);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: reset_cfun_LYNQ err\r\n");

			goto cfun_reset_error;
		}
    }
    if(rf_manufacture_ == RF_QUECTEL)
    {
		ret = reset_cfun_QUECTEL(rfdev_);
		if(ret!=0)
		{
			LOG_ERROR("ERROR: reset_cfun_QUECTEL err\r\n");

			goto cfun_reset_error;
		}
    }
	
	pthread_mutex_unlock(&g_rf_mutex);
    return ret;
cfun_reset_error:
		pthread_mutex_unlock(&g_rf_mutex);
		LOG_ERROR("ERROR: rf_cfun_reset err\r\n");
		return -1;
}

int N1_RF_hwreset(int gpio, int on_level)
{
    int ret = -1;
    pthread_mutex_lock(&g_rf_mutex);     

    if( 0 == init_rf_gpio(gpio))
    {
		if(0 == reset_rf(gpio, on_level))
	   		ret = 0;		
	}
    pthread_mutex_unlock(&g_rf_mutex);
    return  ret;
}

void print_RF_status(RFINFO_T* pstrfinfo_)
{
    if(NULL == pstrfinfo_)
		return;  
    printf("Manufacture : %s\n", pstrfinfo_->Manufacture);
    printf("Model : %s\n", pstrfinfo_->Model);    
    printf("Rev : %s\n", pstrfinfo_->Rev);
    printf("SN : %s\n", pstrfinfo_->SN);
    printf("IMEI : %s\n", pstrfinfo_->IMEI);
	printf("SIM STATUS : %s\n", pstrfinfo_->SIMStatus);
    printf("CICCID : %s\n", pstrfinfo_->CICCID);
    printf("CPIN : %s\n", pstrfinfo_->RFsignal.Cpin);
    printf("RSRQ : %d\n", pstrfinfo_->RFsignal.RSRQ);
    printf("RSRP : %d\n", pstrfinfo_->RFsignal.RSRP);
    printf("RSSI : %d\n", pstrfinfo_->RFsignal.RSSI);
    printf("RSSNR : %d\n", pstrfinfo_->RFsignal.RSSNR);
    printf("SINR : %d\n", pstrfinfo_->RFsignal.SINR);
    printf("SignalQos : %d\n", pstrfinfo_->RFsignal.SignalQos);
    printf("Sysmode : %s\n", pstrfinfo_->RFsignal.Sysmode); 
    printf("Opmode : %s\n", pstrfinfo_->RFsignal.Opmode);
    printf("PLMN : %s\n", pstrfinfo_->PLMN);
    printf("Freqband : %s\n", pstrfinfo_->RFsignal.Freqband);
	printf("RF Network connect UTC time : %d\n", pstrfinfo_->rfnetConnectUtctime);
}

static void SetSerial (int fd, int Baud)
{
    struct termios newtio_rf;
    
    bzero (&newtio_rf, sizeof (newtio_rf));   /* clear struct for new port settings */
    newtio_rf.c_cflag = Baud | CS8 | CLOCAL | CREAD;// | CRTSCTS;    /*115200 8N1 HW flow control */
    newtio_rf.c_iflag = IGNPAR;
    newtio_rf.c_oflag = 0;
    newtio_rf.c_cc[VTIME] = 10;    /* inter-character timer 0.1s timeout */
    newtio_rf.c_cc[VMIN] = 0;     
    tcsetattr (fd, TCSANOW, &newtio_rf);
    tcflush (fd, TCIOFLUSH);
}

/*
 *Return:
 *  fd, if okay
 * -1, if failed
 */
static int OpenSerial (char* rfdev_)
{
    int uartfd = -1;
    
    if(NULL == rfdev_)
	return uartfd;
    
    uartfd = open(rfdev_, O_RDWR | O_NOCTTY | O_SYNC);

    if (uartfd < 0)
    {
		return uartfd = -1;
    }
    
    return uartfd;
}
static int isSubstring(const char* s_, const char* sub_, const int slen_, const int sublen_)
{
    int ret = 0;
    int i = 0;
    int j = 0;
    int cnt = 0;
    
    if((NULL == s_) || (NULL == sub_))
	return ret;
    
    for(i  = 0; i < slen_; i++)
    {
	if(s_[i] == sub_[j++])
	{
	    if(++cnt == sublen_)
	    {
		ret = 1;
		break;
	    }
	}
	else
	{
	    j = 0;
	    cnt = 0;
	}
    }
    
    return ret;
}

static int rf_read(const int fd_, char* buf_, unsigned int bufsize_)
{
    int i = 0;
    int cnt = 0;
    int count = 3;
    unsigned int total = 0;
    int ret = 0;
    int utime = 100000;
    char szOK[] = "OK";
    char szERROR[] = "ERROR";
    
    if((fd_ <= 0) || (NULL == buf_))
		return total;
    
    do{
		for(i = 0; i < 10; i++)
		{
	    	ret = read (fd_, buf_ + total, bufsize_ - total);
	    	if (ret > 0)
	    	{
				total += ret;
				buf_[total] = 0;
		
				if(isSubstring(buf_, szOK, total, strlen(szOK)) \
		    		|| isSubstring(buf_, szERROR, total, strlen(szERROR)))
				{
		    		cnt = count + 1;
		    		break;
				}
	    	}
	    
	    	if(ret <= 0){
				break;
	    	}
	    	else
	    	{
				if(bufsize_ == total) 
		    		break;
	    	}
	    	usleep(utime);
		}
	
    }while(++cnt < count);    
    
    return total;
}

static int rf_write(const int fd_, const char* atcmd_, const int size_)
{
    int ret = 0;
    
    if((fd_ <= 0) || (NULL == atcmd_) || ('\0' == atcmd_[0]))
	return ret;
    
    ret = write(fd_, atcmd_, size_);
    tcflush (fd_, TCIFLUSH);
    
    return ret;
}

int parse_RFinfo_request(const char* rfdev_, const char* atcmd_, char* resp_, unsigned int bufsize_)
{
    int ret = -1;
    int fd = -1;
    int len;
    char tmpbuf[128] = {0};
    
    if((NULL == atcmd_) || (NULL == resp_) || (NULL == rfdev_))
	return ret;

    if((fd = OpenSerial((char *)rfdev_)) < 0 ) 
    {
		LOG_ERROR("can not open serial device %s\n",rfdev_);
		return ret;
    }
    else
    {
		SetSerial(fd, B115200);
    }

    len =  strlen(atcmd_) + 1;
    ret = rf_write(fd, atcmd_, len);

    if((ret <= 0) || (ret != len))
    {
		if(ret > 0)
		{
	    	snprintf(tmpbuf, sizeof(tmpbuf) - 1, "Error: Failed to write %s to %s\n", atcmd_, rfdev_);
	    	strncpy(resp_, tmpbuf, bufsize_);
		}
		close(fd);
		return ret = -1;
    }
    
    if( rf_read(fd, resp_, bufsize_) > 0)
		ret = 0;  
    close(fd);
    fd = -1;

    return ret;
}
