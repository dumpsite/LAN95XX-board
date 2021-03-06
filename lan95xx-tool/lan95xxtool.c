/*
 * LAN95XXTOOL.C
 * 
 * This is an example for communication with
 * the LAN95XX ethernet controller.
 * It can be used to read hardware statistic
 * counters or the chipid.
 * 
 * This is version 20161017T1200ZSB
 *
 * Stephan Baerwolf (matrixstorm@gmx.de), Schwansee 2016
 * (please contact me at least before commercial use)
 */

#define __MAIN_C_d98e3c32094c4feba6311b74c71782c6

#include "usbhelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>
#include <errno.h>

#if _WIN32
#	include <fcntl.h>
#endif


#define USBDEV_SHARED_VENDOR    0x0424
#define USBDEV_SHARED_PRODUCT   0xEC00
#define USBDEV_VENDOR_NAME	""
#define USBDEV_DEVICE_NAME	""

#ifdef MYDEBUG
#	define fdebugf(file, args...) fprintf(file, ##args)
#else
#	define fdebugf(file, args...) 
#endif

#ifndef _BV
#	define _BV(x)	((1)<<(x))
#endif

static void usage(char *name)
{
#define __plusstr	"[Bus] [Device]"
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s help\n\n", name);
    fprintf(stderr, "  %s list\n\n", name);
    fprintf(stderr, "  %s chipid "__plusstr"\n\n", name);
    fprintf(stderr, "  %s eedump "__plusstr"\n\n", name);
    fprintf(stderr, "  statistics commands:\n");
    fprintf(stderr, "  %s rxstats "__plusstr"\n", name);
    fprintf(stderr, "  %s txstats "__plusstr"\n\n", name);
    fprintf(stderr, "  register-read/write commands:\n");
    fprintf(stderr, "  %s rr:<register-address in hex> "__plusstr"\n", name);
    fprintf(stderr, "  %s rw:<register-address in hex>:<value in hex> "__plusstr"\n\n", name);
    fprintf(stderr, "  internal phy-read/write commands:\n");
    fprintf(stderr, "  %s phyr:<register-address in decimal> "__plusstr"\n", name);
    fprintf(stderr, "  %s phyw:<register-address in decimal>:<value in hex> "__plusstr"\n\n", name);
#undef __plusstr
}


static int listdevicesMAP(const struct usb_bus *bus, const struct usb_device *dev, const usb_dev_handle *handle, const uint16_t vendor, const uint16_t product, const char *vendorName, const char *productName, const char *serial, void* userparameter) {
//  if (strcmp(vendorName, USBDEV_VENDOR_NAME) == 0) {
//    if (strcmp(productName, USBDEV_DEVICE_NAME) == 0) {
      int *counter = userparameter;
      (*counter)++;
      fprintf(stderr, "\tBUS %03i DEVICE %03i: ID %04x:%04x SMSC9512/9514 Fast Ethernet Adapter\n", (int)bus->location, (int)dev->devnum, (int)vendor, (int)product);
//    }
//  }
  return 0;
}
static void listdevices(const uint32_t busID, const uint8_t deviceID) {
  int counter = 0;
    fprintf(stderr, "compatible devices:\n");

    /* the following must return 0 - ensure that with mapping function */
    usbScanDevice(NULL, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, listdevicesMAP, &counter);

    if (counter<=0) {
      fprintf(stderr, "\tno suitable device(s) found!\n");
    }
    fflush(stderr);
}

// lan9512_lan9512i databook rev. 1.2 (03-01-12).pdf page 74
#define __RXSTATSTR_PRINTF_INJECTION	"%23s"
static const char *rxstatstr[8] = {
  "RX good frames         ",
  "RX CRC errors          ",
  "RX runt frame errors   ",
  "RX alligment errors    ",
  "RX frame to long       ",
  "RX later collision     ",
  "RX bad frames total    ",
  "RX FIFO drops          "
};

#define __TXSTATSTR_PRINTF_INJECTION	__RXSTATSTR_PRINTF_INJECTION
static const char *txstatstr[10] = {
  "TX good frames         ",
  "TX pause frames        ",
  "TX single collisions   ",
  "TX multiple collisions ",
  "TX excessive collisions",
  "TX late collisions     ",
  "TX buffer underrun err.",
  "TX excessive deferrals ",
  "TX carrier errors      ",
  "TX bad frames total    ",
};


