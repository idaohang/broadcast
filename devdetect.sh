#!/bin/bash
AWK=/usr/bin/awk
ECHO=/bin/echo
GREP=/bin/grep
UDEVADM=/sbin/udevadm
SED=/bin/sed
RM=/bin/rm
SERVICE=/usr/sbin/service
#IFUP=/sbin/ifup
#IFDOWN=/sbin/ifdown
DRIVERS=(pl2303)

if [ -e  /opt/usb ]; then
	$RM /opt/usb
fi

for i in 0 1 2 3
do
	if [ -e /dev/ttyUSB$i ]; then
		udev=$($UDEVADM info -a -n /dev/ttyUSB$i)
		IN=$($ECHO $udev | $SED "s/ /\n/g" | $GREP -m 1 "InterfaceNumber" | $AWK -F'"' '{print $2}') 
		DR=$($ECHO $udev | $SED "s/ /\n/g" | $GREP -m 1 "DRIVERS" | $AWK -F'"' '{print $2}')
		#for DRIVER in ${DRIVERS[*]}
		#do
			#if [ $DR = $DRIVER ]; then
				#echo "BANNER:banner:\r\nser2net\r\n\r\n" > /etc/ser2net.conf
				#echo "2000:telnet:600:/dev/ttyUSB"$i":9600 8DATABITS NONE 1STOPBIT banner" >> /etc/ser2net.conf
				#$SERVICE ser2net restart
				#continue
			#fi
		#done


		$ECHO "/dev/ttyUSB"$i","$IN","$DR >> /opt/usb
	fi
done

if [ -e /opt/usb ]; then
	echo "all is well"
fi
exit 0;
