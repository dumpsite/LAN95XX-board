
#include <stdio.h>
#include <stdlib.h>

//#include <endian.h>
#include <stdint.h>


/* basic usb standard stuff - align by since all sizes are even... */
#define USBMSG __attribute__((aligned(1),section(".usbmsg")))

#ifndef htole16
#	define htole16(x)	(x)
#	warning htole16 not defined!
#endif
#define USB_STRING_DESCRIPTOR_LEN(stringLength) (htole16((stringLength>0)?(2*(stringLength)+2):(0)))
#define USB_STRING_DESCRIPTOR_HEADER(stringLength) (htole16((USB_STRING_DESCRIPTOR_LEN(stringLength)) | (3<<8)))

#define USBTOEEPROM_WORDOFFSET(eeptr) ((sizeof(lan95xx_eeprom_header)+eeptr)>>1)
#define STRTOEEPROM_WORDOFFSET(eeptr) ((sizeof(lan95xx_eeprom_header)+(2*sizeof(struct usbDeviceDescriptor))+(2*sizeof(struct usbConfigurationDescriptor))+(2*sizeof(struct usbInterfaceDescriptor))+eeptr)>>1)


/* debugging and more */
#if 1
#define XTYPE_LAN951X 2
#define LAN951X_SELFPOWERED 0
#define LAN951X_REMOTEWAKEUP 0

#define MANUFACTURERIDSTRINGDESC_LEN 15
#define MANUFACTURERIDSTRINGDESC_MSG 'm','a','t','r','i','x','s','t','o','r','m','.','c','o','m'

#define PRODUCTNAMESTRINGDESC_LEN 6
#define PRODUCTNAMESTRINGDESC_MSG 'u','s','b','n','e','t'

#define SERIALNUMBERSTRINGDESC_LEN 16
#define SERIALNUMBERSTRINGDESC_MSG '0','0','0','-','2','0','1','6','1','0','1','6','2','0','3','0'

#endif


/* content definitions */

#ifndef XTYPE_LAN951X
#	define XTYPE_LAN951X	2
#endif

#ifndef LAN951X_SELFPOWERED
#	define LAN951X_SELFPOWERED	1
#endif

#ifndef LAN951X_REMOTEWAKEUP
#	define LAN951X_REMOTEWAKEUP	1
#endif

/* manufactureridstring */
#ifndef MANUFACTURERIDSTRINGDESC_LEN
#	define MANUFACTURERIDSTRINGDESC_LEN 0
#endif

#if (MANUFACTURERIDSTRINGDESC_LEN > 0)
#	ifndef MANUFACTURERIDSTRINGDESC_MSG
#		error MANUFACTURERIDSTRINGDESC_MSG has length but is not defined
#	endif
#endif

/* productnamestring */
#ifndef PRODUCTNAMESTRINGDESC_LEN
#	define PRODUCTNAMESTRINGDESC_LEN 0
#endif

#if (PRODUCTNAMESTRINGDESC_LEN > 0)
#	ifndef PRODUCTNAMESTRINGDESC_MSG
#		error PRODUCTNAMESTRINGDESC_LEN has length but is not defined
#	endif
#endif

/* serialnumberstring */
#ifndef SERIALNUMBERSTRINGDESC_LEN
#	define SERIALNUMBERSTRINGDESC_LEN 0
#endif

#if (SERIALNUMBERSTRINGDESC_LEN > 0)
#	ifndef SERIALNUMBERSTRINGDESC_MSG
#		error SERIALNUMBERSTRINGDESC_MSG has length but is not defined
#	endif
#endif

/* configurationstring */
#ifndef CONFIGURATIONSTRINGDESC_LEN
#	define CONFIGURATIONSTRINGDESC_LEN 0
#endif

#if (CONFIGURATIONSTRINGDESC_LEN > 0)
#	ifndef CONFIGURATIONSTRINGDESC_MSG
#		error CONFIGURATIONSTRINGDESC_MSG has length but is not defined
#	endif
#endif

