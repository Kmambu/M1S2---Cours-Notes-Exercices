#!/bin/sh
find . | cpio -o -H newc | gzip > ../my_initramfs.cpio.gz
