ADDR=`cat /sys/devices/platform/tegra-sdhci.0/mmc_host/mmc0/mmc0:0001/mmc0:0001:1/net/wlan0/address`
A=`echo $ADDR | cut -d ':' -f 4`
B=`echo $ADDR | cut -d ':' -f 5`
C=`echo $ADDR | cut -d ':' -f 6`
D=`echo $ADDR | cut -d ':' -f 3`
E=`echo $ADDR | cut -d ':' -f 1`
F=`echo $ADDR | cut -d ':' -f 2`
line1='// BT_ADDR'
line2="&0001 = 00$A $B$C 00$D $E$F" 
echo $line1 > /data/bluez_provider/bluecore6.psr
echo $line2 >> /data/bluez_provider/bluecore6.psr
cat /system/etc/bluecore6.psr >> /data/bluez_provider/bluecore6.psr
echo "done" > /data/bluez_provider/bluez_reg

if [ -e /data/bluez_provider/bluez_reg ]; then
echo "bluez registered"
else
cat /system/etc/bluecore6.psr > /data/bluez_provider/bluecore6.psr
chmod 777 /data/bluez_provider/bluecore6.psr
fi

mount -w -o remount -t yaffs2 /dev/block/mtdblock3 /system
ln -s /data/bluez_provider/bluecore6.psr /system/etc/bluez/bluecore6.psr
