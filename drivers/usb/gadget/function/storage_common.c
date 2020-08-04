/*
 * storage_common.c -- Common definitions for mass storage functionality
 *
 * Copyright (C) 2003-2008 Alan Stern
 * Copyeight (C) 2009 Samsung Electronics
 * Author: Michal Nazarewicz (mina86@mina86.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * This file requires the following identifiers used in USB strings to
 * be defined (each of type pointer to char):
 *  - fsg_string_interface    -- name of the interface
 */

/*
 * When USB_GADGET_DEBUG_FILES is defined the module param num_buffers
 * sets the number of pipeline buffers (length of the fsg_buffhd array).
 * The valid range of num_buffers is: num >= 2 && num <= 4.
 */

#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/usb/composite.h>

#include "storage_common.h"

#ifdef CONFIG_USBIF_COMPLIANCE
static struct usb_otg_descriptor
fsg_otg_desc = {
	.bLength = sizeof(fsg_otg_desc),
	.bDescriptorType = USB_DT_OTG,
	/* OTG 2.0: */
	.bmAttributes =	USB_OTG_SRP | USB_OTG_HNP,
	.bcdOTG = cpu_to_le16(0x200),
};
#endif

/* There is only one interface. */

struct usb_interface_descriptor fsg_intf_desc = {
	.bLength =		sizeof fsg_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,

	.bNumEndpoints =	2,		/* Adjusted during fsg_bind() */
	.bInterfaceClass =	USB_CLASS_MASS_STORAGE,
	.bInterfaceSubClass =	USB_SC_SCSI,	/* Adjusted during fsg_bind() */
	.bInterfaceProtocol =	USB_PR_BULK,	/* Adjusted during fsg_bind() */
	.iInterface =		FSG_STRING_INTERFACE,
};
EXPORT_SYMBOL_GPL(fsg_intf_desc);

/*
 * Three full-speed endpoint descriptors: bulk-in, bulk-out, and
 * interrupt-in.
 */

struct usb_endpoint_descriptor fsg_fs_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	/* wMaxPacketSize set by autoconfiguration */
};
EXPORT_SYMBOL_GPL(fsg_fs_bulk_in_desc);

struct usb_endpoint_descriptor fsg_fs_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	/* wMaxPacketSize set by autoconfiguration */
};
EXPORT_SYMBOL_GPL(fsg_fs_bulk_out_desc);

struct usb_descriptor_header *fsg_fs_function[] = {
#ifdef CONFIG_USBIF_COMPLIANCE
	(struct usb_descriptor_header *) &fsg_otg_desc,
#endif
	(struct usb_descriptor_header *) &fsg_intf_desc,
	(struct usb_descriptor_header *) &fsg_fs_bulk_in_desc,
	(struct usb_descriptor_header *) &fsg_fs_bulk_out_desc,
	NULL,
};
EXPORT_SYMBOL_GPL(fsg_fs_function);


/*
 * USB 2.0 devices need to expose both high speed and full speed
 * descriptors, unless they only run at full speed.
 *
 * That means alternate endpoint descriptors (bigger packets)
 * and a "device qualifier" ... plus more construction options
 * for the configuration descriptor.
 */
struct usb_endpoint_descriptor fsg_hs_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_bulk_in_desc during fsg_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};
EXPORT_SYMBOL_GPL(fsg_hs_bulk_in_desc);

struct usb_endpoint_descriptor fsg_hs_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_bulk_out_desc during fsg_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
	.bInterval =		1,	/* NAK every 1 uframe */
};
EXPORT_SYMBOL_GPL(fsg_hs_bulk_out_desc);


struct usb_descriptor_header *fsg_hs_function[] = {
#ifdef CONFIG_USBIF_COMPLIANCE
	(struct usb_descriptor_header *) &fsg_otg_desc,
#endif
	(struct usb_descriptor_header *) &fsg_intf_desc,
	(struct usb_descriptor_header *) &fsg_hs_bulk_in_desc,
	(struct usb_descriptor_header *) &fsg_hs_bulk_out_desc,
	NULL,
};
EXPORT_SYMBOL_GPL(fsg_hs_function);

