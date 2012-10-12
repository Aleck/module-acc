#include <iostream>
#include "acc.h"
#include <or1ksim.h>

#define ADDR_START_DEF = 1000;
#define START_LEN = 1;
#define ADDR_OP_1 = 1001;
#define OP_1_LEN = 1;
#define ADDR_OP_2 = 1002;
#define OP_2_LEN = 1;
#define ADDR_RIS = 1003;
#define RIS_LEN = 1;

//classe acceleratore

class acc{
public:
unsigned long int addr_start = ADDR_START;
int start_len = START_LEN;
unsigned long int addr_op_1 = ADDR_OP_1;
int op_1_len = OP_1_LEN;
unsigned long int addr_op_2 = ADDR_OP_2;
int op_2_len = OP_2_LEN;
unsigned long int addr_ris = ADDR_RIS;
int ris_len = RIS_LEN;
}



