/*------------------------------------------------------------------------------
* ping.h : ping
*          Copyright (C) 2018 by QXSI, All rights reserved.
*/
#ifndef __PING_H__
#define __PING_H__

#include "types.h"

extern int32_t ping(const int8_t *daddr, uint16_t id, int32_t times,uint32_t timeout);

extern int32_t ping_intf(const int8_t *daddr, const char* intf, uint16_t id, int32_t times,uint32_t timeout);

#endif /*__PING_H__*/