struct usb_endpoint_descriptor fsg_ss_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_bulk_in_desc during fsg_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(1024),
};
EXPORT_SYMBOL_GPL(fsg_ss_bulk_in_desc);

struct usb_ss_ep_comp_descriptor fsg_ss_bulk_in_comp_desc = {
	.bLength =		sizeof(fsg_ss_bulk_in_comp_desc),
	.bDescriptorType =	USB_DT_SS_ENDPOINT_COMP,

	/*.bMaxBurst =		DYNAMIC, */
};
EXPORT_SYMBOL_GPL(fsg_ss_bulk_in_comp_desc);

struct usb_endpoint_descriptor fsg_ss_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_bulk_out_desc during fsg_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(1024),
};
EXPORT_SYMBOL_GPL(fsg_ss_bulk_out_desc);

struct usb_ss_ep_comp_descriptor fsg_ss_bulk_out_comp_desc = {
	.bLength =		sizeof(fsg_ss_bulk_in_comp_desc),
	.bDescriptorType =	USB_DT_SS_ENDPOINT_COMP,

	/*.bMaxBurst =		DYNAMIC, */
};
EXPORT_SYMBOL_GPL(fsg_ss_bulk_out_comp_desc);

struct usb_descriptor_header *fsg_ss_function[] = {
#ifdef CONFIG_USBIF_COMPLIANCE
	(struct usb_descriptor_header *) &fsg_otg_desc,
#endif
	(struct usb_descriptor_header *) &fsg_intf_desc,
	(struct usb_descriptor_header *) &fsg_ss_bulk_in_desc,
	(struct usb_descriptor_header *) &fsg_ss_bulk_in_comp_desc,
	(struct usb_descriptor_header *) &fsg_ss_bulk_out_desc,
	(struct usb_descriptor_header *) &fsg_ss_bulk_out_comp_desc,
	NULL,
};
EXPORT_SYMBOL_GPL(fsg_ss_function);


 /*-------------------------------------------------------------------------*/

/*
 * If the next two routines are called while the gadget is registered,
 * the caller must own fsg->filesem for writing.
 */

void fsg_lun_close(struct fsg_lun *curlun)
{
	if (curlun->filp) {
		LDBG(curlun, "close backing file\n");
		fput(curlun->filp);
		curlun->filp = NULL;
	}
}
EXPORT_SYMBOL_GPL(fsg_lun_close);

