/*
 *
 * Elarix
 * src/kernel/init/main.c
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

#include <fs.h>
#include <issue.h>
#include <kbd.h>
#include <kernel.h>
#include <module.h>
#include <lock.h>
#include <mboot.h>
#include <pci.h>
#include <reboot.h>
#include <sys/time.h>

#include <asm/8259.h>
#include <asm/cpu.h>

#include <char/serial/serial.h>
#include <sound/ac97.h>
#include <sound/sb16.h>
#include <sound/pcspk.h>
#include <timer/pit.h>
#include <video/vga.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void usrmode_enter();

void kernel_main(struct mboot_info *mboot)
{
#if 1 /* XXX MOVE XXX Arch init */
	SPINLOCK(main);

	spin_lock(main);

	/* FIXME Memory map cannot be printed before vga_init() */
	mm_init(mboot->mmap_addr, mboot->mmap_len);

	ramfs_init();

	/* struct mboot_info *mboot = kmalloc(sizeof(struct mboot_info)); */
	/* memcpy(mboot, _mboot, sizeof(struct mboot_info)); */
	/* Initialize early video and debugging hardware */
	vga_init();
	serial_init(COM0);

	/* TODO Other format (UTC) */
	kprintf("Welcome to Elarix! (compiled on %s %s)\n",
			__DATE__, __TIME__);

	/* TODO Move */
	kprintf(KP_CON "Elarix has been loaded by %s\n",
			mboot->boot_loader_name);
	kprintf(KP_CON "cmdline: %s\n", mboot->cmdline);

	/* cpu_info(); */

	/* Initialize mandatory hardware */
	pic_remap();
	/* pic_disable(); */
	idt_init();
	/* tss_init(); */
	/* lapic_init(); */
	/* ioapic_init(); */
#endif

#if 1
	/* Initialize video hardware properly */
	dprintf("fb", KP_DBG "addr is %#x\n", mboot->framebuffer_addr);
	dprintf("fb", KP_DBG "bpp is %#x\n", mboot->framebuffer_bpp);
	dprintf("fb", KP_DBG "pitch is %#x\n", mboot->framebuffer_pitch);
	dprintf("fb", KP_DBG "type is %#x\n", mboot->framebuffer_type);

	/* TODO Switch */
	if (mboot->framebuffer_type == 1) {
		u64 *fb = (u64 *) (u64) mboot->framebuffer_addr;
		u32 color = ((1 << mboot->framebuffer_blue_mask_size) - 1) <<
			mboot->framebuffer_blue_field_position;
		u8 *pixel;
		u16 i, j;

		for (i = 0; i < mboot->framebuffer_width &&
				i < mboot->framebuffer_height; i++) {
			switch (mboot->framebuffer_bpp) {
			case 8:
				break;
			case 15:
			case 16:
				break;
			case 24:
				break;
			case 32:
				pixel = fb + mboot->framebuffer_pitch *
					i + 4 * i;
				*pixel = color;
				break;
			default:
				/* TODO Return error */
				break;
			}
		}

		for (j = 0; j < 1024; j++) {
			u32 v = j * (mboot->framebuffer_bpp / 8) + 2 *
				mboot->framebuffer_pitch;
			/* u32 i = j * 4 + 32 * 3200; */
			fb[v + 0] = 0 & 255;
			fb[v + 1] = 255 & 255;
			fb[v + 2] = 255 & 255;
		}
		for (;;)
			asm volatile ("hlt");
	} else {
		/* TODO Return error */
	}
#endif

	asm volatile ("sti");
	dprintf("cpu0", KP_DBG "interrupts enabled\n");
	/* TODO Actually get starting cpu */

	/* TODO Modules */
	/* acpi_init(); */
	cmos_init();
	timer_init();
	ps2kbd_init();
	/* FIXME Corrupts video memory */
	/* ide_init(); */
	ac97_init();
	sb16_init();
	pci_init();

	/* Temporary and crappy code */
#if 0
	usrmode_enter();

	for (;;)
		asm volatile ("hlt");

	/* TODO Start init */

	panic("Init was killed", 0, 0);
#else
	char cmd[64];
	u8 i, p = 0;
	/* int ipp; */

	cmd[0] = '\0';

	kprintf("\e[1;34mSV Shell:\n$ ");

	for (;;) {
		char c;
		c = getch();

		if (c == '\b') {
			if (p < 1)
				continue;

			kprintf("%c", c);

			cmd[strlen(cmd) - 1] = '\0';
			p--;

			continue;
		} else if (c != '\n') {
			char cur[2];

			cur[0] = c;
			cur[1] = '\0';
			strcat(cmd, cur);

			kprintf("%c", c);

			p++;

			continue;
		}

		kprintf("%c", c);

		/* File system */
		if (strcmp(cmd, "finit") == 0) {
			sv_mount(0, "ramfs");
		} else if (strcmp(cmd, "ls") == 0) {
			struct usr_dirent udep;
			int tmpfd = sys_open("/", 0, 0);

			int res = sys_readdir(tmpfd, &udep);
			kprintf("fd: %d, res: %d\n", tmpfd, res);
			if (res > 0) {
			/* while (sys_readdir(tmpfd, &udep)) { */
				kprintf("entry: %s\n", udep.name);
			/* } */
			}
		} else if (strcmp(cmd, "mkdir test") == 0) {
			int res2 = sys_mkdir("/test", 0);
			kprintf("res: %d", res2);

		/* Audio */
		} else if (strcmp(cmd, "beep") == 0) {
			pcspk_play(835);
			sleep(10);
			pcspk_stop();
		} else if (strcmp(cmd, "pac") == 0) {
			ac97_play();
		} else if (strcmp(cmd, "psb") == 0) {
			sb16_play();
		} else if (strcmp(cmd, "fj") == 0) {
			pcspk_fj();
		} else if (strcmp(cmd, "mi") == 0) {
			pcspk_mi();
		} else if (strcmp(cmd, "hc") == 0) {
			pcspk_hc();
		} else if (strcmp(cmd, "acri") == 0) {
			pcspk_acri();

		/* Other */
		} else if (strcmp(cmd, "reboot") == 0) {
			reboot();
		} else if (strcmp(cmd, "halt") == 0) {
			panic("halt", 0, 0);
		} else if (strcmp(cmd, "clear") == 0) {
			vga_clear();
		} else if (strcmp(cmd, "uptime") == 0) {
			kprintf("uptime: %d seconds\n", uptime());
		} else if (cmd[0] != '\0') {
			kprintf("shell: command not found: %s\n", cmd);
		}

		p = 0;

		kprintf("$ ");

		for (i = 1; i < strlen(cmd); i++)
			cmd[i] = 0;
		cmd[0] = '\0';
	} 
#endif
}

void usermode_main(void)
{
	asm volatile ("cli");
	dprintf("cpu0", KP_DBG "User mode\n");
	for (;;)
		asm volatile ("hlt");
}
