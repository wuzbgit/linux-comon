#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include "types.h"
#include "rf_quectel.h"
#include "debug/debug.h"

extern int parse_RFinfo_request(const char* rfdev_, const char* atcmd_, char* resp_, unsigned int bufsize_);

static char * removeStringCharter(char *str,char ch)
{
	char *ptr = str;
	int i = 0;
	while(*str != '\0')
	{
		if(*str != ch)
		{
			*(ptr+i) = *str;
			i++;
		}
		str++;
	}

	*(ptr + i) = '\0';
	return ptr;
}
static void  set_RF_dftSysInfo_QUECTEL(RFINFO_T* pstrfinfo_)
{
    if(NULL == pstrfinfo_)
		return;
    
    pstrfinfo_->RFsignal.RSRQ = -9999;
    pstrfinfo_->RFsignal.RSRP = -9999;
    pstrfinfo_->RFsignal.RSSI = -9999;
    pstrfinfo_->RFsignal.RSSNR = -9999;
    pstrfinfo_->RFsignal.SINR = -9999;
    
    return;
}

static bool rf_cmdecho(const char *response,const char *cmd)
{
	char* pos = NULL;
	if(!strcmp(response, cmd)) {
		return true;
	}

	return false;
}

static int get_RF_ICCID_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    char packageIden[]="OK\r\n";
	char *pos = NULL;
	
    if(NULL == pstrfinfo_)
		return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+QCCID\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+QCCID failed\n");
		return ret;
    }

	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no package end string in response\n", __func__);
		return ret;
    }

	if(NULL == (pos = strstr((const char *)response, "+QCCID:")))
		return ret;
	
    if(1 != sscanf(pos, "+QCCID:%s\n", pstrfinfo_->CICCID))
    {
		LOG_ERROR("ERROR: sscanf AT+QCCID failed\n");
		return ret;
    }
    
    return ret = 0;
}

int get_RF_DeviceInfo_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    char aticmd[] = "ATI\r\n";
    char imeicmd[] = "AT+GSN\r\n";
	char cgsncmd[] = "AT+CGSN\r\n";
	char* pos = NULL;
	char packageIden[]="OK\r\n";

    if((NULL == pstrfinfo_) || (NULL == rfdev_))
		return ret;    

    if(0 != parse_RFinfo_request(rfdev_, aticmd, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read %s failed\n", aticmd);
		return ret;
    }
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no package end string in response:%s\n",__func__,response);
		return ret;
    }

	pos = response;
	ret = strncmp(response, "ATI",strlen("ATI"));
	if(!ret) {
		pos = response + strlen("ATI\n\r");
	}

    if(3 != sscanf(pos, "%s\n\r%s\n\rRevision: %s\n\r", pstrfinfo_->Manufacture, pstrfinfo_->Model, pstrfinfo_->Rev))
    {
		LOG_ERROR("ERROR: sscanf ATI failed,response:%s\n",pos);
		return ret;
    }  

    if(0 != parse_RFinfo_request(rfdev_, imeicmd, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read %s failed\n",imeicmd);
		return ret;
    }
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no package end string in response:%s\n",__func__,response);
		return ret;
    }
	if(NULL == (pos = strstr(response, "AT+GSN\n\r"))) {
		pos = response;
	}
	else {
		pos += strlen("AT+GSN\n\r");
	}
    if(1 != sscanf(pos, "%s\n\r", pstrfinfo_->IMEI))
    {
		LOG_ERROR("ERROR: sscanf AT+CGSN failed,response:%s\n",response);
		return ret;
    }    
	
    if(0 != parse_RFinfo_request(rfdev_, cgsncmd, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read %s failed,response:%s\n",cgsncmd,response);
		return ret;
    }
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no package end string in response:%s\n",__func__,response);
		return ret;
    }
	if(NULL == (pos = strstr(response, "AT+CGSN\n\r"))) {
		pos = response;
	}
	else {
		pos += strlen("AT+CGSN\n\r");
	}			
    if(1 != sscanf(pos, "%s\n\r", pstrfinfo_->SN))
    {
		LOG_ERROR("ERROR: sscanf AT+CGSN failed,response:%s\n",response);
		return ret;
    }    

    if(0 != parse_RFinfo_request(rfdev_, "AT+QSIMSTAT?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+QSIMSTAT? failed\n");
		return ret;
    }
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no package end string in response:%s\n",__func__,response);
		return ret;
    }
	
	if(NULL == (pos = strstr((const char *)response, "+QSIMSTAT:")))
    {
		LOG_ERROR("Error: %s, no package end string response:%s\n",__func__,response);
		return ret;
    }
	pos = pos + strlen("+QSIMSTAT: ");
	int enable,stat;
	if(2 != sscanf(pos,"%d,%d",&enable,&stat))
	{
		LOG_ERROR("Error: %s, sscanf fail\n",__func__);
		return ret;
	}
	if(stat == 0)
	{
		strcpy(pstrfinfo_->SIMStatus,"Removed");
	}
	else if(stat == 1)
	{
		strcpy(pstrfinfo_->SIMStatus,"Inserted");
	}
	else
	{
		strcpy(pstrfinfo_->SIMStatus,"Unkown");
	}

	if (!strcmp(pstrfinfo_->SIMStatus,"Inserted")) {
		ret = get_RF_ICCID_QUECTEL(rfdev_, pstrfinfo_);
	}
	
    return ret;
}