/* interfacestring */
#ifndef INTERFACESTRINGDESC_LEN
#	define INTERFACESTRINGDESC_LEN 0
#endif

#if (INTERFACESTRINGDESC_LEN > 0)
#	ifndef INTERFACESTRINGDESC_MSG
#		error INTERFACESTRINGDESC_MSG has length but is not defined
#	endif
#endif

#define USBCONFIGFLAGS_RESERVED_MUSTSET (0x80)
#define USBCONFIGFLAGS_SELFPOWERED	(0x40)
#define USBCONFIGFLAGS_REMOTEWAKEUP	(0x20)

#define EEPROMCONFIGFLAGS_SELFPOWERED	(0x1)
#define EEPROMCONFIGFLAGS_REMOTEWAKEUP	(0x4)

struct usbDeviceDescriptor {
  uint8_t	bLength;
  uint8_t	bDescriptorType;
  uint16_t	bcdUSB;
  uint8_t	bDeviceClass;
  uint8_t	bDeviceSubClass;
  uint8_t	bDeviceProtocol;
  uint8_t	bMaxPacketSize;
  uint16_t	idVendor;
  uint16_t	idProduct;
  uint16_t	bcdDevice;
  uint8_t	iManufacturer;
  uint8_t	iProduct;
  uint8_t	iSerialNumber;
  uint8_t	bNumConfigurations;
} __attribute__((packed));

struct usbConfigurationDescriptor {
  uint8_t	bLength;
  uint8_t	bDescriptorType;
  uint16_t	wTotalLength;
  uint8_t	bNumInterfaces;
  uint8_t	bConfigurationValue;
  uint8_t	iConfiguration;
  uint8_t	bmAttributes;
  uint8_t	bMaxPower;
} __attribute__((packed));

struct usbInterfaceDescriptor {
  uint8_t	bLength;
  uint8_t	bDescriptorType;
  uint8_t	bInterfaceNumber;
  uint8_t	bAlternateSetting;
  uint8_t	bNumEndpoints;
  uint8_t	bInterfaceClass;
  uint8_t	bInterfaceSubClass;
  uint8_t	bInterfaceProtocol;
  uint8_t	iInterface;
} __attribute__((packed));

struct LAN95XXEEPROMlayout {
  uint8_t	eeprom_magic;
  uint8_t	macaddr[6];
  uint8_t	pollinginterval_fullspeed;
  uint8_t	pollinginterval_highspeed;
  uint8_t	configuration_flags;
  uint16_t	languageid_desc;

  uint8_t	manufactureridstring_desc_len;			/* remember to use "USB_STRING_DESCRIPTOR_LEN()" */
  uint8_t	manufactureridstring_desc_wordoffset;		/* use "STRTOEEPROM_WORDOFFSET()" ... */

  uint8_t	productnamestring_desc_len;			/* remember to use "USB_STRING_DESCRIPTOR_LEN()" */
  uint8_t	productnamestring_desc_wordoffset;		/* use "STRTOEEPROM_WORDOFFSET()" ... */

  uint8_t	serialnumberstring_desc_len;			/* remember to use "USB_STRING_DESCRIPTOR_LEN()" */
  uint8_t	serialnumberstring_desc_wordoffset;		/* use "STRTOEEPROM_WORDOFFSET()" ... */

  uint8_t	configurationstring_desc_len;			/* remember to use "USB_STRING_DESCRIPTOR_LEN()" */
  uint8_t	configurationstring_desc_wordoffset;		/* use "STRTOEEPROM_WORDOFFSET()" ... */

  uint8_t	interfacestring_desc_len;			/* remember to use "USB_STRING_DESCRIPTOR_LEN()" */
  uint8_t	interfacestring_desc_wordoffset;		/* use "STRTOEEPROM_WORDOFFSET()" ... */
  
