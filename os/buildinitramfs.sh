#!/bin/bash

echo "ok!"

cd "$(echo $HOME | tr -d '\r')/Documents/lunera/os/" # exact path to be sure
pwd
rm initramfs.cpio.gz # delete old
rm initramfs.cpio # just in case
cd ./initramfs # go into the initramfs dir
pwd
find . -print0 | cpio --null -ov --format=newc > ../initramfs.cpio # compile all into one cpio file
cd ../ # go to new cpio file
pwd
gzip initramfs.cpio # gzip it

echo "done!"