int fsg_lun_open(struct fsg_lun *curlun, const char *filename)
{
	int				ro;
	struct file			*filp = NULL;
	int				rc = -EINVAL;
	struct inode			*inode = NULL;
	loff_t				size;
	loff_t				num_sectors;
	loff_t				min_sectors;
	unsigned int			blkbits;
	unsigned int			blksize;

	/* R/W if we can, R/O if we must */
	ro = curlun->initially_ro;
	if (!ro) {
		filp = filp_open(filename, O_RDWR | O_LARGEFILE, 0);
		if (PTR_ERR(filp) == -EROFS || PTR_ERR(filp) == -EACCES)
			ro = 1;
	}
	if (ro)
		filp = filp_open(filename, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(filp)) {
		LINFO(curlun, "unable to open backing file: %s\n", filename);
		return PTR_ERR(filp);
	}

	if (!(filp->f_mode & FMODE_WRITE))
		ro = 1;

	inode = file_inode(filp);
	if ((!S_ISREG(inode->i_mode) && !S_ISBLK(inode->i_mode))) {
		LINFO(curlun, "invalid file type: %s\n", filename);
		goto out;
	}

	/*
	 * If we can't read the file, it's no good.
	 * If we can't write the file, use it read-only.
	 */
	if (!(filp->f_mode & FMODE_CAN_READ)) {
		LINFO(curlun, "file not readable: %s\n", filename);
		goto out;
	}
	if (!(filp->f_mode & FMODE_CAN_WRITE))
		ro = 1;

	size = i_size_read(inode->i_mapping->host);
	if (size < 0) {
		LINFO(curlun, "unable to find file size: %s\n", filename);
		rc = (int) size;
		goto out;
	}

	if (curlun->cdrom) {
		blksize = 2048;
		blkbits = 11;
	} else if (inode->i_bdev) {
		blksize = bdev_logical_block_size(inode->i_bdev);
		blkbits = blksize_bits(blksize);
	} else {
		blksize = 512;
		blkbits = 9;
	}

	num_sectors = size >> blkbits; /* File size in logic-block-size blocks */
	min_sectors = 1;
	if (curlun->cdrom) {
		min_sectors = 300;	/* Smallest track is 300 frames */
		if (num_sectors >= 256*60*75) {
			num_sectors = 256*60*75 - 1;
			LINFO(curlun, "file too big: %s\n", filename);
			LINFO(curlun, "using only first %d blocks\n",
					(int) num_sectors);
		}
	}
	if (num_sectors < min_sectors) {
		LINFO(curlun, "file too small: %s\n", filename);
		rc = -ETOOSMALL;
		goto out;
	}

	if (fsg_lun_is_open(curlun))
		fsg_lun_close(curlun);

	curlun->blksize = blksize;
	curlun->blkbits = blkbits;
	curlun->ro = ro;
	curlun->filp = filp;
	curlun->file_length = size;
	curlun->num_sectors = num_sectors;
	LDBG(curlun, "open backing file: %s\n", filename);
	return 0;

out:
	fput(filp);
	return rc;
}
EXPORT_SYMBOL_GPL(fsg_lun_open);


/*-------------------------------------------------------------------------*/

/*
 * Sync the file data, don't bother with the metadata.
 * This code was copied from fs/buffer.c:sys_fdatasync().
 */
int fsg_lun_fsync_sub(struct fsg_lun *curlun)
{
	struct file	*filp = curlun->filp;

	if (curlun->ro || !filp)
		return 0;
	return vfs_fsync(filp, 1);
}
EXPORT_SYMBOL_GPL(fsg_lun_fsync_sub);

void store_cdrom_address(u8 *dest, int msf, u32 addr)
{
	if (msf) {
		/* Convert to Minutes-Seconds-Frames */
		addr >>= 2;		/* Convert to 2048-byte frames */
		addr += 2*75;		/* Lead-in occupies 2 seconds */
		dest[3] = addr % 75;	/* Frames */
		addr /= 75;
		dest[2] = addr % 60;	/* Seconds */
		addr /= 60;
		dest[1] = addr;		/* Minutes */
		dest[0] = 0;		/* Reserved */
	} else {
		/* Absolute sector */
		put_unaligned_be32(addr, dest);
	}
}
EXPORT_SYMBOL_GPL(store_cdrom_address);

//zj add for mac ++
/**
 * fsg_get_toc() - Builds a TOC with required format @format.
 * @curlun: The LUN for which the TOC has to be built
 * @msf: Min Sec Frame format or LBA format for address
 * @format: TOC format code
 * @buf: The buffer into which the TOC is built
 *
 * Builds a Table of Content which can be used as data for READ_TOC command.
 * The TOC simulates a single session, single track CD-ROM mode 1 disc.
 *
 * Returns number of bytes written to @buf, -EINVAL if format not supported.
 */
