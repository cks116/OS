#!/bin/sh

filename=$1

sudo ./main < OS_PJ1_Test/${filename}.txt > output/${filename}_stdout.txt
sudo dmesg -c | grep Project1 > output/${filename}_dmesg.txt
