#include <stdio.h>
#include <ctype.h>
#include "strfunc.h"

#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

int atoul(char *str,unsigned * pulValue);
int atoulx(char *str,unsigned * pulValue);

int StrToNumber(char *str , unsigned * pulValue)
{
    /* check hex string */
    if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') )
    {
        if (*(str+2) == '\0')
        {
            return HI_FAILURE;
        }
        else
        {
            return atoulx(str+2,pulValue);
        }
    }
    else
    {
        return atoul(str,pulValue);
    }
}

int atoul(char *str,unsigned * pulValue)
{
    unsigned ulResult=0;

    while (*str)
    {
        if (isdigit((int)*str))
        {
            /* max value: 0xFFFFFFFF(4294967295),
               X * 10 + (*str)-48 <= 4294967295
               so, X = 429496729 */
            if ((ulResult<429496729) || ((ulResult==429496729) && (*str<'6')))
            {
                ulResult = ulResult*10 + (*str)-48;
            }
            else
            {
                *pulValue = ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    *pulValue=ulResult;
    return HI_SUCCESS;
}

int  atoulx(char *str,unsigned * pulValue)
{
    unsigned   ulResult=0;
    unsigned char ch;

    while (*str)
    {
        ch=toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue=ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    
    *pulValue=ulResult;

    return HI_SUCCESS;
}



