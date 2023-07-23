/*
* infinity2m.c- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: joe.su <joe.su@sigmastar.com.tw>
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

#include <common.h>
#include "env.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

#define STORAGE_SPI_NAND_SKIP_SD    BIT4
#define STORAGE_SPI_NAND            (BIT4|BIT0)
#define STORAGE_SPI_NOR             (BIT4|BIT3)
#define STORAGE_SPI_NOR_SKIP_SD     (BIT4|BIT3|BIT0)
#define STORAGE_BOOT_TYPES         (BIT4|BIT3|BIT0)
#define STORAGE_SD_CARD             (STORAGE_SPI_NAND|STORAGE_SPI_NAND)
typedef struct
{
	unsigned char                       tagPrefix[3];
	unsigned char                       reserved[1];
	unsigned char                       tagSuffix[3];


} MS_VERSION;

MS_VERSION UBT_VERSION =
{
	{'M','V','X'},
	{'#'},
	{'X','V','M'},
};
DECLARE_GLOBAL_DATA_PTR;

//#define DEV_BUF_ADDR (PHYS_SDRAM_1+0x3F800)


int arch_cpu_init(void)
{

    gd->xtal_clk=CONFIG_PIUTIMER_CLOCK;



	/* following code is not worked since bd has not allocated yet!!*/
//
//	gd->bd->bi_arch_number = MACH_TYPE_COLUMBUS2;
//
//	/* adress of boot parameters */
//	gd->bd->bi_boot_params = BOOT_PARAMS;


    return 0;
}

// we borrow the DRAN init to do the devinfo setting...
int dram_init(void)
{
    gd->ram_size=CONFIG_UBOOT_RAM_SIZE;

    return 0;
}

DEVINFO_CHIP_TYPE ms_check_chip(void)
{
/*
	U16 chipType;
	chipType = INREG16(GET_REG_ADDR(REG_ADDR_BASE_MIU, 0x69)) & 0xF000;

	if (chipType == 0x6000)
		return DEVINFO_313E;
	else if (chipType == 0x8000)
		return DEVINFO_318;
	else if (chipType == 0x9000)
		return DEVINFO_318;
	else
		return DEVINFO_NON;
*/
	return DEVINFO_NON;
}

int checkboard(void)
{
#ifdef CONFIG_CMD_BDI
        printf("***********************************************************\r\n");
        printf("* MEMORY LAYOUT                                            \r\n");
        printf("* PHYS_SDRAM_1:           0X%08x                           \r\n",PHYS_SDRAM_1);
        printf("* PHYS_SDRAM_1_SIZE:      0X%08x                           \r\n",PHYS_SDRAM_1_SIZE);
        printf("* CONFIG_SYS_TEXT_BASE:   0X%08x                           \r\n",CONFIG_SYS_TEXT_BASE);
        printf("* CONFIG_SYS_SDRAM_BASE:  0X%08x                           \r\n",CONFIG_SYS_SDRAM_BASE);
        printf("* CONFIG_SYS_INIT_SP_ADDR:0X%08x  (gd_t *)pointer          \r\n",CONFIG_SYS_INIT_SP_ADDR);
        printf("* SCFG_MEMP_START:        0X%08x                           \r\n",SCFG_MEMP_START);
        printf("* SCFG_PNLP_START:        0X%08x                           \r\n",SCFG_PNLP_START);
        printf("* BOOT_PARAMS:            0X%08x                           \r\n",BOOT_PARAMS);
        printf("* CONFIG_SYS_LOAD_ADDR:   0X%08x                           \r\n",CONFIG_SYS_LOAD_ADDR);
        //printf("* KERNEL_RAM_BASE:0X%08x                                   \r\n",KERNEL_RAM_BASE);
        printf("* CONFIG_UNLZO_DST_ADDR:  0X%08x                           \r\n",CONFIG_UNLZO_DST_ADDR);
        printf("\r\n");
        printf("* CONFIG_ENV_SIZE:        0X%08x                           \r\n",CONFIG_ENV_SIZE);
        printf("* CONFIG_SYS_MALLOC_LEN:  0X%08x                           \r\n",CONFIG_SYS_MALLOC_LEN);
        printf("* CONFIG_STACKSIZE:       0X%08x                           \r\n",CONFIG_STACKSIZE);
        printf("* KERNEL_IMAGE_SIZE:      0X%08x                           \r\n",KERNEL_IMAGE_SIZE);
        printf("***********************************************************\r\n");
#endif
	return 0;
}