int update_RF_UESysInfo_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    const char s[] = ",";
    char* token;
    int i = 0;
    char* pos = NULL;
    char queryNetcmd[] = "AT+QNWINFO\r\n";
	char packageIden[] = "OK\r\n";

    if((NULL == pstrfinfo_) || (NULL == rfdev_))
	return ret;

    if(0 != parse_RFinfo_request(rfdev_, queryNetcmd, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read %s failed\n" ,queryNetcmd);
		return ret;
    }

    if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
		return ret;

    if(NULL == (pos = strstr((const char *)response, "+QNWINFO:")))
    {
    	LOG_ERROR("find +QNWINFO: failed\n");
		return ret;
	}
		
    pos += strlen("+QNWINFO:");
    token = strtok(pos, s);
    
    while( token != NULL ) 
    {
		switch (i++)
		{
	    	case 0:
			strncpy(pstrfinfo_->RFsignal.Sysmode, token, sizeof(pstrfinfo_->RFsignal.Sysmode) - 1);
			break;
	    	case 1:
			strncpy(pstrfinfo_->PLMN, token, sizeof(pstrfinfo_->PLMN) - 1);
			break;
	    	case 2:
			strncpy(pstrfinfo_->RFsignal.Freqband, token, sizeof(pstrfinfo_->RFsignal.Freqband) - 1);
			break;
	    	default:
			break;
		}
		token = strtok(NULL, s);
    }    

	removeStringCharter(pstrfinfo_->RFsignal.Sysmode,'\"');
	removeStringCharter(pstrfinfo_->PLMN,'\"');
	removeStringCharter(pstrfinfo_->RFsignal.Freqband,'\"');
    return ret = 0;
}

typedef enum
{
	MODE_NO_SERVICE = 0,
	MODE_GSM,
	MODE_WCDMA,
	MODE_TDSCDMA,
	MODE_LTE,
	MODE_CDMA,
	MODE_EVDO,
	MODE_CDMA_EVDO,
	MODE_END
}QCsq_Sysmode;
static char qcsqsysmode[MODE_END][15] = {"\"NOSERVICE\"","\"GSM\"","\"WCDMA\"","TDSCDMA","\"LTE\"","\"CDMA\"","\"EVDO\"","\"CDMA-EVDO\""};

