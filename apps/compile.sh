INC='-I/usr/include/xrt -I/usr/include/dfx-mgr'
LNK='-luuid -lxrt_coreutil -lxrt++ -ldfx-mgr'
g++ -Wall -g -std=c++1y $INC lib/siha.c twice.c $LNK -o testTWICE