  uint8_t	highspeed_device_desc_len;			/* should be 18 bytes - use sizeof(struct usbDeviceDescriptor) */
  uint8_t	highspeed_device_desc_wordoffset;		/* use "USBTOEEPROM_WORDOFFSET()" ... */
  uint8_t	highspeed_configuration_interface_desc_len;	/* should be also 9+9=18 bytes - use sizeof(struct usbConfigurationDescriptor)+sizeof(struct usbInterfaceDescriptor) */
  uint8_t	highspeed_configuration_interface_desc_wordoffset;

  uint8_t	fullspeed_device_desc_len;			/* should be 18 bytes - use sizeof(struct usbDeviceDescriptor) */
  uint8_t	fullspeed_device_desc_wordoffset;		/* use "USBTOEEPROM_WORDOFFSET()" ... */
  uint8_t	fullspeed_configuration_interface_desc_len;	/* should be also 9+9=18 bytes - use sizeof(struct usbConfigurationDescriptor)+sizeof(struct usbInterfaceDescriptor) */
  uint8_t	fullspeed_configuration_interface_desc_wordoffset;

  uint16_t	reserved_1e1f;



   /* 0x20 USB-Hub Configuration Registers (HCFG) */
  uint16_t	idHubVendor;
  uint16_t	idHubProduct;
  uint16_t	idHubDevice;
  uint8_t	cfg_data[3];
  uint8_t	nrd;						/* non-removable devices */
  uint8_t	pds;						/* port disable for self-powered operation */
  uint8_t	pdb;						/* port disable for bus-powered operation */
  uint8_t	maxps;						/* max power for self-powered operation */
  uint8_t	maxpb;						/* max power for bus-powered operation */
  uint8_t	hcmcs;						/* hub controller max current for self-powered operation */
  uint8_t	hcmcb;						/* hub controller max current for bus-powered operation */
  uint8_t	pwrt;						/* power on time */
  uint8_t	boostup;

#if (XTYPE_LAN951X==4)
  uint8_t	boost5;
#else
  uint8_t	reserved_32;
#endif
#if (XTYPE_LAN951X==2)
  uint8_t	boost32;
#elif ((XTYPE_LAN951X>2)&&(XTYPE_LAN951X<5))
  uint8_t	boost42;
#else
#	warning unknown XTYPE_LAN951X
  uint8_t	reserved_33;
#endif
  uint8_t	reserved_34;

  uint8_t	prtsp;
  uint8_t	prtr12;
#if (XTYPE_LAN951X==2)
  uint8_t	prtr3;
#elif ((XTYPE_LAN951X>2)&&(XTYPE_LAN951X<5))
  uint8_t	prtr34;
#else
#	warning unknown XTYPE_LAN951X
  uint8_t	reserved_37;
#endif
  
#if (XTYPE_LAN951X==4)
  uint8_t	prtr5;
#else
  uint8_t	reserved_38;
#endif
  uint8_t	stcd;						/* status command register */
} __attribute__((packed));