static int update_RF_PINInfo_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
    char packageIden[] = "OK\r\n";
	char *pos = NULL;
	
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
	return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+CPIN?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CPIN? failed\n");
		return ret;
    }
	
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
		return ret;

	if(NULL == (pos = strstr((const char *)response, "+CPIN:")))
		return ret;
		
    if(1 != sscanf(pos, "+CPIN:%s\n", pstrfinfo_->RFsignal.Cpin))
    {
		LOG_ERROR("ERROR: sscanf AT+CPIN failed\n");
		return ret;
    }
    
    return ret = 0;
}

static int get_rssi_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
	int ret  = -1;
    char response[256] = {0};
	char* pos = NULL;
	const char s[] = ",";
    char* token;
	char packageIden[] = "OK\r\n";

    if((NULL == pstrfinfo_) || (NULL == rfdev_))
		return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+CSQ\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CSQ failed\n");
		set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
		return ret;
    }
	
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
		return ret;
	
	if(NULL == (pos = strstr((const char *)response, "+CSQ:")))
    {
		LOG_ERROR("Error: %s, no CSQ string in response\n",__func__);
		return ret;
    }

	pos += strlen("+CSQ:");
    token = strtok(pos, s);

	if(1!=sscanf(token, "%d",&pstrfinfo_->RFsignal.RSSI))
	{
		set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
		LOG_ERROR("ERROR: sscanf AT+CSQ failed\n");
		return ret;
	}

	return 0;	
}

int get_nwmode_QUECTEL(const char* rfdev_, RF_NWMODE *mode)
{
    int ret  = -1;
    char response[256] = {0};
	char* pos = NULL;
	char packageIden[] = "OK\r\n";
	int idx;
    if((NULL == mode) || (NULL == rfdev_))
		return ret;

    if(0 != parse_RFinfo_request(rfdev_, "AT+QCSQ\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+QCSQ failed\n");
		return ret;
    }
	
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
		return ret;
	
	if(NULL == (pos = strstr((const char *)response, "+QCSQ:")))
    {
		LOG_ERROR("Error: %s, no QCSQ string in response\n",__func__);
		return ret;
    }

	for(idx=0;idx<MODE_END;idx++)
	{
		if(NULL != (pos = strstr((const char *)response, (const char *)qcsqsysmode[idx])))
		{
			pos += strlen(qcsqsysmode[idx]);
			break;
		}	
	}

	switch(idx)
	{
		case MODE_NO_SERVICE:
			*mode = RF_NWMODE_NONE;
			break;
		case MODE_GSM:
			*mode = RF_NWMODE_2G;
			break;
		case MODE_WCDMA:
		case MODE_TDSCDMA:
		case MODE_CDMA:
		case MODE_EVDO:
		case MODE_CDMA_EVDO:
			*mode = RF_NWMODE_3G;
			break;
		case MODE_LTE:
			*mode = RF_NWMODE_4G;
			break;			
		default:
			break;
	}

    return 0;
}

