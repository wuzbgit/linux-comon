#! /bin/bash

cp out/rootfs/usr/bin/* /opt/Project/iot/t113_i/T113-i_v1.0/buildroot/buildroot-2022.08/board/boc_t113i/rootfs/opt/test/ -f
cp out/rootfs/usr/lib/libutils.so /opt/Project/iot/t113_i/T113-i_v1.0/buildroot/buildroot-2022.08/board/boc_t113i/rootfs/usr/lib/libutils.so -f
cp out/rootfs/usr/bin/board_iot_srv /opt/Project/iot/t113_i/T113-i_v1.0/buildroot/buildroot-2022.08/board/boc_t113i/rootfs/usr/bin/ -f
