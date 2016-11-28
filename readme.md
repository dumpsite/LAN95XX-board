SMSC LAN95XX adapter board
==========================

This board serves as a carrier for the famous LAN95XX 
(LAN9512, LAN9513 or LAN9514) USB to fast Ethernet controller.

All the (high) frequency-, power- and decoupling stuff is done
on the board, while the most interesting pins are routed to the
outside edges. There the pitch is the usual 2.54mm (0.1") and
easily accessible for any further projects.

The board features an onboard eeprom (type "93C56 or 93C66") in cheap
SO8 package and implements a 25 MHz 5032 SMD crystal circuit.
(For cheaper HC49* versions there was no space...)
The smallest component is 0603, but most of them are replaceable with some 0805...

All is done (and works!) with two active design layers only!
A theoretical third layer is "emulated" by connecting some edge-pins together.
So it needs not be produced with the PCB itself...
The traces are scaled a bit thinner to compensate spreading during design transfer
to copper. All package footprints are using longer pin pads.
The primary IC (LAN95XX) QFN package footprint is extended by an 3.2mm inner
bonding hole, allowing backside soldering and solder bridging the two layers.

All this allows fully DIY capability and "etching at home".
VIAs can be drilled using 0.2mm or 0.3mm drill.
Side-to-side bonding can be done using stripped AWG30 wire wrapping wire.


The minimal essential size is "33mm x 33mm" using castellations or
"36mm x 36mm" using full pin headers.
Including the whole PCB area having the board nicely centered, its
dimensions are "50mm x 50mm" .


All the content is open and freely usable in non commercial applications.
It would be very kind to mention me in your projects and ask me first if you plan commercial use...

by Stephan Baerwolf (stephan@matrixstorm.com), Schwansee 2016

(who is not bearing any responsibility or guarantee of any kind for his projects)