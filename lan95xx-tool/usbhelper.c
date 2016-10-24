/*
 * USBHELPER.C
 * based on vusbdevice.c
 * 
 * This is version 20161017T1200ZSB specific for LAN95XX-board
 * https://github.com/dumpsite/LAN95XX-board
 *
 * Stephan Baerwolf (matrixstorm@gmx.de), Schwansee 2016
 * (please contact me at least before commercial use)
 */

#define __USBHELPER_C_e25922918d5d40bc99895958f02024b4 1

#include "usbhelper.h"

#include <stdio.h>

static long initialize_count = 0;

int usbhelper_initialize(void) {
  if (initialize_count==0L) {
    usb_init();
  }
  initialize_count++;
  return 0;
}

int usbhelper_finalize(void) {
  if (initialize_count>0L) {
    initialize_count--;
  } else return USB_ERROR_NOINIT;
  return 0;
}

int usbGetStringAscii(usb_dev_handle *dev, int index, int langid, char *buf, size_t buflen) {
char    buffer[256];
int     rval, i;

    if((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, langid, buffer, sizeof(buffer), 1000)) < 0)
        return rval;
    if(buffer[1] != USB_DT_STRING)
        return 0;
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0];
    rval /= 2;
    /* lossy conversion to ISO Latin1 */
    for(i=1;i<rval;i++){
        if(i > buflen)  /* destination buffer overflow */
            break;
        buf[i-1] = buffer[2 * i];
        if(buffer[2 * i + 1] != 0)  /* outside of ISO Latin1 range */
            buf[i-1] = '?';
    }
    buf[i-1] = 0;
    return i-1;
}

int usbScanDevice(usb_dev_handle **device, const uint32_t busID, const uint8_t deviceID, const uint16_t idvendor, const uint16_t idproduct, usbhelper_devicecallback_t mapfunc, void* userparameter) {
  struct usb_bus      *bus;
  struct usb_device   *dev;
  usb_dev_handle      *handle = NULL;
  int                 result = 0;
  static int          didUsbInit = 0;

  if (!mapfunc) return result;

  usb_find_busses();
  usb_find_devices();
  for (bus=usb_get_busses(); bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {
      if (((bus->location==busID) || (busID>=(uint32_t)0xffffffff)) && ((dev->devnum==deviceID) || (deviceID>=(uint8_t)0xff))) {
	if ((dev->descriptor.idVendor == idvendor) && (dev->descriptor.idProduct == idproduct)) {
	  handle = usb_open(dev); /* we need to open the device in order to query strings */
	  if (handle) {
	    char vendorstr[256], productstr[256], serialstr[256];
	    int  vendorlen,      productlen,      seriallen;

//	    vendorlen = usbGetStringAscii(handle, dev->descriptor.iManufacturer, 0x0409, vendorstr, sizeof(vendorstr));
//	    if (vendorlen>0) {
//	    productlen = usbGetStringAscii(handle, dev->descriptor.iProduct, 0x0409, productstr, sizeof(productstr));
//	    if (productlen>0) {
	      //seriallen = usbGetStringAscii(handle, dev->descriptor.iSerialNumber, 0x0409, serialstr, sizeof(serialstr));
	      serialstr[0]=0;
	      //if (seriallen>=0) {
		result=mapfunc(bus, dev, handle, dev->descriptor.idVendor, dev->descriptor.idProduct, vendorstr, productstr, serialstr, userparameter);
	      //}
//	    }
//	    }

	    if (result<0) {
	      usb_close(handle);
	      handle=NULL;
	    }
	    if (result) {
	      *device = handle;
	      return result;
	    }
	  } /* end: valid handle */
	} /* vendor and product ok */
      } /* right bus and device location */
    }
  }

  return result;
}

struct __usbOpenDeviceMAPdata {
  const char *vendorName;
  const char *productName;
  const char *serial; 
};

static int __usbOpenDeviceMAP(const struct usb_bus *bus, const struct usb_device *dev, const usb_dev_handle *handle, const uint16_t vendor, const uint16_t product, const char *vendorName, const char *productName, const char *serial, void* userparameter) {
  struct __usbOpenDeviceMAPdata *data=userparameter;
//  if (strcmp(data->vendorName, vendorName) == 0)
//    if (strcmp(data->productName, productName) == 0)
//      if ((strlen(data->serial)==0) || (strcmp(data->serial, serial) == 0)) return 1;
  return 1;
}

int usbOpenDevice(usb_dev_handle **device, const uint32_t busID, const uint8_t deviceID, const uint16_t idvendor, const uint16_t idproduct, const char *vendorName, const char *productName, const char *serial) {
  struct __usbOpenDeviceMAPdata params;
  params.vendorName=vendorName;
  params.productName=productName;
  params.serial=serial;
  if (usbScanDevice(device, busID, deviceID, idvendor, idproduct, __usbOpenDeviceMAP, &params)>0) {
    return 0;
  }
  return USB_ERROR_NOTFOUND;
}
