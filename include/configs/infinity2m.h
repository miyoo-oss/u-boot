#ifndef __CONFIG_H
#define __CONFIG_H

/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/
/* This is columbus2 hardware */
#define CONFIG_ARCH_INFINITY2M	1
#define CONFIG_ARMCORTEXA7		/* This is an ARM V7 CPU core */
#define CONFIG_SYS_L2CACHE_OFF		/* No L2 cache */
/*#define CONFIG_SYS_ARCH_TIMER   1*/
#define CONFIG_MS_PIU_TIMER   1
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_DELAY_STR "\x0d" /* press ENTER to interrupt BOOT */

#define CONFIG_DISPLAY_BOARDINFO    1

#define CONFIG_BOARD_LATE_INIT

#if CONFIG_VERSION_FPGA
#define CONFIG_SYS_HZ_CLOCK 12000000
#define CONFIG_UART_CLOCK   12000000
#define CONFIG_BAUDRATE	    38400
#define CONFIG_PIUTIMER_CLOCK 12000000
#else
#define CONFIG_SYS_HZ_CLOCK 400000000
#define CONFIG_UART_CLOCK   172800000
#define CONFIG_BAUDRATE	    115200
#define CONFIG_PIUTIMER_CLOCK 12000000
#endif

#define CONFIG_WDT_CLOCK        CONFIG_PIUTIMER_CLOCK


/* define baud rate */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*------------------------------------------------------------------------------
    Macro
-------------------------------------------------------------------------------*/

/* boot delay time */
#define CONFIG_SYS_CACHELINE_SIZE	64


#define CONFIG_ZERO_BOOTDELAY_CHECK

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_CBSIZE       1024             /* Console I/O Buffer Size  */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	        (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	        64		    /* max number of command args   */
#define CONFIG_SYS_BARGSIZE	    CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size    */

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	    (4*1024*1024)  /* regular stack */

/*
 * Physical Memory Map
 */
#define PHYS_SDRAM_1		0x20000000	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x08000000	/* 128 MB */

/*Enable watchdog*/
/*#define CONFIG_HW_WATCHDOG 1*/
#ifdef CONFIG_HW_WATCHDOG
#define CONFIG_HW_WATCHDOG_TIMEOUT_S	60
#endif



/* following is a multiple DRAM bank sample*/
/*
 * Physical Memory Map
 */
/* #define CONFIG_NR_DRAM_BANKS	4			// we have 2 banks of DRAM
 * #define PHYS_SDRAM_1			0xa0000000	// SDRAM Bank #1
 * #define PHYS_SDRAM_1_SIZE		0x04000000	// 64 MB
 * #define PHYS_SDRAM_2			0xa4000000	// SDRAM Bank #2
 * #define PHYS_SDRAM_2_SIZE		0x00000000	// 0 MB
 * #define PHYS_SDRAM_3			0xa8000000	// SDRAM Bank #3
 * #define PHYS_SDRAM_3_SIZE		0x00000000	// 0 MB
 * #define PHYS_SDRAM_4			0xac000000	// SDRAM Bank #4
 * #define PHYS_SDRAM_4_SIZE		0x00000000	// 0 MB
 */


#define CONFIG_SYS_MEMTEST_START	0x20000000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x24000000	/* 0 ... 64 MB in DRAM	*/

#define CONFIG_SYS_TEXT_BASE	0x23E00000
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_TEXT_BASE  - GENERATED_GBL_DATA_SIZE)

#define CONFIG_UBOOT_RAM_SIZE   0x04000000 // let us to use only 64MB for uboot



/* RAM base address */
#define RAM_START_ADDR          0x20000000

/* RAM size */
#define RAM_SIZE		        PHYS_SDRAM_1_SIZE
/* The address used to save tag list used when kernel is booting */
#define BOOT_PARAMS 	        (RAM_START_ADDR)
#define BOOT_PARAMS_LEN         0x2000

#define CONFIG_CMDLINE_TAG       1    /* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG        1

/* kernel starting address */
#define KERNEL_RAM_BASE	        CFG_LOAD_ADDR

/* Which block used to save IPL, u-boot and kernel images */
#define IPL_NAND_BLOCK      0
#define UBOOT_NAND_BLOCK    1
#define KERNEL_NAND_BLOCK   2


#define CONFIG_CMDLINE_EDITING 1

/* boot time analysis*/
#define CONFIG_BOOT_TIME_ANALYSIS			0
#define CONFIG_BOOT_TIME_ANALYSIS_USE_RTC	0

#define CONFIG_SYS_NO_FLASH 			   1

#ifdef CONFIG_MS_ISP_FLASH
#define CONFIG_CMD_SF
#ifdef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH

#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_SF_DEFAULT_BUS 0
#define CONFIG_SF_DEFAULT_CS 0
#define CONFIG_SF_DEFAULT_SPEED 60000000
#define CONFIG_SF_DEFAULT_MODE 0x0

