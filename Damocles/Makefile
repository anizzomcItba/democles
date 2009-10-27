all:
	nasm -f aout -g loader.asm -o loader.o
	nasm -f aout -g sys.asm -o sysasm.o
	gcc -Wall -g  -m32 -c kernel.c -o kernel.o
	gcc -Wall -g  -m32 -c sys.c -fno-stack-protector -o sysc.o
	gcc -Wall -g  -m32 -c -fno-stack-protector drivers/video/crtc6845.c -o crtc6845.o
	gcc -Wall -g 	-m32 -c lib/string.c -o string.o
	gcc -Wall -g -m32 -c lib/video.c -o video.o
	gcc -Wall -g -m32 -c drivers/keyboard/keyboard.c -o keyboard.o
	gcc -Wall -g -m32 -fno-stack-protector -c lib/stdio.c -o stdio.o
	gcc -Wall -g -m32 -c drivers/mouse/mouse.c -o mouse.o
	gcc -Wall -g -m32 -c lib/math.c -o math.o
	gcc -Wall -g -m32 -c drivers/timer/timer.c -o timer.o
	gcc -Wall -g -m32 -c -fno-stack-protector shell/shell.c -o shell.o
	gcc -Wall -g -m32 -c services/clipboard.c -o clipboard.o
	gcc -Wall -g -m32 -c services/fortune.c -o fortune.o
	gcc -Wall -g -m32 -c mmu.c -o mmu.o
	gcc -Wall -g -m32 -c lib/stack.c -o stack.o
	gcc -Wall -g -m32 -c IO.c -o IO.o -fno-stack-protector
	gcc -Wall -g -m32 -c sched.c -o sched.o
	gcc -Wall -g -m32 -c sysvideo.c -o sysvideo.o
	gcc -Wall -g -m32 -c syscall.c -o syscall.o
	ld -melf_i386 -T link.ld -o kernel.bin *.o
	rm *.o
	mcopy -o kernel.bin a:/boot/