static int update_RF_SysInfo_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
	char* pos = NULL;
	char packageIden[] = "OK\r\n";
	int idx;
	int reserved[20] = {0};
	int sret;
    if((NULL == pstrfinfo_) || (NULL == rfdev_))
		return ret;

    if(0 != parse_RFinfo_request(rfdev_, "AT+QCSQ\r\n", response, sizeof(response)))
    {
		set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
		LOG_ERROR("ERROR: read AT+QCSQ failed\n");
		return ret;
    }
	
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
		return ret;
	
	if(NULL == (pos = strstr((const char *)response, "+QCSQ:")))
    {
		LOG_ERROR("Error: %s, no QCSQ string in response\n",__func__);
		return ret;
    }

	for(idx=0;idx<MODE_END;idx++)
	{
		if(NULL != (pos = strstr((const char *)response, (const char *)qcsqsysmode[idx])))
		{
			pos += strlen(qcsqsysmode[idx]);
			break;
		}	
	}

	set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
	get_rssi_QUECTEL(rfdev_, pstrfinfo_);
	switch(idx)
	{
		case MODE_NO_SERVICE:
			return ret;
			break;
		case MODE_GSM:
			if(1!=sscanf(pos, ",%d",&pstrfinfo_->RFsignal.RSSI))
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;					
			}
			break;
		case MODE_WCDMA:
			if(3!=sscanf(pos, ",%d,%d,%d",&pstrfinfo_->RFsignal.RSSI,&reserved[0],&reserved[1]))
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;					
			}
			break;
		case MODE_TDSCDMA:
			if(3 != sscanf(pos, ",%d,%d,%d",&pstrfinfo_->RFsignal.RSSI,&reserved[0],&reserved[1]))
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;				
			}
			break;
		case MODE_LTE:
			sret = sscanf(pos, ",%d,%d,%d,%d",&pstrfinfo_->RFsignal.RSSI,&pstrfinfo_->RFsignal.RSRP,&pstrfinfo_->RFsignal.SINR,&pstrfinfo_->RFsignal.RSRQ);
			if(4 != sret)
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;				
			}
			pstrfinfo_->RFsignal.SINR = pstrfinfo_->RFsignal.SINR/5-20;
			break;
		case MODE_CDMA:
			if(2!=sscanf(pos, ",%d,%d",&pstrfinfo_->RFsignal.RSSI,&reserved[0]))
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;				
			}
			break;
		case MODE_EVDO:
			if(3!=sscanf(pos, ",%d,%d,%d",&pstrfinfo_->RFsignal.RSSI,&reserved[0],&pstrfinfo_->RFsignal.SINR))
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;				
			}
			pstrfinfo_->RFsignal.SINR = pstrfinfo_->RFsignal.SINR/5-20;
			break;
		case MODE_CDMA_EVDO:
			if(5!=sscanf(pos, ",%d,%d,%d,%d,%d",&pstrfinfo_->RFsignal.RSSI,&reserved[0],&reserved[1],&reserved[2],&pstrfinfo_->RFsignal.SINR))
			{
				set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
				LOG_ERROR("ERROR: sscanf AT+QCSQ failed\n");
				return ret;						
			}
			pstrfinfo_->RFsignal.SINR = pstrfinfo_->RFsignal.SINR/5-20;
			break;
		default:
			return 0;
			break;
	}

	pstrfinfo_->RFsignal.RSSI = pstrfinfo_->RFsignal.RSSI-120;
    return ret = 0;
}

static int update_RF_MCC_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
    int ret  = -1;
    char response[256] = {0};
	char* pos = NULL;
	const char s[] = ",";
    char* token;
	char packageIden[] = "OK\r\n";

    if((NULL == pstrfinfo_) || (NULL == rfdev_))
		return ret;
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+COPS?\r\n", response, sizeof(response)))
    {
		//set_RF_dftSysInfo_QUECTEL(pstrfinfo_);
		LOG_ERROR("ERROR: read AT+COPS? failed\n");
		return ret;
    }
	
	if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
		return ret;
	
	if(NULL == (pos = strstr((const char *)response, "+COPS:")))
    {
		LOG_ERROR("Error: %s, no +COPS: string in response\n",__func__);
		return ret;
    }

	pos += strlen("+COPS:");
    token = strtok(pos, s);
    int i = 0;
    while( token != NULL ) 
    {
		switch (i++)
		{
	    	case 0:
			break;
	    	case 1:
			break;
	    	case 2:
			strncpy(pstrfinfo_->RFsignal.Opmode, token, sizeof(pstrfinfo_->RFsignal.Opmode) - 1);
			break;
	    	default:
			break;
		}
		token = strtok(NULL, s);
    } 
	removeStringCharter(pstrfinfo_->RFsignal.Opmode,'\"');
    return ret = 0;
}

