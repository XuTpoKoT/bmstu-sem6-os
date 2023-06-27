#!/bin/bash

make
sudo insmod myvfs.ko
lsmod | grep myvfs

echo
sudo dmesg | grep myvfs

echo
touch image
mkdir dir
touch image1
mkdir dir1

echo
sudo mount -o loop -t myvfs ./image ./dir
sudo mount -o loop -t myvfs ./image1 ./dir1

echo 
sudo cat /proc/filesystems
echo
sudo cat /proc/mounts | grep myvfs
echo
sudo cat /proc/slabinfo | grep MyVFSCache
mount | grep myvfs

