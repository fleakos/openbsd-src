/*	$OpenBSD: conf.c,v 1.8 2021/05/14 06:48:52 jsg Exp $	*/

/*
 * Copyright (c) 1994, 1995 Charles M. Hannum.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Charles Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/tty.h>

#include <machine/conf.h>

#include "sd.h"
#include "cd.h"
#include "uk.h"
#include "vnd.h"
#include "rd.h"

struct bdevsw	bdevsw[] =
{
	/* XXX no riscv machine will have this but we are using arm64 as a template */
	bdev_notdef(),			/* */
	bdev_swap_init(1,sw),		/* 1: swap pseudo-device */
	bdev_notdef(),			/* 2: was floppy diskette */
	bdev_notdef(),			/* 3 */
	bdev_disk_init(NSD,sd),		/* 4: SCSI disk */
	bdev_notdef(),			/* 5: was: SCSI tape */
	bdev_disk_init(NCD,cd),		/* 6: SCSI CD-ROM */
	bdev_notdef(),			/* 7 */
	bdev_disk_init(NRD,rd),		/* 8: ram disk driver */
	bdev_notdef(),			/* 9 */
	bdev_notdef(),			/* 10 */
	bdev_notdef(),			/* 11 */
	bdev_notdef(),			/* 12 */
	bdev_notdef(),			/* 13 */
	bdev_disk_init(NVND,vnd),	/* 14: vnode disk driver */
	bdev_notdef(),			/* 15: was: Sony CD-ROM */
	bdev_notdef(),			/* 16: was: concatenated disk driver */
	bdev_notdef(),			/* 17 */
	bdev_notdef(),			/* 18 */
};
int	nblkdev = nitems(bdevsw);

#define	mmread	mmrw
#define	mmwrite	mmrw
cdev_decl(mm);
#include "bio.h"
#include "pty.h"
#include "com.h"
cdev_decl(com);
#include "lpt.h"
cdev_decl(lpt);
#include "ch.h"
#include "bpfilter.h"
#include "tun.h"
#include "audio.h"
#include "video.h"
#include "midi.h"
#include "ksyms.h"
#include "kstat.h"
#include "radio.h"

#ifdef USER_PCICONF
#include "pci.h"
cdev_decl(pci);
#endif

#include "dt.h"
#include "pf.h"
#include "hotplug.h"
#include "vscsi.h"
#include "pppx.h"
#include "fuse.h"
#include "openprom.h"
#include "ipmi.h"
#include "switch.h"