USBMSG const struct LAN95XXEEPROMlayout lan95xx_eeprom_header = {
  .eeprom_magic 					= 0xa5,
  /* (microchip technologies) 00:04:A3 : 12:34:56 */
  .macaddr						= {0x00,0x04,0xa3,0x12,0x34,0x56},
  .pollinginterval_fullspeed				= 01,
  .pollinginterval_highspeed				= 04,
  .configuration_flags					= (LAN951X_SELFPOWERED*EEPROMCONFIGFLAGS_SELFPOWERED)|(LAN951X_REMOTEWAKEUP*EEPROMCONFIGFLAGS_REMOTEWAKEUP),
  .languageid_desc					= htole16(0x409),

  .manufactureridstring_desc_len			= USB_STRING_DESCRIPTOR_LEN(MANUFACTURERIDSTRINGDESC_LEN),
#ifdef MANUFACTURERIDSTRINGDESC_MSG
  .manufactureridstring_desc_wordoffset			= STRTOEEPROM_WORDOFFSET(0),
#else
  .manufactureridstring_desc_wordoffset			= 0,
#endif
  
    .productnamestring_desc_len				= USB_STRING_DESCRIPTOR_LEN(PRODUCTNAMESTRINGDESC_LEN),
#ifdef PRODUCTNAMESTRINGDESC_MSG
   .productnamestring_desc_wordoffset			= STRTOEEPROM_WORDOFFSET(USB_STRING_DESCRIPTOR_LEN(MANUFACTURERIDSTRINGDESC_LEN)),
#else
   .productnamestring_desc_wordoffset			= 0,
#endif

    .serialnumberstring_desc_len			= USB_STRING_DESCRIPTOR_LEN(SERIALNUMBERSTRINGDESC_LEN),
#ifdef SERIALNUMBERSTRINGDESC_MSG
   .serialnumberstring_desc_wordoffset			= STRTOEEPROM_WORDOFFSET(USB_STRING_DESCRIPTOR_LEN(MANUFACTURERIDSTRINGDESC_LEN)+USB_STRING_DESCRIPTOR_LEN(PRODUCTNAMESTRINGDESC_LEN)),
#else
   .serialnumberstring_desc_wordoffset			= 0,
#endif

    .configurationstring_desc_len			= USB_STRING_DESCRIPTOR_LEN(CONFIGURATIONSTRINGDESC_LEN),
#ifdef CONFIGURATIONSTRINGDESC_MSG
   .configurationstring_desc_wordoffset			= STRTOEEPROM_WORDOFFSET(USB_STRING_DESCRIPTOR_LEN(MANUFACTURERIDSTRINGDESC_LEN)+USB_STRING_DESCRIPTOR_LEN(PRODUCTNAMESTRINGDESC_LEN)+USB_STRING_DESCRIPTOR_LEN(SERIALNUMBERSTRINGDESC_LEN)),
#else
   .configurationstring_desc_wordoffset			= 0,
#endif

    .interfacestring_desc_len				= USB_STRING_DESCRIPTOR_LEN(INTERFACESTRINGDESC_LEN),
#ifdef INTERFACESTRINGDESC_MSG
   .interfacestring_desc_wordoffset			= STRTOEEPROM_WORDOFFSET(USB_STRING_DESCRIPTOR_LEN(MANUFACTURERIDSTRINGDESC_LEN)+USB_STRING_DESCRIPTOR_LEN(PRODUCTNAMESTRINGDESC_LEN)+USB_STRING_DESCRIPTOR_LEN(SERIALNUMBERSTRINGDESC_LEN)+USB_STRING_DESCRIPTOR_LEN(CONFIGURATIONSTRINGDESC_LEN)),
#else
   .interfacestring_desc_wordoffset			= 0,
#endif

   .highspeed_device_desc_len				= sizeof(struct usbDeviceDescriptor),
   .highspeed_device_desc_wordoffset			= USBTOEEPROM_WORDOFFSET(0),
   .highspeed_configuration_interface_desc_len		= sizeof(struct usbConfigurationDescriptor)+sizeof(struct usbInterfaceDescriptor),
   .highspeed_configuration_interface_desc_wordoffset	= USBTOEEPROM_WORDOFFSET(sizeof(struct usbDeviceDescriptor)),

   .fullspeed_device_desc_len				= sizeof(struct usbDeviceDescriptor),
   .fullspeed_device_desc_wordoffset			= USBTOEEPROM_WORDOFFSET(sizeof(struct usbDeviceDescriptor)+sizeof(struct usbConfigurationDescriptor)+sizeof(struct usbInterfaceDescriptor)),
   .fullspeed_configuration_interface_desc_len		= sizeof(struct usbConfigurationDescriptor)+sizeof(struct usbInterfaceDescriptor),
   .fullspeed_configuration_interface_desc_wordoffset	= USBTOEEPROM_WORDOFFSET(2*sizeof(struct usbDeviceDescriptor)+sizeof(struct usbConfigurationDescriptor)+sizeof(struct usbInterfaceDescriptor)),



   /* 0x20 USB-Hub Configuration Registers (HCFG) */
   .idHubVendor						= htole16(0x0424),
   .idHubProduct					= htole16(0x9510 | XTYPE_LAN951X),
   .idHubDevice						= htole16(0x0200),
   .cfg_data						= {0x1f,0x18,0x00},
   .nrd							= 0x02,
//TODO:
   .pds							= 0x3c,
   .pdb							= 0x3c,

   .maxps						= 0,
   .maxpb						= 50,
   .hcmcs						= 0,
   .hcmcb						= 50,
   .pwrt						= 150,
   .boostup						= 0x00,
#if (XTYPE_LAN951X==4)
   .boost5						= 0x00,
#endif

#if   (XTYPE_LAN951X==2)
    .boost32						= 0x00,
#elif (XTYPE_LAN951X==3)
    .boost42						= 0x00,
#elif (XTYPE_LAN951X==4)
    .boost42						= 0x00,
#endif

    .prtsp						= 0x00,
    .prtr12						= 0x21,
#if   (XTYPE_LAN951X==2)
    .prtr3						= 0x03,
#elif (XTYPE_LAN951X==3)
    .prtr34						= 0x43,
#elif (XTYPE_LAN951X==4)
    .prtr34						= 0x43,
    .prtr5						= 0x05,
#endif

    .stcd						= 0x01,
};

