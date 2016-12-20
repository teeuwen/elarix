/*
 *
 * Elara
 * src/kernel/dev/char/tty.c
 *
 * Copyright (C) 2016 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#include <asm/cpu.h>

#include <kernel.h>

static const char devname[] = "tty";

static int tty_open(struct inode *ip, struct file *)
{

}

static int tty_close(struct inode *ip, struct file *fp)
{

}

static int tty_read(struct file *fp, void *buf, off_t off, size_t n)
{

}

static int tty_write(struct file *fp, const void *buf, off_t off, size_t n)
{

}

static struct file_ops tty_file_ops = {
	.open	= &tty_open,
	.close	= &tty_close,
	.read	= &tty_read,
	.write	= &tty_write
};

static struct fs_dev = {
	.name	= devname,

	.op	= &tty_file_ops
};

void tty_init(void)
{
	fs_chrdev_reg(0, &fs_dev);
}

void tty_exit(void)
{
	/* TODO */

	fs_chrdev_unreg(0, name);
}