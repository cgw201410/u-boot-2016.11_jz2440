/*
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/compiler.h>
#include <asm/arch/s3c24x0_cpu.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SERIAL1
#define UART_NR	S3C24X0_UART0

#elif defined(CONFIG_SERIAL2)
#define UART_NR	S3C24X0_UART1

#elif defined(CONFIG_SERIAL3)
#define UART_NR	S3C24X0_UART2

#else
#error "Bad: you didn't configure serial ..."
#endif

#include <asm/io.h>
#include <serial.h>

/* Multi serial device functions */
#define DECLARE_S3C_SERIAL0_FUNCTIONS() \
	int s3serial0_init(void) \
	{ \
		return serial0_init_dev(); \
	} \
	void s3serial0_setbrg(void) \
	{ \
		serial0_setbrg_dev(); \
	} \
	void s3serial0_putc(const char c) \
	{ \
		serial0_putc_dev(c); \
	} \
	void s3serial0_puts(const char *s) \
	{ \
		serial0_puts_dev(s); \
	}

#define INIT_S3C_SERIAL0_STRUCTURE(__name) {	\
	.name	= __name,				\
	.start	= s3serial0_init,		\
	.stop	= NULL,					\
	.setbrg	= s3serial0_setbrg,		\
	.putc	= s3serial0_putc,		\
	.puts	= s3serial0_puts,		\
}

static void _serial0_setbrg(void)
{
	struct s3c24x0_uart *uart = s3c24x0_get_base_uart(S3C24X0_UART0);
	unsigned int reg = 0;
	int i;

	/* value is calculated so : (int)(PCLK/16./baudrate) -1 */
	reg = get_PCLK() / (16 * gd->baudrate) - 1;

	writel(reg, &uart->ubrdiv);
	for (i = 0; i < 100; i++)
		/* Delay */ ;
}

static inline void serial0_setbrg_dev(void)
{
	_serial0_setbrg();
}

/* Initialise the serial port. The settings are always 8 data bits, no parity,
 * 1 stop bit, no start bits.
 */
static int serial0_init_dev(void)
{
	struct s3c24x0_uart *uart = s3c24x0_get_base_uart(S3C24X0_UART0);

	/* FIFO enable, Tx/Rx FIFO clear */
	writel(0x07, &uart->ufcon);
	writel(0x0, &uart->umcon);

	/* Normal,No parity,1 stop,8 bit */
	writel(0x3, &uart->ulcon);
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	writel(0x245, &uart->ucon);

	_serial0_setbrg();

	return (0);
}

/*
 * Output a single byte to the serial port.
 */
static void _serial0_putc(const char c)
{
	struct s3c24x0_uart *uart = s3c24x0_get_base_uart(S3C24X0_UART0);

	/* If \n, also do \r */
	if (c == '\n')
		serial_putc('\r');

	while (!(readl(&uart->utrstat) & 0x2))
		/* wait for room in the tx FIFO */ ;

	writeb(c, &uart->utxh);
}

static inline void serial0_putc_dev(const char c)
{
	_serial0_putc(c);
}

static void _serial0_puts(const char *s)
{
	while (*s) {
		_serial0_putc(*s++);
	}
}

static inline void serial0_puts_dev(const char *s)
{
	_serial0_puts(s);
}

DECLARE_S3C_SERIAL0_FUNCTIONS();
struct serial_device s3c24xx_serial0_device =
INIT_S3C_SERIAL0_STRUCTURE("s3ser0");

__weak struct serial_device *default_serial_console(void)
{
#if defined(CONFIG_SERIAL1)
	return &s3c24xx_serial0_device;
#else
#error "CONFIG_SERIAL? missing."
#endif
}