#define CONFIG_ENV_SECT_SIZE    0x1000
#ifdef CONFIG_MS_PARTITION
#define CONFIG_ENV_OFFSET       CONFIG_MSTAR_ENV_NOR_OFFSET
#define CONFIG_MSTAR_ENV_NOR_OFFSET 0
#elif define(CONFIG_MSTAR_ENV_OFFSET)
#define CONFIG_ENV_OFFSET       CONFIG_MSTAR_ENV_NOR_OFFSET
#define CONFIG_MSTAR_ENV_NOR_OFFSET 0
#else
/* bottom 4KB of available space in Uboot */
/* 0x40000 reserved for UBoot, 0x40000 maximum storage size of uboot */
#define CONFIG_ENV_OFFSET       0x30000
#endif
#define CONFIG_ENV_OFFSET_REDUND CONFIG_MSTAR_ENV_NOR_OFFSET + CONFIG_ENV_SIZE
#endif
#endif


/*
 * File system, NAND flash, SD/MMC
 */



/*
 * ENV setting
 */
#if 1
#define ENV_SAVE_DEFAULT /* Save default ENV, if ENV is incorrect after it is readed from flash */
#endif


/*
 * FLASH driver setup
 */


#ifdef CONFIG_MS_SDMMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_MS_SDMMC_MAX_READ_BLOCKS 1024
#endif

#ifdef CONFIG_MS_NAND

/*
#ifndef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET_OOB
#endif

*/

#if defined(CONFIG_MS_SAVE_ENV_IN_NAND_FLASH)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET       CONFIG_MSTAR_ENV_NAND_OFFSET
#define CONFIG_MSTAR_ENV_NAND_OFFSET ms_nand_env_offset
#define CONFIG_ENV_SIZE         0x00020000
#endif


/* #define ENABLE_NAND_RIU_MODE 1 */

#define CONFIG_CMD_UBI
/* #define CONFIG_CMD_UBIFS */
#define CONFIG_UBI_MWRITE

#define MTDIDS_DEFAULT			"nand0=nand0"    /* "nor0=physmap-flash.0,nand0=nand" */
#define MTDPARTS_DEFAULT		"mtdparts=nand0:0xC0000@0x140000(NPT),-(UBI)"  /* must be different from real partition to test NAND partition function */

#define CONFIG_EXTRA_ENV_SETTINGS                              \
       "mtdids=" MTDIDS_DEFAULT "\0"                           \
       "mtdparts=" MTDPARTS_DEFAULT "\0"                       \
       "partition=nand0,0\0"                                   \
       ""

#define CONFIG_CMD_NAND
#define CONFIG_CMD_CIS

#ifdef CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_MAX_CHIPS	1
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0 /* not actually used */
#endif

#endif

#ifdef CONFIG_MS_EMMC
#ifndef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#define CONFIG_ENV_IS_IN_MMC       1
#define CONFIG_SYS_MMC_ENV_DEV     1
#define CONFIG_MS_EMMC_DEV_INDEX   1
#define CONFIG_EMMC_PARTITION
#define CONFIG_UNLZO_DST_ADDR  0x24000000
#define CONFIG_ENV_SIZE         0x00020000
#endif
#endif


/* CMD */

#ifndef CONFIG_CMD_FAT
#define     CONFIG_CMD_FAT
#endif

#ifndef CONFIG_DOS_PARTITION
#define     CONFIG_DOS_PARTITION
#endif

/*
 * USB configuration
 */
#ifdef CONFIG_MS_USB
#if !defined(CONFIG_USB)
    #define     CONFIG_USB
#endif
#define     CONFIG_CMD_USB
#if !defined(CONFIG_USB_STORAGE)
    #define     CONFIG_USB_STORAGE
#endif
#endif

 /* Ethernet configuration */
#ifdef CONFIG_MS_EMAC
#define CONFIG_MINIUBOOT
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_TFTP_PORT
#endif

/* I2C configuration */
#define CONFIG_SYS_I2C
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_I2C_MSTAR
#define I2C_MS_DECLARATIONS2
#define CONFIG_SYS_I2C_MS_SPEED (300000)
#define CONFIG_SYS_I2C_MS_SLAVE (0x7F)
/*
 * padmux: 1 -> PAD_HDMITX_SCL, PAD_HDMITX_SDA
 *         2 -> PAD_TTL1, PAD_TTL2
 *         3 -> PAD_TTL14, PAD_TTL15
 *         4 -> PAD_GPIO6, PAD_GPIO7
 */
#define CONFIG_SYS_I2C_0_PADMUX (0x1)
/*
 * padmux: 1 -> PAD_GPIO2, PAD_GPIO3
 *         2 -> PAD_HDMITX_SCL, PAD_HDMITX_SDA
 *         3 -> PAD_FUART_CTS, PAD_FUART_RTS
 *         4 -> PAD_TTL22, PAD_TTL23
 *         5 -> PAD_SD_CLK, PAD_SD_CMD
 */
#define CONFIG_SYS_I2C_1_PADMUX (0x1)

#define CONFIG_CMD_CACHE

/* EEPROM */
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN (2)
#define CONFIG_SYS_I2C_EEPROM_ADDR (0x54)

/*
#define ENABLE_DOUBLE_SYSTEM_CHECK  1
*/

/* SENSOR */
#define CONFIG_MS_SRCFG

#endif	/* __CONFIG_H */
