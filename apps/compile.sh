INC='-I/usr/include/xrt -I/usr/include/dfx-mgr'
LNK='-luuid -lxrt_coreutil -lxrt++ -ldfx-mgr'
sudo g++ -Wall -g -std=c++1y $INC lib/siha.c add_sub.c $LNK -o testADDSUB