struct cdevsw	cdevsw[] =
{
	cdev_cn_init(1,cn),		/* 0: virtual console */
	cdev_ctty_init(1,ctty),		/* 1: controlling terminal */
	cdev_mm_init(1,mm),		/* 2: /dev/{null,mem,kmem,...} */
	cdev_notdef(),			/* 3: */
	cdev_notdef(),			/* 4 was /dev/drum */
	cdev_tty_init(NPTY,pts),	/* 5: pseudo-tty slave */
	cdev_ptc_init(NPTY,ptc),	/* 6: pseudo-tty master */
	cdev_log_init(1,log),		/* 7: /dev/klog */
	cdev_tty_init(NCOM,com),	/* 8: serial port */
	cdev_notdef(),			/* 9: was floppy disk */
	cdev_notdef(),			/* 10 */
	cdev_notdef(),			/* 11: Sony CD-ROM */
	cdev_notdef(),			/* 12: frame buffers, etc. */
	cdev_disk_init(NSD,sd),		/* 13: SCSI disk */
	cdev_notdef(),			/* 14: was: SCSI tape */
	cdev_disk_init(NCD,cd),		/* 15: SCSI CD-ROM */
	cdev_lpt_init(NLPT,lpt),	/* 16: parallel printer */
	cdev_ch_init(NCH,ch),		/* 17: SCSI autochanger */
	cdev_notdef(),			/* 18: was: concatenated disk driver */
	cdev_notdef(),			/* 19 */
	cdev_uk_init(NUK,uk),		/* 20: unknown SCSI */
	cdev_notdef(),			/* 21 */
	cdev_fd_init(1,filedesc),	/* 22: file descriptor pseudo-device */
	cdev_bpf_init(NBPFILTER,bpf),	/* 23: Berkeley packet filter */
	cdev_notdef(),			/* 24 */
	cdev_notdef(),			/* 25 */
	cdev_notdef(),			/* 26 */
	cdev_notdef(),			/* 27 */
	cdev_notdef(),			/* 28 was LKM */
	cdev_notdef(),			/* 29 */
	cdev_dt_init(NDT,dt),		/* 30: dynamic tracer */
	cdev_notdef(),			/* 31 */
	cdev_notdef(),			/* 32 */
	cdev_notdef(),			/* 33 */
	cdev_notdef(),			/* 34 */
	cdev_notdef(),			/* 35: Microsoft mouse */
	cdev_notdef(),			/* 36: Logitech mouse */
	cdev_notdef(),			/* 37: Extended PS/2 mouse */
	cdev_notdef(),			/* 38: was: Cyclom serial port */
	cdev_notdef(),			/* 39: Mitsumi CD-ROM */
	cdev_tun_init(NTUN,tun),	/* 40: network tunnel */
	cdev_disk_init(NVND,vnd),	/* 41: vnode disk driver */
	cdev_audio_init(NAUDIO,audio),	/* 42: generic audio I/O */
	cdev_notdef(),			/* 43 */
	cdev_video_init(NVIDEO,video),	/* 44: generic video I/O */
	cdev_random_init(1,random),	/* 45: random data source */
	cdev_notdef(),			/* 46 */
	cdev_disk_init(NRD,rd),		/* 47: ram disk driver */
	cdev_notdef(),			/* 48 */
	cdev_notdef(),			/* 49: Bt848 video capture device */
	cdev_ksyms_init(NKSYMS,ksyms),	/* 50: Kernel symbols device */
	cdev_kstat_init(NKSTAT,kstat),	/* 51: kernel statistics */
	cdev_midi_init(NMIDI,midi),	/* 52: MIDI I/O */
	cdev_notdef(),			/* 53 was: sequencer I/O */
	cdev_notdef(),			/* 54 was: RAIDframe disk driver */
	cdev_notdef(),			/* 55: */
	/* The following slots are reserved for isdn4bsd. */
	cdev_notdef(),			/* 56: i4b main device */
	cdev_notdef(),			/* 57: i4b control device */
	cdev_notdef(),			/* 58: i4b raw b-channel access */
	cdev_notdef(),			/* 59: i4b trace device */
	cdev_notdef(),			/* 60: i4b phone device */
	/* End of reserved slots for isdn4bsd. */
	cdev_notdef(),			/* 61: USB controller */
	cdev_notdef(),			/* 62: USB generic HID */
	cdev_notdef(),			/* 63: USB generic driver */
	cdev_notdef(),			/* 64: USB printers */
	cdev_notdef(),			/* 65: urio */
	cdev_notdef(),			/* 66: USB tty */
	cdev_notdef(),			/* 67: keyboards */
	cdev_notdef(),			/* 68: mice */
	cdev_notdef(),			/* 69: ws multiplexor */
	cdev_openprom_init(NOPENPROM,openprom),	/* 70: /dev/openprom */
	cdev_notdef(),			/* 71: was: Cyclades-Z serial port */
#ifdef USER_PCICONF
	cdev_pci_init(NPCI,pci),	/* 72: PCI user */
#else
	cdev_notdef(),
#endif
	cdev_pf_init(NPF,pf),		/* 73: packet filter */
	cdev_notdef(),			/* 74: ALTQ (deprecated) */
	cdev_notdef(),
	cdev_radio_init(NRADIO, radio), /* 76: generic radio I/O */
	cdev_notdef(),			/* 77: was USB scanners */
	cdev_notdef(),			/* 78: was: system call tracing */
	cdev_bio_init(NBIO,bio),	/* 79: ioctl tunnel */
	cdev_notdef(),			/* 80 */
	cdev_ptm_init(NPTY,ptm),	/* 81: pseudo-tty ptm device */
	cdev_hotplug_init(NHOTPLUG,hotplug), /* 82: devices hot plugging */
	cdev_notdef(),			/* 83: apm */
	cdev_notdef(),			/* 84 */
	cdev_notdef(),			/* 85 */
	cdev_notdef(),			/* 86 */
	cdev_notdef(),			/* 87: drm */
	cdev_notdef(),			/* 88: GPIO interface */
	cdev_vscsi_init(NVSCSI,vscsi),	/* 89: vscsi */
	cdev_disk_init(1,diskmap),	/* 90: disk mapper */
	cdev_pppx_init(NPPPX,pppx),	/* 91: pppx */
	cdev_fuse_init(NFUSE,fuse),	/* 92: fuse */
	cdev_tun_init(NTUN,tap),	/* 93: Ethernet network tunnel */
	cdev_notdef(),			/* 94 */
	cdev_notdef(),			/* 95 */
	cdev_ipmi_init(NIPMI,ipmi),	/* 96: ipmi */
	cdev_switch_init(NSWITCH,switch), /* 97: switch(4) control interface */
	cdev_notdef(),			/* 98: FIDO/U2F security key */
	cdev_pppx_init(NPPPX,pppac),	/* 99: PPP Access Concentrator */
	cdev_notdef(),			/* 100: USB joystick/gamecontroller */
};
int	nchrdev = nitems(cdevsw);

