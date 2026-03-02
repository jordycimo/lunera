# Lunera

Lunera is a project to bring the modern linux kernel to the intel celeron 533.
*All of the code within ./src/bin/ is compiled using musl for the target cpu. 

### how to build
*The script _./compilesrc.sh_ runs through _./src/bin_ and spits out the ELF binaries in _./initramfs/bin_.
*The script _./buildinitramfs.sh_ builds a cpio file with _./initramfs/_ and gzips it so the kernel can load it as the ram fs during init.
*The directories _linux/_ and _bootfloppy/_ are respectively a clone of the linux github repo, and bootgrub, a port of grub to work on old BIOS systems from the floppy drive. I would link bootgrub, but the github page has dissapeared. Lucky me that I have a copy !!.

### why
I recently picked up an absolute shitter of a pc from craigslist. It's powered by an intel [celeron 533](https://theretroweb.com/chips/1389) and sports a [systemax CW35-S](https://theretroweb.com/motherboards/s/dfi-cw35-s) motherboard. It's way easier to develop low level code for these old machines so I've set off to do just that.

### running it
running this is really easy and if I set up my compiling scripts correctly, should run on any intel cpu released after the i486 instruction set was released. I haven't tested it, but in theory this should be able to run on a modern cpu. Setting up a floppy boot disk is really easy and can be done simply by copying the contents of the _bootfloppy/_ directory to any IBM formatted floppy. The bootloader will search for the first hard drive and boot with _bzImage_ as the kernel, and _initramfs.cpio.gz_ as the initrd. I have my main disk set up with a 4gb swap partition (though, that is very overkill) and a 60gb Fat32 partition as the remaining disk space. With the kernel and initramfs files on the root of the Fat32 partition, this will boot into a working userspace.
