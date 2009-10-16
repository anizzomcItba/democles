

void videoPrint(char *fmt){
	char *video = (char *) 0xB8000;
	int i = 1;
	while(*fmt){
		video[i+=2] = *fmt;
	}
}
