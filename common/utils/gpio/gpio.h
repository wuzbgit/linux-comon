/******************************************************************************
* gpio handler for myir company
*/
#ifndef __MYIR_GPIO_H__
#define __MYIR_GPIO_H__


enum {
    GPIO_DIR_IN,
    GPIO_DIR_OUT
};

enum{
    GPIO_RESET_DB1 = 32*2 + 17, /*GPIO2_17, output*/
    GPIO_RESET_DB2 = 32*0 + 8,  /*GPIO0_8 , output*/
    GPIO_RESET_DB3 = 32*0 + 9,  /*GPIO0_9 , output*/

    GPIO_PV_DHB1   = 32*3 + 14, /*GPIO3_14, input*/
    GPIO_PV_DHB2   = 32*3 + 16, /*GPIO3_16, input*/
    GPIO_PV_DHB3   = 32*3 + 17, /*GPIO3_17, input*/

    GPIO_MPU_ENMOD = 32*0 + 2,  /*GPIO0_2,  output*/

    GPIO_COM2_SWI1 = 32*0 + 10, /*GPIO0_10, output*/ /*SWI1 = 1, SWI2 = 0, com2 is for print*/
    GPIO_COM2_SWI2 = 32*0 + 11, /*GPIO0_11, output*/ /*SWI1 = 0, SWI2 = 1, com2 is for rs485*/

    //GPIO_RS485_ENIO  = 32*3 + 15, /*GPIO3_15, output*/ /*0: rs485 is tx; 1: rs485 is rx*/

    GPIO_OPTSIG_WARN = 32*3 + 20, /*GPIO3_20, input*/

    GPIO_TF_CHECK    = 32*2 + 25, /*GPIO2_25, input*/

    GPIO_RF_RESET    = 32*0 + 23,  /*GPIO0_23, output*/ // high > 200ms to rst

    GPIO_RF_PWR      = 32*3 + 15, /*GPIO3_15, output*/  // high pwr on; low pwr off

    GPIO_SWITCH_REST = 32*0 + 26,  /*GPIO0_26, output*/ // low > 10ms to rst
    GPIO_ETH0_REST   = 32*0 + 22,  /*GPIO0_22, output*/ // low > 10ms to rst
    GPIO_ETH1_REST   = 32*0 + 27,  /*GPIO0_27, output*/ // low > 10ms to rst
};

/*
 * Input:         number        --  gpio number integer value. eg.gpio3_7 = 3*32+7
                  level         --  set gpio level. 1: high; 0: low.
                  dir           --  set gpio direction. GPIO_DIR_IN: input; GPIO_DIR_OUT: output
 */
extern int	gpio_export(int number);
extern int	gpio_unexport(int number);
extern int	gpio_set_direction(int number, int dir);
extern int	gpio_get_direction(int number, char* dir);
extern int	gpio_set_level(int number, int level);
extern int 	gpio_get_level(int number, char *level);

#endif /*__MYIR_GPIO_H__*/