int get_RF_Signal_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_)
{
	int ret = 0;

	update_RF_UESysInfo_QUECTEL(rfdev_, pstrfinfo_);
	
    update_RF_PINInfo_QUECTEL(rfdev_, pstrfinfo_);

	update_RF_SysInfo_QUECTEL(rfdev_, pstrfinfo_); 

	update_RF_MCC_QUECTEL(rfdev_,pstrfinfo_);
	
	return ret = 0;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int soft_reset_QUECTEL(char* rfdev_)
{
    int ret  = -1;
    char response[256] = {0};
	char packageIden[] = "OK";
    char* pos;
	
    if(NULL == rfdev_)
		return ret;
	
    if(0 != parse_RFinfo_request(rfdev_, "AT+QPOWD\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: %s, read AT+QPOWD failed\n", __func__);
		return ret;
    }
    
    if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
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
int get_cfun_QUECTEL(char* rfdev_,int *cfun)
{
    int ret  = -1;
    char response[256] = {0};
    
    if(0 != parse_RFinfo_request(rfdev_, "AT+CFUN?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CFUN failed\n");
		return ret;
    }
	
	if(strstr((const char *)response, "+CFUN:") == NULL)
	{
		LOG_ERROR("ERROR: strstr +CFUN: failed\n");
		return ret;
	}
	if(strstr((const char *)response, "1") == NULL)
	{
		*cfun = 0;
	}
	else
	{
		*cfun = 1;
	}
		
    return ret = 0;
}
/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int nwscanmode_QUECTEL(char* rfdev_)
{
    int ret  = -1;
    char response[256] = {0};
	char *pos = NULL;
	char packageIden[] = "OK";
	char nwscan_cmd[] = "AT+QCFG=nwscanmode\r\n";
	
    if(NULL == rfdev_)
		return ret;

    if(0 != parse_RFinfo_request(rfdev_, nwscan_cmd, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: %s, read AT+CFUN failed\n", __func__);
		return ret;
    }
    
    if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
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
int reset_cfun_QUECTEL(char* rfdev_)
{
    int ret  = -1;
    char response[256] = {0};
	char packageIden[] = "OK";
    char* pos;
	char cmd_cfun_off[] = "AT+CFUN=0\r\n";
	char cmd_cfun_on[] = "AT+CFUN=1\r\n";
    if(NULL == rfdev_)
		return ret;

    if(0 != parse_RFinfo_request(rfdev_, cmd_cfun_off, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: %s, read AT+CFUN failed\n", __func__);
		return ret;
    }
    
    if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no OK string in response\n", __func__);
		return ret;
    }


	if(0 != parse_RFinfo_request(rfdev_, cmd_cfun_on, response, sizeof(response)))
    {
		LOG_ERROR("ERROR: %s, read AT+CFUN failed\n",__func__);
		return ret;
    }
    
    if(NULL == (pos = strstr((const char *)response, (const char *)packageIden)))
    {
		LOG_ERROR("Error: %s, no OK string in response\n",__func__);
		return ret;
    }
	
    return ret = 0;

}
/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int get_cpin_QUECTEL(char* rfdev_,int *cpinReady)
{
    int ret  = -1;
    char response[256] = {0};
    *cpinReady = 0;
    if(0 != parse_RFinfo_request(rfdev_, "AT+CPIN?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CPIN failed\n");
		return ret;
    }
	
	if(strstr((const char *)response, "+CPIN:") == NULL)
	{
		LOG_ERROR("ERROR: strstr +CPIN: failed\n");
		return ret;
	}

	if(NULL == strstr((const char *)response, "READY"))
		*cpinReady = 0;
	else
		*cpinReady = 1;
		
    return ret = 0;


}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
int get_creg_QUECTEL(char* rfdev_,int *creg)
{
    int ret  = -1;
    char response[256] = {0};
    char* pos = NULL;
    *creg = 0;
    if((NULL == creg) || (NULL == rfdev_))
		return ret;

    if(0 != parse_RFinfo_request(rfdev_, "AT+CREG?\r\n", response, sizeof(response)))
    {
		LOG_ERROR("ERROR: read AT+CREG?f ailed\r\n");
		return ret;
    }

    if(NULL == (pos = strstr((const char *)response, "+CREG:")))
		return ret;

	if((NULL!=strstr((const char *)pos, "1"))||(NULL!=strstr((const char *)pos, "5")))
		*creg = 1;
  	else
		*creg = 0;
    return ret = 0;
}



