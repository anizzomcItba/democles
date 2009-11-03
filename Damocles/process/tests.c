/* tests.c */

/* Las siguientes tienen que ir a un archivo aparte dentro de la carpeta process */


void opDie();
void zeroDie();
void overDie();


void zeroDie(){
	int i, a;

	printf("Pid del proceso para crear zero division exception: %d!\n", getpid());

	for(i=0; ;i++){
		a=20/(60-i);
		sleep(100);
	}
}

void overDie(){

	unsigned int a=0xFFFFFFF0, i;

	printf("Pid del proceso para crear overflow exception: %d!\n", getpid());

	for (i=0; ; i++){
		a += i;
		_overDie();
	}
}

void opDie(){
	printf("Pid del proceso para crear invalid opcode exception %d!", getpid());
	sleep(100);
	_opDie();
}


