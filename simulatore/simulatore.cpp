#include <iostream>
#include "simulatore.h"
#include <or1ksim.h>

#define BASE_ADDR = 0x30000010
#define NUM_PARAM = 3
#define MIO_ADDR = 0x30000000
#define 

int contatore = 4;
int stato = -1;
int risultato = 0;
int statovero = 

int funione_lettura(void              *class_ptr,
                    unsigned long int  addr,
                    unsigned char      mask[],
                    unsigned char      rdata[],
                    int                data_len) {
  
  
  
  return -1;
}



int funione_scrittura(void              *class_ptr,
                      unsigned long int  addr,
                      unsigned char      mask[],
                      unsigned char      wdata[],
                      int                data_len) {
  if(stato = 0){
     return 0;
  }else{
  int op1;
  int op2;
  unsigned long int addr_op1 = BASE_ADDR;
  unsigned long int addr_op2 = BASE_ADDR+sizeof(int);
  unsigned long int addr_state = STATE_ADDR;
  or1ksim_read_mem(addr_op1 , &op1 , sizeof(int) );
  or1ksim_read_mem(addr_op2 , &op2 , sizeof(int) );
  
  risultato = op1+op2;
  stato = 0;
  or1ksim_write_mem(addr_state , &stato , sizeof(int) );

  
  
  
  return 0;
  }
}








int main(int argc, char* argv[]) {

  char* defaultargv[4];
  defaultargv[0] = const_cast<char *>("./simulatore");
  defaultargv[1] = const_cast<char *>("-f");
  defaultargv[2] = const_cast<char *>("or1ksim.cfg");
  defaultargv[3] = const_cast<char *>("vmlinux");
  
  if(argc=1){
	argc = 4;
	argv = defaultargv;
  }
  
  std::cout << "Inizializzo il simulatore" << std::endl;
  std::cout << "argc: " << argc << " e argv(deferenziato): " << std::endl;
  for(int i=0;i < 4;i++){
	std::cout << argv[i] << std::endl;
  }
  
  if ( or1ksim_init(argc, argv, NULL, NULL, funzione_scrittura) == OR1KSIM_RC_OK) {
    std::cout << "Riuscita inizializzazione simulatore" << std::endl;
    
    while(1){
    or1ksim_run(100);
    if(stato = 0){
	contatore--;
	if(contatore < 0){
		or1ksim_write_mem(STATE_ADDR , &stato , sizeof(int) );
		or1ksim_write_mem(BASE_ADDR+3*sizeof(int) , &risultato , sizeof(int) );
		stato = -1;
		}
	}
  }
  
  
    
  } else {
    std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }
  

  //simulazione acceleratore acc d'esempio
  


  
}
