/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Aneesh V <aneesh@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <spl.h>
#include <asm/u-boot.h>
#include <nand.h>
#include <fat.h>
#include <version.h>
#include <image.h>
#include <dm/root.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_UBOOT_START
#define CONFIG_SYS_UBOOT_START      CONFIG_SYS_NAND_U_BOOT_START	
#endif
#ifndef CONFIG_SYS_MONITOR_LEN
/* Unknown U-Boot size, let's assume it will not be more than 200 KB */
#define CONFIG_SYS_MONITOR_LEN	(200 * 1024)
#endif

/* Define board data structure */
static bd_t bdata __attribute__ ((section(".data")));


void spl_set_header_raw_uboot(struct spl_image_info *spl_image)
{
	spl_image->size = CONFIG_SYS_MONITOR_LEN;
	spl_image->entry_point = CONFIG_SYS_UBOOT_START;
	spl_image->load_addr = CONFIG_SYS_UBOOT_START;
	spl_image->os = IH_OS_U_BOOT;
	spl_image->name = "U-Boot";
}

int spl_parse_image_header(struct spl_image_info *spl_image,
			   const struct image_header *header)
{
	spl_set_header_raw_uboot(spl_image);

	return 0;
}

__weak void __noreturn jump_to_image_no_args(struct spl_image_info *spl_image)
{
	typedef void __noreturn (*image_entry_noargs_t)(void);

	image_entry_noargs_t image_entry =
		(image_entry_noargs_t)(unsigned long)spl_image->entry_point;

	puts("image entry point: ");
	if (spl_image->entry_point == CONFIG_SYS_NAND_U_BOOT_START) {
        puts("0x30800000\n");	
		puts("          \n");
		putc('\n');
	}
	image_entry();
}


int spl_init(void)
{

	//puts("spl_init()\n");

	gd->flags |= GD_FLG_SPL_INIT;

	return 0;
}

#ifndef BOOT_DEVICE_NONE
#define BOOT_DEVICE_NONE 0xdeadbeef
#endif

__weak void board_boot_order(u32 *spl_boot_list)
{
	spl_boot_list[0] = spl_boot_device();
}


#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
struct boot_device_name {
	u32 boot_dev;
	const char *name;
};

struct boot_device_name boot_name_table[] = {
#ifdef CONFIG_SPL_NAND_SUPPORT
	{ BOOT_DEVICE_NAND, "NAND" },
#endif
	/* Keep this entry last */
	{ BOOT_DEVICE_NONE, "unknown boot device" },
};

static void announce_boot_device(u32 boot_device)
{
	int i;

	puts("Trying to boot from ");

	for (i = 0; i < ARRAY_SIZE(boot_name_table) - 1; i++) {
		if (boot_name_table[i].boot_dev == boot_device) {
		    puts("NAND\n");
			break;
		}
	}

	//printf("%s\n", boot_name_table[i].name);
}
#endif

static struct spl_image_loader *spl_ll_find_loader(uint boot_device)
{
	struct spl_image_loader *drv =
		ll_entry_start(struct spl_image_loader, spl_image_loader);
	const int n_ents =
		ll_entry_count(struct spl_image_loader, spl_image_loader);
	struct spl_image_loader *entry;

	for (entry = drv; entry != drv + n_ents; entry++) {
		if (boot_device == entry->boot_device)
			return entry;
	}

	/* Not found */
	return NULL;
}

static int spl_load_image(struct spl_image_info *spl_image, u32 boot_device)
{
	struct spl_boot_device bootdev;
	struct spl_image_loader *loader = spl_ll_find_loader(boot_device);

	bootdev.boot_device = boot_device;
	bootdev.boot_device_name = NULL;
	if (loader)
		return loader->load_image(spl_image, &bootdev);

	puts("SPL: Unsupported Boot Device!\n");

	return -ENODEV;
}

