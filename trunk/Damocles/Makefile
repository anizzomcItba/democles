INC_PATH = include/
OBJ_PATH = obj/
DIRECTIVES = -fno-builtin -nostartfiles -nostdlib -fno-stack-protector -I $(INC_PATH)

all:
	nasm -f aout -g loader.asm -o $(OBJ_PATH)entry/loader.o
	nasm -f aout -g sys.asm -o $(OBJ_PATH)sysasm.o
	
	gcc -Wall -g -m32 -c drivers/rtc/rtc.c -o $(OBJ_PATH)drivers_rtc_rtc.o $(DIRECTIVES)
	
	gcc -Wall -g -m32 -c kernel.c -o $(OBJ_PATH)kernel.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c sys.c  -o $(OBJ_PATH)sysc.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c drivers/video/crtc6845.c -o $(OBJ_PATH)crtc6845.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c lib/string.c -o $(OBJ_PATH)string.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c lib/video.c -o $(OBJ_PATH)video.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c drivers/keyboard/keyboard.c -o $(OBJ_PATH)keyboard.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c drivers/timer/timer.c -o $(OBJ_PATH)timer.o $(DIRECTIVES) 
	gcc -Wall -g -m32 -c drivers/mouse/mouse.c -o $(OBJ_PATH)mouse.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c lib/stdio.c -o $(OBJ_PATH)stdio.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c lib/math.c -o $(OBJ_PATH)math.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c shell/shell.c -o $(OBJ_PATH)shell.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c services/clipboard.c -o $(OBJ_PATH)clipboard.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/fortune.c -o $(OBJ_PATH)fortune.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c mmu.c -o $(OBJ_PATH)mmu.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c lib/stack.c -o $(OBJ_PATH)stack.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c IO.c -o $(OBJ_PATH)IO.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c sched.c -o $(OBJ_PATH)sched.o $(DIRECTIVES)
	#gcc -Wall -g -m32 -c schedpriority.c -o $(OBJ_PATH)sched.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c sysvideo.c -o $(OBJ_PATH)sysvideo.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c syscall.c -o $(OBJ_PATH)syscall.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c ttys.c -o $(OBJ_PATH)ttys.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process.c -o $(OBJ_PATH)process.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c semaphore.c -o $(OBJ_PATH)semaphore.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c debug.c -o $(OBJ_PATH)debug.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c lib/syslib.c -o $(OBJ_PATH)syslib.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/top.c -o $(OBJ_PATH)top.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c exceptions.c -o $(OBJ_PATH)exceptions.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c filesystem.c -o $(OBJ_PATH)filesystem.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/cat.c -o $(OBJ_PATH)cat.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/tree.c -o $(OBJ_PATH)tree.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/cd.c -o $(OBJ_PATH)cd.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/ls.c -o $(OBJ_PATH)ls.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/open.c -o $(OBJ_PATH)open.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/rm.c -o $(OBJ_PATH)rm.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/mkdir.c -o $(OBJ_PATH)mkdir.o $(DIRECTIVES)
	gcc -Wall -g -m32 -c process/write.c -o $(OBJ_PATH)write.o $(DIRECTIVES)
	ld -melf_i386 -T link.ld  -o kernel.bin $(OBJ_PATH)entry/loader.o $(OBJ_PATH)*.o
	mcopy -o kernel.bin h:/boot/
	
clear:
	rm -f $(OBJ_PATH)entry/*.o
	rm -f $(OBJ_PATH)*.o