int fsg_get_toc(struct fsg_lun *curlun, int msf, int format, u8 *buf)
{
	int i, len;
	switch (format) {

	case 0:
		/* Formatted TOC */
		len = 4 + 2*8;                /* 4 byte header + 2 descriptors */
		memset(buf, 0, len);
		buf[1] = len - 2;        /* TOC Length excludes length field */

		buf[2] = 1;                /* First track number */
		buf[3] = 1;                /* Last track number */
		buf[5] = 0x16;                /* Data track, copying allowed */
		buf[6] = 0x01;                /* Only track is number 1 */
		store_cdrom_address(&buf[8], msf, 0);

		buf[13] = 0x16;                /* Lead-out track is data */
		buf[14] = 0xAA;                /* Lead-out track number */
		store_cdrom_address(&buf[16], msf, curlun->num_sectors);
		break;

	case 2:
		/* Raw TOC */
		len = 4 + 3*11;                /* 4 byte header + 3 descriptors */
		memset(buf, 0, len);        /* Header + A0, A1 & A2 descriptors */
		buf[1] = len - 2;        /* TOC Length excludes length field */
		buf[2] = 1;                /* First complete session */
		buf[3] = 1;                /* Last complete session */

		buf += 4;
		/* fill in A0, A1 and A2 points */
		for (i = 0; i < 3; i++) {
			buf[0] = 1;        /* Session number */
			buf[1] = 0x16;        /* Data track, copying allowed */
			/* 2 - Track number 0 ->  TOC */
			buf[3] = 0xA0 + i; /* A0, A1, A2 point */
			/* 4, 5, 6 - Min, sec, frame is zero */
			buf[8] = 1;        /* Pmin: last track number */
			buf += 11;        /* go to next track descriptor */
		}
		buf -= 11;                /* go back to A2 descriptor */

		/* For A2, 7, 8, 9, 10 - zero, Pmin, Psec, Pframe of Lead out */
		store_cdrom_address(&buf[7], msf, curlun->num_sectors);
		break;

	default:
		/* Multi-session, PMA, ATIP, CD-TEXT not supported/required */
		len = -EINVAL;
		break;
	}
	return len;
}
EXPORT_SYMBOL_GPL(fsg_get_soc);
//zj add for mac --
/*-------------------------------------------------------------------------*/


ssize_t fsg_show_ro(struct fsg_lun *curlun, char *buf)
{
	return sprintf(buf, "%d\n", fsg_lun_is_open(curlun)
				  ? curlun->ro
				  : curlun->initially_ro);
}
EXPORT_SYMBOL_GPL(fsg_show_ro);

ssize_t fsg_show_nofua(struct fsg_lun *curlun, char *buf)
{
	return sprintf(buf, "%u\n", curlun->nofua);
}
EXPORT_SYMBOL_GPL(fsg_show_nofua);

ssize_t fsg_show_file(struct fsg_lun *curlun, struct rw_semaphore *filesem,
		      char *buf)
{
	char		*p;
	ssize_t		rc;

	down_read(filesem);
	if (fsg_lun_is_open(curlun)) {	/* Get the complete pathname */
		p = d_path(&curlun->filp->f_path, buf, PAGE_SIZE - 1);
		if (IS_ERR(p))
			rc = PTR_ERR(p);
		else {
			rc = strlen(p);
			memmove(buf, p, rc);
			buf[rc] = '\n';		/* Add a newline */
			buf[++rc] = 0;
		}
	} else {				/* No file, return 0 bytes */
		*buf = 0;
		rc = 0;
	}
	up_read(filesem);
	return rc;
}
EXPORT_SYMBOL_GPL(fsg_show_file);

ssize_t fsg_show_cdrom(struct fsg_lun *curlun, char *buf)
{
	return sprintf(buf, "%u\n", curlun->cdrom);
}
EXPORT_SYMBOL_GPL(fsg_show_cdrom);

ssize_t fsg_show_removable(struct fsg_lun *curlun, char *buf)
{
	return sprintf(buf, "%u\n", curlun->removable);
}
EXPORT_SYMBOL_GPL(fsg_show_removable);

/*
 * The caller must hold fsg->filesem for reading when calling this function.
 */
static ssize_t _fsg_store_ro(struct fsg_lun *curlun, bool ro)
{
	if (fsg_lun_is_open(curlun)) {
		LDBG(curlun, "read-only status change prevented\n");
		return -EBUSY;
	}

	curlun->ro = ro;
	curlun->initially_ro = ro;
	LDBG(curlun, "read-only status set to %d\n", curlun->ro);

	return 0;
}

