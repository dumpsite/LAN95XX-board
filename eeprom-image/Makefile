all:
	$(gccprefix)gcc -o eeprom.elf  lan95xx_eeprom_configurator.c
	$(gccprefix)objcopy -O binary -j .usbmsg eeprom.elf eeprom.bin
	hexdump -C eeprom.bin

clean:
	rm eeprom.elf
	rm eeprom.bin