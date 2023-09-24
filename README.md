# GhostBox
Creation of a GhostBox from a RDA5807M module

Everything is in the title. I have been recently interested in the ghostbox/spiritbox topic and felt a bit annoyed when saw the price of such device.
Basically, a ghostbox is a fm radio tuner that keeps searching for station without stopping, creating the effect you are looking for.
Looking at the popular P-SB11 from obscure "ITC research device" on the web, it is obviously overpriced. 
So I had the idea of buying a cheap DIY FM radio kit, assemble it and reprogram the MCU to make it work as a ghostbox.

I choose a quite cheap RDA5807M based kit from the web (widely available on a well known chinese marketplace) for less than 20$.
The kit is very simple, FM band only, 4 buttons and a 4 7-segment module. More than enough for what I want to achieve.
It is driven by a STC15W408AS which is a very cheap but powerfull mcu that is fully 8051 compatible.
You wont find a lot of information about this mcu on the web, unless you are fluent in chinese, fortunately the datasheet is available in english.
As it is 8051 architecture based, it should be straight forward.

The first thing to put in place is the toolchain. You basically have two options:
- The Keil IDE and STC-ISP flash and muti purpose utility from STC
- The VS code IDE with Embedded IDE plugin and stcgal open source flash utility
In both case I use the Keil C51 compiler.

After trying both options, I decided to use the second one mostly because it is much more convenient to do everything from the same window (code, compile, flash, monitor).
Though, the source code repository will work with both options. 