/*
 * Swapdev is a fake device implemented
 * in sw.c used only internally to get to swstrategy.
 * It cannot be provided to the users, because the
 * swstrategy routine munches the b_dev and b_blkno entries
 * before calling the appropriate driver.  This would horribly
 * confuse, e.g. the hashing routines. Instead, /dev/drum is
 * provided as a character (raw) device.
 */
dev_t	swapdev = makedev(BMAJ_SW, 0);

/*
 * Returns true if dev is /dev/mem or /dev/kmem.
 */
int
iskmemdev(dev_t dev)
{

	return (major(dev) == CMAJ_MM && (minor(dev) < 2 || minor(dev) == 14));
}

/*
 * Returns true if dev is /dev/zero.
 */
int
iszerodev(dev_t dev)
{

	return (major(dev) == CMAJ_MM && minor(dev) == 12);
}

dev_t
getnulldev(void)
{
	return makedev(CMAJ_MM, 2);
}

int chrtoblktbl[] = {
	/*VCHR*/	/*VBLK*/
	/*  0 */	NODEV,
	/*  1 */	NODEV,
	/*  2 */	NODEV,
	/*  3 */	NODEV,
	/*  4 */	NODEV,
	/*  5 */	NODEV,
	/*  6 */	NODEV,
	/*  7 */	NODEV,
	/*  8 */	NODEV,
	/*  9 */	NODEV,		/* was fd */
	/* 10 */	NODEV,
	/* 11 */	NODEV,
	/* 12 */	NODEV,
	/* 13 */	4,		/* sd */
	/* 14 */	NODEV,
	/* 15 */	6,		/* cd */
	/* 16 */	NODEV,
	/* 17 */	NODEV,
	/* 18 */	NODEV,
	/* 19 */	NODEV,
	/* 20 */	NODEV,
	/* 21 */	NODEV,
	/* 22 */	NODEV,
	/* 23 */	NODEV,
	/* 24 */	NODEV,
	/* 25 */	NODEV,
	/* 26 */	NODEV,
	/* 27 */	NODEV,
	/* 28 */	NODEV,
	/* 29 */	NODEV,
	/* 30 */	NODEV,
	/* 31 */	NODEV,
	/* 32 */	NODEV,
	/* 33 */	NODEV,
	/* 34 */	NODEV,
	/* 35 */	NODEV,
	/* 36 */	NODEV,
	/* 37 */	NODEV,
	/* 38 */	NODEV,
	/* 39 */	NODEV,
	/* 40 */	NODEV,
	/* 41 */	14,		/* vnd */
	/* 42 */	NODEV,
	/* 43 */	NODEV,
	/* 44 */	NODEV,
	/* 45 */	NODEV,
	/* 46 */	NODEV,
	/* 47 */	8,		/* rd */
};

int nchrtoblktbl = nitems(chrtoblktbl);

/*
 * In order to map BSD bdev numbers of disks to their BIOS equivalents
 * we use several heuristics, one being using checksums of the first
 * few blocks of a disk to get a signature we can match with /boot's
 * computed signatures.  To know where from to read, we must provide a
 * disk driver name -> bdev major number table, which follows.
 * Note: floppies are not included as those are differentiated by the BIOS.
 */
int findblkmajor(struct device *dv);
dev_t dev_rawpart(struct device *);	/* XXX */

dev_t
dev_rawpart(struct device *dv)
{
	int majdev;

	majdev = findblkmajor(dv);

	switch (majdev) {
	/* add here any device you want to be checksummed on boot */
	case BMAJ_WD:
	case BMAJ_SD:
		return (MAKEDISKDEV(majdev, dv->dv_unit, RAW_PART));
		break;
	default:
		;
	}

	return (NODEV);
}