void board_init_r(gd_t *dummy1, ulong dummy2)
{
	u32 spl_boot_list[] = {
		BOOT_DEVICE_NONE,
		BOOT_DEVICE_NONE,
		BOOT_DEVICE_NONE,
		BOOT_DEVICE_NONE,
		BOOT_DEVICE_NONE,
	};
	struct spl_image_info spl_image;
	int i;

	if (!(gd->flags & GD_FLG_SPL_INIT)) {
		if (spl_init())
			hang();
	}

	/*
	 * timer_init() does not exist on PPC systems. The timer is initialized
	 * and enabled (decrementer) in interrupt_init() here.
	 */
	//timer_init();

#ifdef CONFIG_SPL_BOARD_INIT
	spl_board_init();
#endif

	memset(&spl_image, '\0', sizeof(spl_image));
	board_boot_order(spl_boot_list);
	for (i = 0; i < ARRAY_SIZE(spl_boot_list) &&
			spl_boot_list[i] != BOOT_DEVICE_NONE; i++) {
		announce_boot_device(spl_boot_list[i]);
		if (!spl_load_image(&spl_image, spl_boot_list[i]))
			break;
	}

	if (i == ARRAY_SIZE(spl_boot_list) ||
	    spl_boot_list[i] == BOOT_DEVICE_NONE) {
		puts("SPL: failed to boot from all boot devices\n");
		hang();
	}

	switch (spl_image.os) {
	case IH_OS_U_BOOT:
		//puts("Jumping to U-Boot\n");
		break;
	default:
	    puts("Unsupported OS image.. Jumping nevertheless..\n");
	}

	puts("loaded - jumping to U-Boot...\n");
	jump_to_image_no_args(&spl_image);
}

/*
 * This requires UART clocks to be enabled.  In order for this to work the
 * caller must ensure that the gd pointer is valid.
 */
void preloader_console_init(void)
{
	gd->bd = &bdata;
	gd->baudrate = CONFIG_BAUDRATE;

	serial_init();		/* serial communications setup */

	puts(" \nU-Boot SPL " PLAIN_VERSION " (" U_BOOT_DATE " - " \
			U_BOOT_TIME ")\n");
}

/**
 * spl_relocate_stack_gd() - Relocate stack ready for board_init_r() execution
 *
 * Sometimes board_init_f() runs with a stack in SRAM but we want to use SDRAM
 * for the main board_init_r() execution. This is typically because we need
 * more stack space for things like the MMC sub-system.
 *
 * This function calculates the stack position, copies the global_data into
 * place, sets the new gd (except for ARM, for which setting GD within a C
 * function may not always work) and returns the new stack position. The
 * caller is responsible for setting up the sp register and, in the case
 * of ARM, setting up gd.
 *
 * All of this is done using the same layout and alignments as done in
 * board_init_f_init_reserve() / board_init_f_alloc_reserve().
 *
 * @return new stack location, or 0 to use the same stack
 */
ulong spl_relocate_stack_gd(void)
{
#ifdef CONFIG_SPL_STACK_R
	gd_t *new_gd;
	ulong ptr = CONFIG_SPL_STACK_R_ADDR;

#ifdef CONFIG_SPL_SYS_MALLOC_SIMPLE
	if (CONFIG_SPL_STACK_R_MALLOC_SIMPLE_LEN) {
		ptr -= CONFIG_SPL_STACK_R_MALLOC_SIMPLE_LEN;
		gd->malloc_base = ptr;
		gd->malloc_limit = CONFIG_SPL_STACK_R_MALLOC_SIMPLE_LEN;
		gd->malloc_ptr = 0;
	}
#endif
	/* Get stack position: use 8-byte alignment for ABI compliance */
	ptr = CONFIG_SPL_STACK_R_ADDR - roundup(sizeof(gd_t),16);
	new_gd = (gd_t *)ptr;
	memcpy(new_gd, (void *)gd, sizeof(gd_t));
#if !defined(CONFIG_ARM)
	gd = new_gd;
#endif
	return ptr;
#else
	return 0;
#endif
}