// WARNING: order of following declarations DOES MATTER !

/* high-speed USB 2.0 */
static USBMSG const struct usbDeviceDescriptor deviceDescriptor_highspeed = {
  .bLength						= sizeof(struct usbDeviceDescriptor),
  .bDescriptorType					= 0x01,
  .bcdUSB						= 0x200,
  .bDeviceClass						= 0xff,
  .bDeviceSubClass					= 0x00,
  .bDeviceProtocol					= 0xff,
  .bMaxPacketSize					= 0x40,
  .idVendor						= 0x0424,
  .idProduct						= 0xec00,
  .bcdDevice						= 0x200,
#ifdef MANUFACTURERIDSTRINGDESC_MSG
  .iManufacturer					= 1,
#else
  .iManufacturer					= 0,
#endif
#ifdef PRODUCTNAMESTRINGDESC_MSG
  .iProduct						= 2,
#else
  .iProduct						= 0,
#endif
#ifdef SERIALNUMBERSTRINGDESC_MSG
  .iSerialNumber					= 3,
#else
  .iSerialNumber					= 0,
#endif
  .bNumConfigurations					= 0x01,
};

static USBMSG const struct usbConfigurationDescriptor configurationDescriptor_highspeed = {
  .bLength						= sizeof(struct usbConfigurationDescriptor),
  .bDescriptorType					= 0x02,
  .wTotalLength						= htole16(0x0027),
  .bNumInterfaces					= 0x01,
  .bConfigurationValue					= 0x01,
#ifdef CONFIGURATIONSTRINGDESC_MSG
  .iConfiguration					= 4,
#else
  .iConfiguration					= 0,
#endif
  .bmAttributes						= (LAN951X_SELFPOWERED*USBCONFIGFLAGS_SELFPOWERED)|(LAN951X_REMOTEWAKEUP*USBCONFIGFLAGS_REMOTEWAKEUP)|(USBCONFIGFLAGS_RESERVED_MUSTSET),
  .bMaxPower						= 50,
};

static USBMSG const struct usbInterfaceDescriptor deviceInterfaceDescriptor_highspeed = {
  .bLength						= sizeof(struct usbInterfaceDescriptor),
  .bDescriptorType					= 0x04,
  .bInterfaceNumber					= 0x00,
  .bAlternateSetting					= 0x00,
  .bNumEndpoints					= 0x03,
  .bInterfaceClass					= 0xff,
  .bInterfaceSubClass					= 0x00,
  .bInterfaceProtocol					= 0xff,
#ifdef INTERFACESTRINGDESC_MSG
  .iInterface						= 5,
#else
  .iInterface						= 0,
#endif
};

