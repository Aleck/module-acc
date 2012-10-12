#include <stdio.h>
#include <stdlib.h>

int main(){
	int op_1 = 1;
	int op_2 = 3;
	int risultato = 0;
	int parti = 0;
	int *ptr1;
	int *ptr2;
	int *ptr3;
	int *ptr0;
	ptr1 = 0x02000001; 
	ptr2 = 0x02000002;
	ptr3 = 0x02000003;
	ptr0 = 0x02000000;
	*ptr1 = op_1;
	*ptr2 = op_2;
	*ptr0 = parti;
	while(1){
		if(*ptr0 = 1){
			*ptr0 = 0;
			risultato = *ptr3;
			break;
		}	
		delay(10);
	}
	printf("il risultato ottenuto è: %i", risultato);
	return 0;
}
