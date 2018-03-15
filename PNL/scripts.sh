function qemu-run
{
qemu-system-x86_64 -s -S -hda pnl-tp.img \
	-hdb myHome.img \
	-kernel linux-4.9.82/arch/x86_64/boot/bzImage \
	-append 'root=/dev/sda1 rw console=ttyS0 kgdbwait' \
	--virtfs local,path=.,mount_tag=share,security_model=passthrough,id=share \
	-serial stdio \
	2>log.txt
}

function qemu-run-sh-as-init
{
qemu-system-x86_64 -s -S -hda pnl-tp.img \
	-hdb myHome.img \
	-kernel linux-4.9.82/arch/x86_64/boot/bzImage \
	-append 'root=/dev/sda1 rw vga=791 console=ttyS0 
				init=/bin/sh' \
	--virtfs local,path=.,mount_tag=share,security_model=passthrough,id=share \
	-serial stdio \
	2>log.txt
}

function qemu-run-init
{
qemu-system-x86_64 -s -S -hda pnl-tp.img \
	-hdb myHome.img \
	-kernel linux-4.9.82/arch/x86_64/boot/bzImage \
	-append 'root=/dev/sda1 rw vga=791 console=ttyS0 
				kgdboc=ttyS1' \
	--virtfs local,path=.,mount_tag=share,security_model=passthrough,id=share \
	-initrd $1 \
	-serial stdio \
	2>log.txt
}