/* full-speed USB 1.1 */
static USBMSG const struct usbDeviceDescriptor deviceDescriptor_fullspeed = {
  .bLength						= sizeof(struct usbDeviceDescriptor),
  .bDescriptorType					= 0x01,
  .bcdUSB						= 0x200,
  .bDeviceClass						= 0xff,
  .bDeviceSubClass					= 0x00,
  .bDeviceProtocol					= 0xff,
  .bMaxPacketSize					= 0x40,
  .idVendor						= 0x0424,
  .idProduct						= 0xec00,
  .bcdDevice						= 0x200,
#ifdef MANUFACTURERIDSTRINGDESC_MSG
  .iManufacturer					= 1,
#else
  .iManufacturer					= 0,
#endif
#ifdef PRODUCTNAMESTRINGDESC_MSG
  .iProduct						= 2,
#else
  .iProduct						= 0,
#endif
#ifdef SERIALNUMBERSTRINGDESC_MSG
  .iSerialNumber					= 3,
#else
  .iSerialNumber					= 0,
#endif
  .bNumConfigurations					= 0x01,
};

static USBMSG const struct usbConfigurationDescriptor configurationDescriptor_fullspeed = {
  .bLength						= sizeof(struct usbConfigurationDescriptor),
  .bDescriptorType					= 0x02,
  .wTotalLength						= htole16(0x0027),
  .bNumInterfaces					= 0x01,
  .bConfigurationValue					= 0x01,
#ifdef CONFIGURATIONSTRINGDESC_MSG
  .iConfiguration					= 4,
#else
  .iConfiguration					= 0,
#endif
  .bmAttributes						= (LAN951X_SELFPOWERED*USBCONFIGFLAGS_SELFPOWERED)|(LAN951X_REMOTEWAKEUP*USBCONFIGFLAGS_REMOTEWAKEUP)|(USBCONFIGFLAGS_RESERVED_MUSTSET),
  .bMaxPower						= 50,
};

static USBMSG const struct usbInterfaceDescriptor deviceInterfaceDescriptor_fullspeed = {
  .bLength						= sizeof(struct usbInterfaceDescriptor),
  .bDescriptorType					= 0x04,
  .bInterfaceNumber					= 0x00,
  .bAlternateSetting					= 0x00,
  .bNumEndpoints					= 0x03,
  .bInterfaceClass					= 0xff,
  .bInterfaceSubClass					= 0x00,
  .bInterfaceProtocol					= 0xff,
#ifdef INTERFACESTRINGDESC_MSG
  .iInterface						= 5,
#else
  .iInterface						= 0,
#endif
};


#ifdef MANUFACTURERIDSTRINGDESC_MSG
static USBMSG const uint16_t manufactureridstring_desc[] = {
  USB_STRING_DESCRIPTOR_HEADER(MANUFACTURERIDSTRINGDESC_LEN),
  MANUFACTURERIDSTRINGDESC_MSG
};
#endif

#ifdef PRODUCTNAMESTRINGDESC_MSG
static USBMSG const uint16_t productnamestring_desc[] = {
  USB_STRING_DESCRIPTOR_HEADER(PRODUCTNAMESTRINGDESC_LEN),
  PRODUCTNAMESTRINGDESC_MSG
};
#endif

#ifdef SERIALNUMBERSTRINGDESC_MSG
static USBMSG const uint16_t serialnumberstring_desc[] = {
  USB_STRING_DESCRIPTOR_HEADER(SERIALNUMBERSTRINGDESC_LEN),
  SERIALNUMBERSTRINGDESC_MSG
};
#endif

#ifdef CONFIGURATIONSTRINGDESC_MSG
static USBMSG const uint16_t configurationstring_desc[] = {
  USB_STRING_DESCRIPTOR_HEADER(CONFIGURATIONSTRINGDESC_LEN),
  CONFIGURATIONSTRINGDESC_MSG
};
#endif

#ifdef INTERFACESTRINGDESC_MSG
static USBMSG const uint16_t interfacestring_desc[] = {
  USB_STRING_DESCRIPTOR_HEADER(INTERFACESTRINGDESC_LEN),
  INTERFACESTRINGDESC_MSG
};
#endif

