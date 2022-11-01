#!/bin/bash
#cd /root/Bela && make run PROJECT=palms CL="-p 128 -N4 --high-performance-mode"
#cd /root/Bela && make run PROJECT=palms CL="-p 128 -N4"
cd /root/Bela && make run PROJECT=palms CL="-p 128 -N4 -Z0,1 -C4" LDLIBS="-lmonome"
