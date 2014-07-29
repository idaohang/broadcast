#!/bin/bash
AWK=/usr/bin/awk
ECHO=/bin/echo
GREP=/bin/grep
UDEVADM=/sbin/udevadm
SED=/bin/sed
TOUCH=/bin/touch
RM=/bin/rm

$TOUCH /opt/udevlock
udev0=$($UDEVADM info -a -n /dev/ttyUSB0)
udev1=$($UDEVADM info -a -n /dev/ttyUSB1)
udev2=$($UDEVADM info -a -n /dev/ttyUSB2)

$ECHO $udev0 | sed "s/ /\n/g" | $GREP -m 1 "InterfaceNumber" | $AWK -F'"' '{print $2}' > /opt/usb0
$ECHO $udev0 | sed "s/ /\n/g" | $GREP -m 1 "DRIVERS" | $AWK -F'"' '{print $2}' >> /opt/usb0

$ECHO $udev1 | sed "s/ /\n/g" | $GREP -m 1 "InterfaceNumber" | $AWK -F'"' '{print $2}' > /opt/usb1
$ECHO $udev1 | sed "s/ /\n/g" | $GREP -m 1 "DRIVERS" | $AWK -F'"' '{print $2}' >> /opt/usb1

$ECHO $udev2 | sed "s/ /\n/g" | $GREP -m 1 "InterfaceNumber" | $AWK -F'"' '{print $2}' > /opt/usb2
$ECHO $udev2 | sed "s/ /\n/g" | $GREP -m 1 "DRIVERS" | $AWK -F'"' '{print $2}' >> /opt/usb2

$RM /opt/udevlock
