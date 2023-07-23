#!/bin/sh
rm -Rf u-boot.bin.xz

xz -z -k u-boot.bin
ms_ver="$(strings -a -T binary u-boot.bin | grep 'MVX' | grep 'UBT1501' | sed 's/\\*MVX/MVX/g' | cut -c 1-32)"

#out_file=u-boot.img.bin
out_file_xz=u-boot.xz.img.bin

echo ""
echo $out_file_xz
echo mkimage -A arm -O u-boot -C xz -a 0 -e 0 -n "$(echo $ms_ver)" -d u-boot.bin.xz "$out_file_xz"
mkimage -A arm -O u-boot -C lzma -a 0 -e 0 -n "$ms_ver" -d u-boot.bin.xz "$out_file_xz"
rm -Rf u-boot.bin.xz
echo ""