ssize_t fsg_store_ro(struct fsg_lun *curlun, struct rw_semaphore *filesem,
		     const char *buf, size_t count)
{
	ssize_t		rc;
	bool		ro;

	rc = strtobool(buf, &ro);
	if (rc)
		return rc;

	/*
	 * Allow the write-enable status to change only while the
	 * backing file is closed.
	 */
	down_read(filesem);
	rc = _fsg_store_ro(curlun, ro);
	if (!rc)
		rc = count;
	up_read(filesem);

	return rc;
}
EXPORT_SYMBOL_GPL(fsg_store_ro);

ssize_t fsg_store_nofua(struct fsg_lun *curlun, const char *buf, size_t count)
{
	bool		nofua;
	int		ret;

	ret = strtobool(buf, &nofua);
	if (ret)
		return ret;

	/* Sync data when switching from async mode to sync */
	if (!nofua && curlun->nofua)
		fsg_lun_fsync_sub(curlun);

	curlun->nofua = nofua;

	return count;
}
EXPORT_SYMBOL_GPL(fsg_store_nofua);

ssize_t fsg_store_file(struct fsg_lun *curlun, struct rw_semaphore *filesem,
		       const char *buf, size_t count)
{
	int		rc = 0;

#if !defined(CONFIG_USB_G_ANDROID)
	if (curlun->prevent_medium_removal && fsg_lun_is_open(curlun)) {
		LDBG(curlun, "eject attempt prevented\n");
		return -EBUSY;				/* "Door is locked" */
	}
#endif

	pr_notice("fsg_store_file file=%s, count=%d, curlun->cdrom=%d\n", buf, (int)count, curlun->cdrom);

	/*
	 * WORKAROUND:VOLD would clean the file path after switching to bicr.
	 * So when the lun is being a CD-ROM a.k.a. BICR. Dont clean the file path to empty.
	 */
	if (curlun->cdrom == 1 && count == 1)
		return count;

	/*
	 * WORKAROUND:Should be closed the fsg lun for virtual cd-rom, when switch to
	 * other usb functions. Use the special keyword "off", because the init can
	 * not parse the char '\n' in rc file and write into the sysfs.
	 */
	if (count == 3 &&
		buf[0] == 'o' && buf[1] == 'f' && buf[2] == 'f' &&
		fsg_lun_is_open(curlun)) {
			((char *) buf)[0] = 0;
	}

	/* Remove a trailing newline */
	if (count > 0 && buf[count-1] == '\n')
		((char *) buf)[count-1] = 0;		/* Ugh! */

	/* Load new medium */
	down_write(filesem);
	if (count > 0 && buf[0]) {
		/* fsg_lun_open() will close existing file if any. */
		rc = fsg_lun_open(curlun, buf);
		if (rc == 0)
			curlun->unit_attention_data =
					SS_NOT_READY_TO_READY_TRANSITION;
	} else if (fsg_lun_is_open(curlun)) {
		fsg_lun_close(curlun);
		curlun->unit_attention_data = SS_MEDIUM_NOT_PRESENT;
	}
	up_write(filesem);
	return (rc < 0 ? rc : count);
}
EXPORT_SYMBOL_GPL(fsg_store_file);

ssize_t fsg_store_cdrom(struct fsg_lun *curlun, struct rw_semaphore *filesem,
			const char *buf, size_t count)
{
	bool		cdrom;
	int		ret;

	ret = strtobool(buf, &cdrom);
	if (ret)
		return ret;

	down_read(filesem);
	ret = cdrom ? _fsg_store_ro(curlun, true) : 0;

	if (!ret) {
		curlun->cdrom = cdrom;
		ret = count;
	}
	up_read(filesem);

	return ret;
}
EXPORT_SYMBOL_GPL(fsg_store_cdrom);

ssize_t fsg_store_removable(struct fsg_lun *curlun, const char *buf,
			    size_t count)
{
	bool		removable;
	int		ret;

	ret = strtobool(buf, &removable);
	if (ret)
		return ret;

	curlun->removable = removable;

	return count;
}
EXPORT_SYMBOL_GPL(fsg_store_removable);

MODULE_LICENSE("GPL");
