#!/bin/bash

DEVICE=/dev/ram0
modprobe brd rd_nr=1 rd_size=2097152
SIZE=$(blockdev --getsize $DEVICE)
DELAY=500
echo "0 $SIZE delay $DEVICE 0 $DELAY" | dmsetup create delayed
