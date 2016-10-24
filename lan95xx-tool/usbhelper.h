/*
 * USBHELPER.H
 * based on vusbdevice.h
 * 
 * This is version 20161017T1200ZSB specific for LAN95XX-board
 * https://github.com/dumpsite/LAN95XX-board
 *
 * Stephan Baerwolf (matrixstorm@gmx.de), Schwansee 2016
 * (please contact me at least before commercial use)
 */

#ifndef __USBHELPER_H_e25922918d5d40bc99895958f02024b4
#	define __USBHELPER_H_e25922918d5d40bc99895958f02024b4 1

#	if  defined(__unix__)
#		include <usb.h>    /* this is libusb, see http://libusb.sourceforge.net/ */
#	else
#		if	(defined(_WIN32) || defined(_WIN64))
#			include <lusb0_usb.h>
#		else
#			error	unknown platform
#		endif
#	endif

#include <stdint.h>
#include  <string.h>

#define USB_ERROR_NOTFOUND  1
#define USB_ERROR_ACCESS    2
#define USB_ERROR_IO        3
#define USB_ERROR_NOINIT    251


#ifdef __USBHELPER_C_e25922918d5d40bc99895958f02024b4
#	define	USBHELPERPUBLIC
#else
#	define	USBHELPERPUBLIC		extern
#endif

USBHELPERPUBLIC int usbhelper_initialize(void);
USBHELPERPUBLIC int usbhelper_finalize(void);

USBHELPERPUBLIC int usbGetStringAscii(usb_dev_handle *dev, int index, int langid, char *buf, size_t buflen);


typedef int (*usbhelper_devicecallback_t)(const struct usb_bus *bus, const struct usb_device *dev, const usb_dev_handle *handle, const uint16_t vendor, const uint16_t product, const char *vendorName, const char *productName, const char *serial, void* userparameter);


/*
 * Scan over all usb devices calling "mapfunc" for each.
 * When "mapfunc" returns non-zero the scanning is terminated and the result used as return for "usbScanDevice".
 * When "mapfunc" returns positive, device stays open for further use.
 */
USBHELPERPUBLIC int usbScanDevice(usb_dev_handle **device, const uint32_t busID, const uint8_t deviceID, const uint16_t idvendor, const uint16_t idproduct, usbhelper_devicecallback_t mapfunc, void* userparameter);

/*
 * "usbOpenDevice" opens a spezial USB device.
 * The Device primary is identified by its idVendor:idProduct.
 * Because for most VUSB this is the same (free4all) 0x16C0:0x05DC, userdefinable identificationstrings are used here, too.
 */
USBHELPERPUBLIC int usbOpenDevice(usb_dev_handle **device, const uint32_t busID, const uint8_t deviceID, const uint16_t idvendor, const uint16_t idproduct, const char *vendorName, const char *productName, const char *serial);

#endif