int main(int argc, char **argv) {
    usb_dev_handle      *handle = NULL;
    unsigned char       buffer[32 + 1 + 1 + 8];
    int                 nBytes, txReset=0;
    uint32_t		busID;
    uint32_t		deviceID;

    usbhelper_initialize();
    
    if (argc > 2) busID=atol(argv[2]);
    else busID=0xffffffff;
    
    if (argc > 3) deviceID=atoi(argv[3]);
    else deviceID=0xff;

    
    if (argc < 2) {
      usage(argv[0]);
    }else if(strcmp(argv[1], "help") == 0){
      usage(argv[0]);
    }else if(strcmp(argv[1], "list") == 0){
      listdevices(busID, deviceID);
    }else if((strcmp(argv[1], "id") == 0)||(strcmp(argv[1], "chipid") == 0)){
      uint32_t val;
      // connect to the LAN95XX controller
      if(usbOpenDevice(&handle, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, USBDEV_VENDOR_NAME, USBDEV_DEVICE_NAME, "") != 0) {
	  fprintf(stderr, "Could not find USB device \"LAN95XX\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
	  exit(1);
      }
      fdebugf(stderr,"%i: connecting to LAN95XX controller...\n",__LINE__);

      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, 0, (char *)&val, sizeof(val), 5000);
      if (nBytes >= 0) {
	fdebugf(stderr,"%i: Bytes = %i\n", __LINE__, nBytes);
	fdebugf(stderr,"%i: chipid-return: 0x%08x\n",__LINE__,(unsigned int)val);
      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
    }else if((strcmp(argv[1], "rx") == 0) || (strcmp(argv[1], "rxstats") == 0)){
      uint32_t i,val;
      // connect to the LAN95XX controller
      if(usbOpenDevice(&handle, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, USBDEV_VENDOR_NAME, USBDEV_DEVICE_NAME, "") != 0) {
	  fprintf(stderr, "Could not find USB device \"LAN95XX\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
	  exit(1);
      }
      fdebugf(stderr,"%i: connecting to LAN95XX controller...\n",__LINE__);

      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA2, 0, 0, (char *)&buffer[0], 0x20, 5000);
      if (nBytes >= 0) {
	for (i=0;i<nBytes;i++) {
	  if ((i % 4) == 0) {
	    fprintf(stderr,"\n");
	    if ((i / 4) < (sizeof(rxstatstr)/sizeof(char*)))	fdebugf(stderr,__RXSTATSTR_PRINTF_INJECTION " --> ", rxstatstr[i/4]);
	    else						fdebugf(stderr,__RXSTATSTR_PRINTF_INJECTION " --> ", "");
	  }
	  fprintf(stderr," 0x%02x", (unsigned int)buffer[i]);
	  val>>=8;
	  val|=((uint32_t)buffer[i])<<24;
	  if (((i+1) % 4) == 0) {
	    fprintf(stderr," (%u)",(unsigned int)val);
	  }
	}
	fprintf(stderr,"\n\n");
	fdebugf(stderr,"%i: Bytes = %i\n", __LINE__, nBytes);
      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
    }else if((strcmp(argv[1], "tx") == 0) || (strcmp(argv[1], "txstats") == 0)){
      uint32_t i,val;
      // connect to the LAN95XX controller
      if(usbOpenDevice(&handle, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, USBDEV_VENDOR_NAME, USBDEV_DEVICE_NAME, "") != 0) {
	  fprintf(stderr, "Could not find USB device \"LAN95XX\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
	  exit(1);
      }
      fdebugf(stderr,"%i: connecting to LAN95XX controller...\n",__LINE__);

      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA2, 0, 1, (char *)&buffer[0], 0x28, 5000);
      if (nBytes >= 0) {
	for (i=0;i<nBytes;i++) {
	  if ((i % 4) == 0) {
	    fprintf(stderr,"\n");
	    if ((i / 4) < (sizeof(txstatstr)/sizeof(char*)))	fdebugf(stderr,__TXSTATSTR_PRINTF_INJECTION " --> ", txstatstr[i/4]);
	    else						fdebugf(stderr,__TXSTATSTR_PRINTF_INJECTION " --> ", "");
	  }
	  fprintf(stderr," 0x%02x", (unsigned int)buffer[i]);
	  val>>=8;
	  val|=((uint32_t)buffer[i])<<24;
	  if (((i+1) % 4) == 0) {
	    fprintf(stderr," (%u)",(unsigned int)val);
	  }
	}
	fprintf(stderr,"\n\n");
	fdebugf(stderr,"%i: Bytes = %i\n", __LINE__, nBytes);
      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
    }else if ((strcmp(argv[1], "rr") >= 0) || (strcmp(argv[1], "rw") >= 0)) {
      int	register_addr=0; /* chipid */
      uint32_t	register_value=0;
      int	operation=0xa1;
      char	*helper;
      /* generate a termination-safe copy for strtok of the argument */
      memset(&buffer[0], 0, sizeof(buffer));
      strncpy(&buffer[0], argv[1], sizeof(buffer)-1);

      /* parse the input */
      helper=strtok(buffer, ":");
      if (helper)
	if (strcmp(helper, "rw")==0) operation=0xa0;
      helper=strtok(NULL,":");
      if (helper) register_addr=strtol(helper, NULL, 16);
      helper=strtok(NULL,":");
      if (helper) register_value=strtol(helper, NULL, 16);

      // connect to the LAN95XX controller
      if(usbOpenDevice(&handle, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, USBDEV_VENDOR_NAME, USBDEV_DEVICE_NAME, "") != 0) {
	  fprintf(stderr, "Could not find USB device \"LAN95XX\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
	  exit(1);
      }
      fdebugf(stderr,"%i: %s LAN95XX controller at register 0x%03x...\n",__LINE__,(operation==0xa0)?"writing":"reading",(unsigned int)register_addr);

      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | ((operation==0xa0)?(USB_ENDPOINT_OUT):(USB_ENDPOINT_IN)), operation, 0, register_addr, (char *)&register_value, sizeof(register_value), 5000);
      if (nBytes >= 0) {
	fdebugf(stderr,"%i: Bytes = %i\n", __LINE__, nBytes);
	fdebugf(stderr,"%i: value: 0x%08x\n",__LINE__,(unsigned int)register_value);
      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));

    }else if((strcmp(argv[1], "phyr") >= 0) || (strcmp(argv[1], "phyw") >= 0)){
      uint32_t	val;
      int	register_addr=1; /* basic status register */
      uint32_t	register_value=0;
      int	operation=0xa1;
      char	*helper;
      /* generate a termination-safe copy for strtok of the argument */
      memset(&buffer[0], 0, sizeof(buffer));
      strncpy(&buffer[0], argv[1], sizeof(buffer)-1);

      /* parse the input */
      helper=strtok(buffer, ":");
      if (helper)
	if (strcmp(helper, "phyw")==0) operation=0xa0;
      helper=strtok(NULL,":");
      if (helper) register_addr=strtol(helper, NULL, 10);
      helper=strtok(NULL,":");
      if (helper) register_value=strtol(helper, NULL, 16);

      if(usbOpenDevice(&handle, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, USBDEV_VENDOR_NAME, USBDEV_DEVICE_NAME, "") != 0) {
	  fprintf(stderr, "Could not find USB device \"LAN95XX\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
	  exit(1);
      }
      fdebugf(stderr,"%i: connecting to LAN95XX controller...\n",__LINE__);

#define MII_DATA_REG			(0x118)
#define MII_ACCESS_REG			(0x114)
#define MII_ACCESS__MIIBZY_BIT		(0)
#define MII_ACCESS__MIIWnR_BIT		(1)
#define MII_ACCESS__MIIPHY_LSB		(6)
#define MII_ACCESS__MUST_BIT		(11)
      /* check the MII if it is accessable */
      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, MII_ACCESS_REG, (char *)&val, sizeof(val), 5000);
      if (nBytes >= 0) {
	if ((val & _BV(MII_ACCESS__MIIBZY_BIT))==0) {
	  if (operation==0xa0) {
	    /* do a phy write - write the MII_DATA_REG first */
	    val = (register_value&(0xffff));
	    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, 0xA0, 0, MII_DATA_REG, (char *)&val, sizeof(val), 5000);
	    val = _BV(MII_ACCESS__MIIWnR_BIT);
	  } else {
	    /* do a phy read */
	    val = 0;
	  }
	  if (nBytes >= 0) {
	    /* continue prepare the value for MII_ACCESS_REG */
	    val|=_BV(MII_ACCESS__MUST_BIT) | _BV(MII_ACCESS__MIIBZY_BIT) | ((register_addr & 0x1f)<<MII_ACCESS__MIIPHY_LSB);
	    /* signal the MII_ACCESS_REG the read or write of phy */
	    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, 0xA0, 0, MII_ACCESS_REG, (char *)&val, sizeof(val), 5000);
	    while (nBytes >= 0) {
	      /* poll the MII if it is accessable again */
	      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, MII_ACCESS_REG, (char *)&val, sizeof(val), 5000);
	      if ((val & _BV(MII_ACCESS__MIIBZY_BIT))==0) {
		break;
	      }
	      fdebugf(stderr,"%i: Bytes = %i (polling)\n", __LINE__, nBytes);
	    }
	    if (nBytes >= 0) {
	      /* read the contents of MII_DATA_REG */
	      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, MII_DATA_REG, (char *)&val, sizeof(val), 5000);
	      if (nBytes >= 0) {
		register_value=val;
		fdebugf(stderr,"%i: Bytes = %i\n", __LINE__, nBytes);
		fdebugf(stderr,"%i: value: 0x%04x (@phy-addr: %3d)\n",__LINE__,(unsigned int)register_value, (unsigned int)register_addr);
	      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
	    } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
	  } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
	} else fdebugf(stderr,"%i: phy is busy right now - please try again later\n",__LINE__);
      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));

    }else if(strcmp(argv[1], "eedump") == 0){
#define E2P_CMD_REG			(0x30)
#define E2P_DATA_REG			(0x34)
#define E2P_CMD__E2PBZY_BIT		(31)
      uint32_t	val;
      int i, allBytes;
      if(usbOpenDevice(&handle, busID, deviceID, USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT, USBDEV_VENDOR_NAME, USBDEV_DEVICE_NAME, "") != 0) {
	  fprintf(stderr, "Could not find USB device \"LAN95XX\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
	  exit(1);
      }
      fdebugf(stderr,"%i: connecting to LAN95XX controller...\n",__LINE__);

      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, E2P_CMD_REG, (char *)&val, sizeof(val), 5000);
      if (nBytes >= 0) {
	if ((val & _BV(E2P_CMD__E2PBZY_BIT)) == 0) {

#if _WIN32
	  setmode(fileno(stdout), O_BINARY);
#endif
	  fdebugf(stderr,"%i: dumping eeprom to stdout...\n",__LINE__);
	  for (i=0;i<512;i++) {
	    val=i & 0x1ff;
	    val|=_BV(E2P_CMD__E2PBZY_BIT);

	    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, 0xA0, 0, E2P_CMD_REG, (char *)&val, sizeof(val), 5000);
	    if (nBytes < 0) break;
	    allBytes+=nBytes;

	    while (val & _BV(E2P_CMD__E2PBZY_BIT)) {
	      usleep(10000);
	      nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, E2P_CMD_REG, (char *)&val, sizeof(val), 5000);
	      if (nBytes < 0) break;
	      allBytes+=nBytes;
	    }
	    if (nBytes < 0) break;
	    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 0xA1, 0, E2P_DATA_REG, (char *)&val, sizeof(val), 5000);
	    if (nBytes < 0) break;
	    allBytes+=nBytes;
	    fputc(val & 0xff, stdout);
	    fflush(stdout);
	  }

	  if (nBytes >= 0) {
	    fdebugf(stderr,"%i: Bytes = %i (%i total)\n", __LINE__, nBytes, allBytes);
	  } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));
	} else fdebugf(stderr,"%i: eeprom is busy right now - please try again later\n",__LINE__);
      } else fdebugf(stderr,"%i: %s\n",__LINE__, strerror(errno));

    } else {
          usage(argv[0]);
     }

    if (handle) usb_close(handle);
    handle=NULL;
    usbhelper_finalize();
    return 0;
}