#define myout(...) fprintf(stderr,__VA_ARGS__)

int main(int argc, char** argv) {
  unsigned long a,b;
  myout("sizeof(struct usbDeviceDescriptor)		= %2lu bytes\n",(unsigned long)sizeof(struct usbDeviceDescriptor));
  myout("sizeof(struct usbConfigurationDescriptor)	= %2lu bytes\n",(unsigned long)sizeof(struct usbConfigurationDescriptor));
  myout("sizeof(struct usbInterfaceDescriptor)		= %2lu bytes\n",(unsigned long)sizeof(struct usbInterfaceDescriptor));
  myout("sizeof(struct LAN95XXEEPROMlayout)		= %2lu bytes\n",(unsigned long)sizeof(struct LAN95XXEEPROMlayout));
  myout("\n");
  myout("USBTOEEPROM_WORDOFFSET(0)			= %2lu words (@0x%02lx)\n",(unsigned long)USBTOEEPROM_WORDOFFSET(0),(unsigned long)USBTOEEPROM_WORDOFFSET(0));
  myout("STRTOEEPROM_WORDOFFSET(0)			= %2lu words (@0x%02lx)\n",(unsigned long)STRTOEEPROM_WORDOFFSET(0),(unsigned long)STRTOEEPROM_WORDOFFSET(0));
  myout("\n");
  a=(unsigned long)&lan95xx_eeprom_header;      myout("&lan95xx_eeprom_header				= 0x%016lx (@0x%03lx)\n",a,a-a);
  b=(unsigned long)&deviceDescriptor_highspeed; myout("&deviceDescriptor_highspeed			= 0x%016lx (@0x%03lx)\n",b,b-a);
  b=(unsigned long)&configurationDescriptor_highspeed; myout("&configurationDescriptor_highspeed		= 0x%016lx (@0x%03lx)\n",b,b-a);
  b=(unsigned long)&deviceInterfaceDescriptor_highspeed; myout("&deviceInterfaceDescriptor_highspeed		= 0x%016lx (@0x%03lx)\n",b,b-a);
  myout("\n");
  b=(unsigned long)&deviceDescriptor_fullspeed; myout("&deviceDescriptor_fullspeed			= 0x%016lx (@0x%03lx)\n",b,b-a);
  b=(unsigned long)&configurationDescriptor_fullspeed; myout("&configurationDescriptor_fullspeed		= 0x%016lx (@0x%03lx)\n",b,b-a);
  b=(unsigned long)&deviceInterfaceDescriptor_fullspeed; myout("&deviceInterfaceDescriptor_fullspeed		= 0x%016lx (@0x%03lx)\n",b,b-a);
  myout("\n");

#ifdef MANUFACTURERIDSTRINGDESC_MSG
  b=(unsigned long)&manufactureridstring_desc; myout("&manufactureridstring_desc			= 0x%016lx (@0x%03lx)\n",b,b-a);
#endif
#ifdef PRODUCTNAMESTRINGDESC_MSG
  b=(unsigned long)&productnamestring_desc; myout("&productnamestring_desc				= 0x%016lx (@0x%03lx)\n",b,b-a);
#endif
#ifdef SERIALNUMBERSTRINGDESC_MSG
  b=(unsigned long)&serialnumberstring_desc; myout("&serialnumberstring_desc			= 0x%016lx (@0x%03lx)\n",b,b-a);
#endif  
#ifdef CONFIGURATIONSTRINGDESC_MSG
  b=(unsigned long)&configurationstring_desc; myout("&configurationstring_desc			= 0x%016lx (@0x%03lx)\n",b,b-a);
#endif
#ifdef INTERFACESTRINGDESC_MSG
  b=(unsigned long)&interfacestring_desc; myout("&interfacestring_desc				= 0x%016lx (@0x%03lx)\n",b,b-a);  
#endif
  return EXIT_SUCCESS;
}
