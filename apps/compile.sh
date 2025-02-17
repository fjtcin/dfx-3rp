INC='-I/usr/include/xrt -I/usr/include/dfx-mgr'
LNK='-luuid -lxrt_coreutil -lxrt++ -ldfx-mgr'
g++ -Wall -g -std=c++1y $INC lib/siha.c add_sub.c $LNK -o testADDSUB
g++ -Wall -g -std=c++1y $INC lib/siha.c inter_add.c $LNK -o testINTER
g++ -Wall -g -std=c++1y $INC lib/siha.c gemm.c $LNK -o testGEMM
