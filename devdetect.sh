#!/bin/bash
AWK=/usr/bin/awk
ECHO=/bin/echo
GREP=/bin/grep
UDEVADM=/sbin/udevadm
SED=/bin/sed
RM=/bin/rm

if [ -e  /opt/usb ]; then
	$RM /opt/usb
fi

for i in 0 1 2 3
do
udev=$($UDEVADM info -a -n /dev/ttyUSB$i)
IN=$($ECHO $udev | sed "s/ /\n/g" | $GREP -m 1 "InterfaceNumber" | $AWK -F'"' '{print $2}') 
DR=$($ECHO $udev | sed "s/ /\n/g" | $GREP -m 1 "DRIVERS" | $AWK -F'"' '{print $2}') 

$ECHO "/dev/ttyUSB"$i" "$IN" "$DR >> /opt/usb
done
if [ -e /opt/usb ]; then
	echo "all is well"
	#/usr/local/bin/info &
	#/usr/local/bin/bcast &
fi
