#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	int op_1 = 1;
	int op_2 = 3;
	int risultato = 0;
	int parti = 0;
	int mem = open("/dev/acc", O_RDWR);
	int w1 = write(mem, &op_1, sizeof(int));
	int w2 = write(mem, &op_2, sizeof(int));
	int w3 = write(mem, &parti, sizeof(int));
	printf("le dimensioni delle write(in bytes) sono: %i, %i e %i .\n", w1, w2, w3);
	while(1){
		
		if(){
			*ptr0 = 0;
			risultato = *ptr3;
			break;
		}	
		delay(10);
	}
	printf("il risultato ottenuto è: %i. \n", risultato);
	close(mem);
	return 0;
}
