/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_S3C24X0		/* This is a SAMSUNG S3C24x0-type SoC */
#define CONFIG_S3C2440		/* specifically a SAMSUNG S3C2410 SoC */
/*#define CONFIG_JZ2440*/ 		/* on a SAMSUNG SMDK2410 Board */

#if defined(CONFIG_SPL_BUILD)
#define CONFIG_SYS_TEXT_BASE	    0
#else

#define CONFIG_SYS_BOOT_FROM_NOR_FLASH

#if defined(CONFIG_SYS_BOOT_FROM_NOR_FLASH)
#define CONFIG_SYS_TEXT_BASE	    0
#else /* CONFIG_BOOT_FROM_NAND_FLASH */
#define CONFIG_SYS_TEXT_BASE	    0x33F28000
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SYS_FLASH_QUIET_TEST
#endif
#endif

/*#define CONFIG_SKIP_LOWLEVEL_INIT_ONLY*/

#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH

/* input clock of PLL (the SMDK2410 has 12MHz input clock) */
#define CONFIG_SYS_CLK_FREQ	12000000

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Hardware drivers
 */
#define CONFIG_DRIVER_DM9000		
#define CONFIG_DM9000_BASE			0x20000000
#define DM9000_IO					0x20000000  
#define DM9000_DATA					0x20000004
/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1		1	/* we use SERIAL 1 on SMDK2410 */

/************************************************************
 * USB support (currently only works with D-cache off)
 ************************************************************/
#define CONFIG_USB_OHCI
#define CONFIG_USB_OHCI_S3C24XX
#define CONFIG_DOS_PARTITION

/************************************************************
 * RTC
 ************************************************************/
#define CONFIG_RTC_S3C24X0

#define CONFIG_BAUDRATE		115200

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BSP
#define CONFIG_CMD_DATE
#define CONFIG_CMD_NAND
#define CONFIG_CMD_REGINFO

#define CONFIG_CMDLINE_EDITING

/* autoboot */
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_ETHADDR	    08:00:3e:26:0a:5b
#define CONFIG_IPADDR		192.168.1.10
#define CONFIG_SERVERIP		192.168.1.167
#define CONFIG_BOOTARGS    	"noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0 rootfstype=jffs2"
#define CONFIG_BOOTCOMMAND	"nand read 0x307FFFC0 0xE0000 0x200000; bootm 0x307FFFC0"

#define CONFIG_EXTRA_ENV_SETTINGS               \
     "mtddevnum=0\0"                            \
	 "mtddevname=bootloader\0"                  \
	 "partition=nand0,0\0"                      \
	 "mtdids=nand0=nandflash0\0"                \
	 "mtdparts=mtdparts=nandflash0:128k@0(spl),640k(bootloader),128k(params),5m(kernel),-(root)\0"  \
     ""
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_CBSIZE	256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x30800000

/* support additional compression methods */
#define CONFIG_BZIP2
#define CONFIG_LZO
#define CONFIG_LZMA

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1          /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x04000000 /* 64 MB */

#define PHYS_FLASH_1		0x00000000 /* Flash Bank #0 */

#define CONFIG_SYS_FLASH_BASE	PHYS_FLASH_1

/* SPL */
#if defined(CONFIG_SPL_BUILD)
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE	          0
#define CONFIG_SPL_MAX_SIZE		          0x1000
#define CONFIG_SPL_STACK		          0x1000
#define CONFIG_SPL_NAND_RAW_ONLY
#define CONFIG_SPL_NAND_BOOT
#define CONFIG_SYS_NAND_U_BOOT_OFFS       0x20000
#define CONFIG_SYS_NAND_U_BOOT_DST        0x33F28000
#define CONFIG_SYS_NAND_U_BOOT_SIZE       0xA0000    /*(640 * 1024)*/
#define CONFIG_SYS_NAND_U_BOOT_START      0x33F28000
#define CONFIG_SPL_LDSCRIPT               "arch/arm/cpu/u-boot-spl.lds"
#define CONFIG_SPL_BOARD_INIT           
#define CONFIG_USE_ONLY_PRINTF

#define CONFIG_SPL_NAND_DRIVERS
/*#define CONFIG_SPL_NAND_SIMPLE*/
#if defined(CONFIG_SPL_NAND_SIMPLE)
#define CONFIG_SYS_NAND_BLOCK_SIZE        (128*1024)
#define CONFIG_SYS_NAND_PAGE_SIZE         2048
#define CONFIG_SYS_NAND_PAGE_COUNT        (CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
#define CONFIG_SYS_NAND_OOBSIZE           64
#define CONFIG_SYS_NAND_ECCPOS            {60,61,62,63}
#define CONFIG_SYS_NAND_BAD_BLOCK_POS     NAND_LARGE_BADBLOCK_POS
#else
#define CONFIG_SPL_NAND_TINY
#endif
#endif

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CFG_FLASH_CFI_WIDTH  FLASH_CFI_16BIT  /* FLASH_CFI_16BIT */
#define CONFIG_FLASH_SHOW_PROGRESS	45

#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_BANKS_LIST     { CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_SECT	(35)

/*#define CONFIG_ENV_IS_IN_FLASH*/
#define CONFIG_ENV_IS_IN_NAND

#if defined(CONFIG_ENV_IS_IN_FLASH) && defined(CONFIG_SYS_BOOT_FROM_NOR_FLASH)
#define CONFIG_ENV_ADDR			(CONFIG_SYS_FLASH_BASE + 0x0A0000)
#define CONFIG_ENV_SIZE			0x10000
#elif defined(CONFIG_ENV_IS_IN_NAND)
#define CONFIG_ENV_OFFSET       (0x20000 + 0xA0000)  /*CONFIG_SYS_NAND_U_BOOT_OFFS + CONFIG_SYS_NAND_U_BOOT_SIZE*/
#define CONFIG_ENV_SIZE			0x10000
#endif
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

#define CONFIG_SYS_MONITOR_LEN	(640 * 1024)
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_S3C2440
#define CONFIG_SYS_S3C2440_NAND_HWECC
#define CONFIG_SYS_NAND_ECCSIZE     2048
#define CONFIG_SYS_NAND_ECCBYTES    4
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x4E000000
#define CONFIG_S3C24XX_CUSTOM_NAND_TIMING
#define CONFIG_S3C24XX_TACLS		1
#define CONFIG_S3C24XX_TWRPH0		5
#define CONFIG_S3C24XX_TWRPH1		3
#endif

/*
 * File system
 */
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_YAFFS2
#define CONFIG_RBTREE
#define CONFIG_JFFS2_NAND

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#endif /* __CONFIG_H */
