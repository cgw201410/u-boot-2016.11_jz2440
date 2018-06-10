/*
 * (C) Copyright 2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <serial.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;


/**
 * get_current() - Return pointer to currently selected serial port
 *
 * This function returns a pointer to currently selected serial port.
 * The currently selected serial port is altered by serial_assign()
 * function.
 *
 * In case this function is called before relocation or before any serial
 * port is configured, this function calls default_serial_console() to
 * determine the serial port. Otherwise, the configured serial port is
 * returned.
 *
 * Returns pointer to the currently selected serial port on success,
 * NULL on error.
 */
static struct serial_device *get_current(void)
{
	struct serial_device *dev;

	dev = default_serial_console();

	return dev;
}

/**
 * serial_init() - Initialize currently selected serial port
 *
 * This function initializes the currently selected serial port. This
 * usually involves setting up the registers of that particular port,
 * enabling clock and such. This function uses the get_current() call
 * to determine which port is selected.
 *
 * Returns 0 on success, negative on error.
 */
int serial_init(void)
{
	gd->flags |= GD_FLG_SERIAL_READY;
	return get_current()->start();
}


/**
 * serial_putc() - Output character via currently selected serial port
 * @c:	Single character to be output from the serial port.
 *
 * This function outputs a character via currently selected serial
 * port. This character is passed to the serial port driver responsible
 * for controlling the hardware. The hardware may still be in process
 * of transmitting another character, therefore this function may block
 * for a short amount of time. This function uses the get_current()
 * call to determine which port is selected.
 */
void serial_putc(const char c)
{
	get_current()->putc(c);
}


/**
 * serial_puts() - Output string via currently selected serial port
 * @s:	Zero-terminated string to be output from the serial port.
 *
 * This function outputs a zero-terminated string via currently
 * selected serial port. This function behaves as an accelerator
 * in case the hardware can queue multiple characters for transfer.
 * The whole string that is to be output is available to the function
 * implementing the hardware manipulation. Transmitting the whole
 * string may take some time, thus this function may block for some
 * amount of time. This function uses the get_current() call to
 * determine which port is selected.
 */
void serial_puts(const char *s)
{
	get_current()->puts(s);
}
