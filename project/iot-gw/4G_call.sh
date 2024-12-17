#! /bin/bash
echo -e "AT+QCFG=\"usbnet\",1" > /dev/ttyUSB1
echo -e "AT+qnetdevctl=1,1,1" > /dev/ttyUSB1
udhcp usb0