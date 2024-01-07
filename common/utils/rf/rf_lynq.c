#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include "types.h"
#include "debug/debug.h"
#include "rf_lynq.h"

extern int parse_RFinfo_request(const char* rfdev_, const char* atcmd_, char* resp_, unsigned int bufsize_);

static void  set_RF_dftSysInfo_LYNQ(RFINFO_T* pstrfinfo_)
{
    if(NULL == pstrfinfo_)
	return;
    
    pstrfinfo_->RFsignal.RSRQ = -200;
    pstrfinfo_->RFsignal.RSRP = -1400;
    pstrfinfo_->RFsignal.RSSI = -1200;
    pstrfinfo_->RFsignal.RSSNR = -10;
    pstrfinfo_->RFsignal.SINR = 0;
    
    return;
}

static int get_RF_ICCID_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    
    if(NULL == pstrfinfo_)
	return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+CICCID\r\n", response, sizeof(response)))
    {
//		LOG_ERROR("ERROR: read AT+CICCID failed\n");
		return ret;
    }
    
    if(1 != sscanf(response, " +CICCID:%s\n", pstrfinfo_->CICCID))
    {
		LOG_ERROR("ERROR: sscanf AT+CICCID failed\n");
		return ret;
    }
    
    return ret = 0;
}


int get_RF_DeviceInfo_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    const char s[] = "\n :";
    char* token;
    int i = 0;
    char szManufacture[] = "Manufacturer";
    char* pos;
    
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
	return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "ATI\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: %s, read ATI failed\n", __func__);
		return ret;
    }
    
    if(NULL == (pos = strstr(response, szManufacture)))
    {
		LOG_ERROR("Error: %s, no Manufacture string in response\n", __func__);
		return ret;
    }
    
    token = strtok(pos, s);
    
    while( token != NULL ) 
    {	
		switch (i++)
		{
	   		case 1:
			strncpy(pstrfinfo_->Manufacture, token, sizeof(pstrfinfo_->Manufacture) - 1);
			break;
	    	case 3:
			strncpy(pstrfinfo_->Model, token, sizeof(pstrfinfo_->Model) - 1);
			break;
	    	case 5:
			strncpy(pstrfinfo_->Rev, token, sizeof(pstrfinfo_->Rev) - 1);
			break;
	    	case 7:
			strncpy(pstrfinfo_->SN, token, sizeof(pstrfinfo_->SN) - 1);
			break;
	    	case 9:
			strncpy(pstrfinfo_->IMEI, token, sizeof(pstrfinfo_->IMEI) - 1);
			break;
	    	default:
			break;
		}
		token = strtok(NULL, s);
    }

	ret = get_RF_ICCID_LYNQ(rfdev_, pstrfinfo_);
	
    return ret;
}


int update_RF_UESysInfo_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    const char s[] = ",";
    char* token;
    int i = 0;
    char* pos = NULL;
    char cpsistr[] = "+CPSI: ";
    
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
	return ret;

    if(0 != parse_RFinfo_request(rfdev_, "AT+CPSI?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CPSI?\r\n, failed\n");
		return ret;
    }
    
    if(NULL == (pos = strstr(response, cpsistr)))
		return ret;
    
    pos += strlen(cpsistr);
    token = strtok(pos, s);
    
    while( token != NULL ) 
    {
	switch (i++)
	{
	    case 0:
		strncpy(pstrfinfo_->RFsignal.Sysmode, token, sizeof(pstrfinfo_->RFsignal.Sysmode) - 1);
		break;
	    case 1:
		strncpy(pstrfinfo_->RFsignal.Opmode, token, sizeof(pstrfinfo_->RFsignal.Opmode) - 1);
		break;
	    case 2:
		strncpy(pstrfinfo_->PLMN, token, sizeof(pstrfinfo_->PLMN) - 1);
		break;
	    case 6:
		strncpy(pstrfinfo_->RFsignal.Freqband, token, sizeof(pstrfinfo_->RFsignal.Freqband) - 1);
		break;
	    default:
		break;
	}
	token = strtok(NULL, s);
    }    
  
    return ret = 0;
}

static int update_RF_PINInfo_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
	return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+CPIN?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CPIN? failed\n");
		return ret;
    }
    
    if(1 != sscanf(response, " +CPIN:%s\n", pstrfinfo_->RFsignal.Cpin))
    {
		LOG_ERROR("ERROR: sscanf AT+CPIN failed\n");
		return ret;
    }
    
    return ret = 0;
}