DEVINFO_BOOT_TYPE ms_devinfo_boot_type(void)
{
    U16 u16Storage;
    u16Storage = (INREG16( GET_REG_ADDR(REG_ADDR_BASE_PM_SLEEP, 0x1F) ) >> 10) & STORAGE_BOOT_TYPES;

    if(u16Storage == STORAGE_SPI_NOR || u16Storage == STORAGE_SPI_NOR_SKIP_SD)
    {
        return DEVINFO_BOOT_TYPE_SPI;
    }
    else if(u16Storage == STORAGE_SPI_NAND || u16Storage == STORAGE_SPI_NAND_SKIP_SD)
    {
        return DEVINFO_BOOT_TYPE_SPINAND_INT_ECC;
    }
    return DEVINFO_BOOT_TYPE_NONE;
}


#ifdef CONFIG_BOARD_LATE_INIT
extern void *malloc(unsigned int);
#define BUF_SIZE 256

int board_late_init(void)
{
    return 0;
}
#endif

#ifndef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#if (defined(CONFIG_MS_NAND) && defined(CONFIG_MS_EMMC))
extern void mmc_env_relocate_spec(void);
extern void nand_env_relocate_spec(void);
extern int mmc_env_init(void);
extern int nand_env_init(void);
extern int mmc_saveenv(void);
extern int nand_saveenv(void);

env_t *env_ptr;

void env_relocate_spec(void)
{
	if(DEVINFO_BOOT_TYPE_EMMC==ms_devinfo_boot_type())
	{
		return mmc_env_relocate_spec();
	}
	else if(DEVINFO_BOOT_TYPE_NAND==ms_devinfo_boot_type())
	{
		return nand_env_relocate_spec();
	}
}


int env_init(void)
{
	if(DEVINFO_BOOT_TYPE_EMMC==ms_devinfo_boot_type())
	{
		return mmc_env_init();
	}
	else if(DEVINFO_BOOT_TYPE_NAND==ms_devinfo_boot_type())
	{
		return nand_env_init();
	}

}

int saveenv(void)
{
	if(DEVINFO_BOOT_TYPE_EMMC==ms_devinfo_boot_type())
	{
		return mmc_saveenv();
	}
	else if(DEVINFO_BOOT_TYPE_NAND==ms_devinfo_boot_type())
	{
		return nand_saveenv();
	}
}
#endif
#endif

#define  reg_flush_op_on_fire           (0x1F000000 + 0x102200*2 + 0x05*4)
#define  reg_Flush_miu_pipe_done_flag   (0x1F000000 + 0x102200*2 + 0x10*4)

inline void _chip_flush_miu_pipe(void)
{

	unsigned short dwReadData = 0;

	//toggle the flush miu pipe fire bit
	*(volatile unsigned short *)(reg_flush_op_on_fire) = 0x10;
	*(volatile unsigned short *)(reg_flush_op_on_fire) = 0x11;

	do
	{
		dwReadData = *(volatile unsigned short *)(reg_Flush_miu_pipe_done_flag);
		dwReadData &= (1<<12);  //Check Status of Flush Pipe Finish

	} while(dwReadData == 0);
}

extern void Chip_Flush_Memory(void);
extern void Chip_Read_Memory(void);

inline void Chip_Flush_Memory(void)
{
	_chip_flush_miu_pipe();
}

inline void Chip_Read_Memory(void)
{
	_chip_flush_miu_pipe();
}
