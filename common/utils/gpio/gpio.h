/******************************************************************************
* gpio handler for myir company
*/
#ifndef __MYIR_GPIO_H__
#define __MYIR_GPIO_H__


enum {
    GPIO_DIR_IN,
    GPIO_DIR_OUT
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
extern int 	gpio_get_level(int number, int *level);

#endif /*__MYIR_GPIO_H__*/

