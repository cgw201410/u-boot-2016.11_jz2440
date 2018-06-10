#include <common.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <nand.h>
#include <linux/mtd/nand_ecc.h>


u32 spl_boot_device(void)
{
	return BOOT_DEVICE_NAND;
}

#ifdef CONFIG_SPL_BOARD_INIT
void spl_board_init(void)
{
	preloader_console_init();
}
#endif


void board_init_f(ulong dummy)
{
    board_early_init_f();
}