static int update_RF_SysInfo_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
	return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+MCSQ\r\n", response, sizeof(response)))
    {
		set_RF_dftSysInfo_LYNQ(pstrfinfo_);
		LOG_ERROR("ERROR: read AT+MCSQ failed\n");
		return ret;
    }
        
    if(5 != sscanf(response, " +MCSQ:%d,%d,%d,%d,%d", 
	&pstrfinfo_->RFsignal.RSRQ, &pstrfinfo_->RFsignal.RSRP, &pstrfinfo_->RFsignal.RSSI, &pstrfinfo_->RFsignal.RSSNR, &pstrfinfo_->RFsignal.SINR))
    {
		LOG_ERROR("ERROR: sscanf AT+MCSQ failed\n");
		return ret;
    }
    else
    {
		set_RF_dftSysInfo_LYNQ(pstrfinfo_);
    }
    
    return ret = 0;
}
    

int get_RF_Signal_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
	int ret = 0;
	if(ret == 0)
	{
		ret = update_RF_UESysInfo_LYNQ(rfdev_, pstrfinfo_);
	}
	
    if(ret == 0)
    {
		update_RF_PINInfo_LYNQ(rfdev_, pstrfinfo_);
	}
		
	if(ret == 0)
    {
		update_RF_SysInfo_LYNQ(rfdev_, pstrfinfo_);
	} 

	return ret;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int soft_reset_LYNQ(char* rfdev_)
{
    int ret  = -1;
    char response[256] = {0};
	char packageIden[] = "OK";
    char* pos;
	
    if(NULL == rfdev_)
		return ret;
	
    if(0 != parse_RFinfo_request(rfdev_, "AT+CRESET\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: %s, read AT+CRESET failed\n", __func__);
		return ret;
    }
    
    if(NULL == (pos = strstr(response, packageIden)))
    {
		LOG_ERROR("Error: %s, no OK string in response\n", __func__);
		return ret;
    }
	
    return ret = 0;

}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int reset_cfun_LYNQ(char* rfdev_)
{
	int ret  = -1;
	char response[256] = {0};
	char packageIden[] = "OK";
	char* pos;
	//	char cmd_cfun_reset[] = "AT+CFUN=1,1\r\n";
	char cmd_cfun_off[] = "AT+CFUN=0\r\n";
	char cmd_cfun_on[] = "AT+CFUN=1\r\n";
	if(NULL == rfdev_)
		return ret;
		
	if(0 != parse_RFinfo_request(rfdev_, cmd_cfun_off, response, sizeof(response)))
	{
		LOG_ERROR("ERROR: %s, read AT+CFUN failed\n", __func__);
		return ret;
	}
		
	if(NULL == (pos = strstr(response, packageIden)))
	{
		LOG_ERROR("Error: %s, no OK string in response\n", __func__);
		return ret;
	}

	sleep(10);
	
	if(0 != parse_RFinfo_request(rfdev_, cmd_cfun_on, response, sizeof(response)))
	{
		LOG_ERROR("ERROR: %s, read AT+CFUN failed\n", __func__);
		return ret;
	}
		
	if(NULL == (pos = strstr(response, packageIden)))
	{
		LOG_ERROR("Error: %s, no OK string in response\n", __func__);
		return ret;
	}

	return ret = 0;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int get_cfun_LYNQ(char* rfdev_,int *cfun)
{
    int ret  = -1;
    char response[256] = {0};
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+CFUN?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CFUN failed\n");
		return ret;
    }
    LOG_INFO("AT+CFUN? response:%s\n",response);
	
    if(1 != sscanf(response, "+CFUN: %d\n", cfun))
    {
		LOG_ERROR("ERROR: sscanf AT+CFUN? failed\n");
		return ret;
    }
    
    return ret = 0;


}
/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int get_cpin_LYNQ(char* rfdev_,int *cpinReady)
{
    int ret  = -1;
    char response[256] = {0};
    *cpinReady = 0;
    if(0 != parse_RFinfo_request(rfdev_, "AT+CPIN?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CPIN failed\n");
		return ret;
    }
	
    LOG_INFO("AT+CPIN? response:%s\n",response);

	if(NULL != strstr(response, "READY"))
		*cpinReady = 1;
    
    return ret = 0;


}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int get_creg_LYNQ(char* rfdev_,int *creg)
{
    int ret  = -1;
    char response[256] = {0};
    char* pos = NULL;
    *creg = 0;
    if((NULL == creg) || (NULL == rfdev_))
		return ret;

    if(0 != parse_RFinfo_request(rfdev_, "AT+CREG?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CREG?\r\n, failed\n");
		return ret;
    }
    
    if(NULL == (pos = strstr(response, "+CREG:")))
		return ret;

	if((NULL!=strstr(pos, "1"))||(NULL!=strstr(pos, "5")))
		*creg = 1;
  
    return ret = 0;
}




