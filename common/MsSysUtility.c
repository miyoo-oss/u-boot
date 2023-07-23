/*
* MsSysUtility.c- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: karl.xiao <karl.xiao@sigmastar.com.tw>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/

#ifndef __MSSYSUTILITY_C__
#define __MSSYSUTILITY_C__


#include <common.h>
#include <command.h>
#include <MsDebug.h>
#include <MsSysUtility.h>
#include "../drivers/mstar/gpio/drvGPIO.h"


#ifdef CONFIG_MS_GPIO
int do_gpio( struct cmd_tbl * cmdtp, int flag, int argc, char * const argv[])
{
    UBOOT_TRACE("IN\n");
    MS_GPIO_NUM gpio_num_p1;

    if(3 > argc)
    {
        if((NULL == strcmp(argv[1], "list")) && (2 == argc))
        {
        }
        else
        {
            cmd_usage(cmdtp);
            return -1;
        }
    }

    gpio_num_p1 = (MS_GPIO_NUM)simple_strtoul(argv[2], NULL, 10);

    if((NULL == strcmp(argv[1], "get")) || (NULL == strncmp(argv[1], "inp", 3)))
    {
        MDrv_GPIO_Pad_Set(gpio_num_p1);
        mdrv_gpio_set_input(gpio_num_p1);      
        UBOOT_INFO("IN  ");
        UBOOT_INFO("pin=%d\n", mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "output", 3))
    {
        MDrv_GPIO_Pad_Set(gpio_num_p1);

        if('1' == argv[3][0])
            mdrv_gpio_set_high(gpio_num_p1);
        else if('0' == argv[3][0])
            mdrv_gpio_set_low(gpio_num_p1);

        UBOOT_INFO("gpio[%d] is %d\n", gpio_num_p1, mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "toggle", 3))
    {
        MDrv_GPIO_Pad_Set(gpio_num_p1);

        if(mdrv_gpio_get_level(gpio_num_p1)) 
        {
            mdrv_gpio_set_low(gpio_num_p1);        
        }
        else 
        {    
            mdrv_gpio_set_high(gpio_num_p1);
        }

        UBOOT_INFO("gpio[%d] is %d\n", gpio_num_p1, mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "state", 3))
    {
        if(mdrv_gpio_get_inout(gpio_num_p1)) 
        {
            UBOOT_INFO("IN  ");
        }
        else
        {
            UBOOT_INFO("OUT ");
        }
        UBOOT_INFO("pin=%d\n", mdrv_gpio_get_level(gpio_num_p1));
    }
    else if(NULL == strncmp(argv[1], "list", 3))
    {
        U8 i;
        U8 size;

        if(2 == argc)
        {
            size = 200;
        }
        else
        {
            size = (MS_GPIO_NUM)simple_strtoul(argv[2], NULL, 10);
        }
        
        for(i=0; i<size; i++)
        {
            UBOOT_INFO("GPIO %3d ", i);
            gpio_num_p1 = i;
        
            if(mdrv_gpio_get_inout(gpio_num_p1)) 
            {
                UBOOT_INFO("IN  ");
            }
            else
            {
                UBOOT_INFO("Out ");
            }
            UBOOT_INFO("pin=%d\n", mdrv_gpio_get_level(gpio_num_p1));

            if((i>0) && (i%10==0)) UBOOT_INFO("\n");
        }

    }
    else
    {
        cmd_usage(cmdtp);
        return -1;
    }

    UBOOT_TRACE("OK\n");    
    return 0;
}


U_BOOT_CMD(
    gpio, 4, 0, do_gpio,
    "Config gpio port",
    "(for 2nd parameter, you must type at least 3 characters)\n"
    "gpio output <gpio#> <1/0>  : ex: gpio output 69 1\n"
    "gpio input/get <gpio#>     : ex: gpio input 10  (gpio 10 set as input)\n"
    "gpio toggle <gpio#>        : ex: gpio tog 49 (toggle)\n"
    "gpio state <gpio#>         : ex: gpio sta 49 (get i/o status(direction) & pin status)\n"
    "gpio list [num_of_pins]    : ex: gpio list 10 (list GPIO1~GPIO10 status)\n"
    );

#endif  //#ifdef CONFIG_MS_GPIO

#endif //#define __MSSYSUTILITY_C__
