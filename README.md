# signalcli-node-serial

## What is it?

A way of retrieving (and soon, sending) Signal messages via an internet connection, and re-broadcasting them locally over low-power radio to a remote node.

## Usage

In the current (crude) version, I'm running a simple bash script ('looper.sh') that runs the NodeJS script ('main.js') that polls the Signal server every few seconds (with command line parameters for Signal user and the name of the USB port), pulls down new messages, and sends them out via the micro gateway.  

E.g.: 

```
./looper.sh +18575551212 /dev/ttyUSB0
```


## Why?

Two main rationales for this system:
- I use Signal a lot, but I don't like interacting with smartphones.  I wanted to be able to get new messages (especially urgent ones) via a system where I had control over the complexity of the interface.  Right now new messages appear on a screen on a remote device, when they arrive, with a timestamp; I hope to develop simple hardware buttons that allow me to interact with the messages or reply to them.
- This system might be useful in scenarios where there is a source of internet within a kilometer or so, but no cellular service, and where folks would still like to be able to interact with Signal in a convenient way.  In particular, I'm imagining people living in rural areas, or folks inside large buildings or in urban areas with spotty wifi access and bad cellular coverage. 

## Current Design

### Hardware Overview

The current prototype hardware system consists of:
1. a PC or single-board computer ('SBC', e.g. aRaspberry Pi), running linux, and connected to the internet;
2. a 'gateway' microcontroller that has a LoRa radio is connected to the PC via a serial connection (USB cable);
3. a 'remote' microcontroller that has a LoRa radio and a display.

### Data flow overview

Data currently flows in three stages (corresponding to the hardware systems listed above):
1. A script (written in Javascript / NodeJS) running on the PC / SBC periodically retrieves new messages from the Signal server (using an install of [signal-cli](https://github.com/AsamK/signal-cli));
2. This program sends this message in JSON format to the serial port that is connected to the 'gateway'
3. The 'gateway' relays this message (as a simple custom data structure) over LoRa to the 'remote', which displays it.

## Hardware + Software requirements for current design

- The PC / SBC is running Linux, and has a system-wide install of signal-cli (could accommodate a local install with simple code tweaks); I've tested the NodeJS and signal-cli code on Ubuntu and on Raspberry Pi 3B+.
- The PC / SBC installation of signal-cli needs to be 'linked' to a Signal account (this can be done via the command line; see instructions in the signal-cli repo). 
- The 'gateway' microcontroller must be capable of sending / receiving data over USB / serial (typical for most micros these days that have a microUSB connector), and has LoRa radio attached (I'm currently using a Heltec Wifi LoRa 32 v2 for the gateway, but e.g. a Feather M0 LoRa orsseveral other hardware boards should work).
- The 'remote' micro has a LoRa radio and a screen (I'm using a Feather M0 LoRa with an SSD1306 124x65 OLED).

## Pin connections

Note that the e-ink uses the SPI bus; in the code, they're not mentioned, but you'll need to connect SCK to the clock pin on the e-ink, and MOSI to the DI pin on the e-ink.  


