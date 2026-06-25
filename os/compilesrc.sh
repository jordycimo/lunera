#!/bin/bash

# remove old
cd "$(echo $HOME | tr -d '\r')/Documents/lunera/os/initramfs/bin"
for f in *; do
    echo "rm $f" ;
    rm $f;
done

# now compile
cd "$(echo $HOME | tr -d '\r')/Documents/lunera/os/src/bin"
for f in *; do
    out=${f%".c"}
    echo "$f->$out" ;
    ../../i486-linux-musl-gcc -static -O2 -march=i486 -mtune=pentiumpro -mpreferred-stack-boundary=2 -o\
    ../../initramfs/bin/$out\
    ./$f ;
done
