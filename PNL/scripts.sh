function qemu-run
{
qemu-system-x86_64 -hda pnl-tp.img \
	-hdb myHome.img \
	-kernel linux-4.9.82/arch/x86_64/boot/bzImage \
	-append 'root=/dev/sda1 rw vga=768 console=ttyS0 
				kgdboc=ttyS1' \
	-serial stdio \
	2>log.txt
}

function qemu-run-sh-as-init
{
qemu-system-x86_64 -hda pnl-tp.img \
	-hdb myHome.img \
	-kernel linux-4.9.82/arch/x86_64/boot/bzImage \
	-append 'root=/dev/sda1 rw vga=768 console=ttyS0 
				kgdboc=ttyS1 init=/bin/sh' \
	-serial stdio \
	2>log.txt
}

function qemu-run-init
{
qemu-system-x86_64 -hda pnl-tp.img \
	-hdb myHome.img \
	-kernel linux-4.9.82/arch/x86_64/boot/bzImage \
	-append 'root=/dev/sda1 rw vga=768 console=ttyS0 
				kgdboc=ttyS1' \
	-initrd $1 \
	-serial stdio \
	2>log.txt
}
