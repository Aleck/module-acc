#include <iostream>
#include <string.h>
#include <or1ksim.h>
#include <stdlib.h>

#define BASE_ADDR  0x30000000



// state value
#define READY 0
#define COMPUTING 1
#define DONE 2

//transmission mode 32 bit = 4 byte
#define TX_COEF 4

//the computation delay 
#define COMPUTATION_DELAY 3
#define UNIT_DELAY 0.5 //(in ms)




//the struct used by the module
struct acc_parameter {
	int state;
	int return_value;
	int param1;
	int param2;
};




int param_size = (sizeof(struct acc_parameter))*TX_COEF;
int size_counter = 0;
struct acc_parameter* acc_param;

//the computation thick delay ( 1 thik = 100ms)
int unit_count = COMPUTATION_DELAY;



int my_read_function(void              *class_ptr,
                    unsigned long int  addr,
                    unsigned char      mask[],
                    unsigned char      rdata[],
                    int                data_len) {
  
  
  
  return -1;
}



int my_write_function(void              *class_ptr,
                      unsigned long int  addr,
                      unsigned char      mask[],
                      unsigned char      wdata[],
                      int                data_len) {
        //offset of the address
        //write the data in my memory
	for(int index=0; index < sizeof(wdata); index++) {
		//memset(acc_param + sizeof(unsigned char)*index*TX_COEF + size_counter, wdata[index], sizeof(unsigned char));
		std::cout << "STUB: Received char " << (int)wdata[index] << std::endl;;
	}
        
        //update the size counter
        size_counter += data_len;
        
	//print the written memory
	std::cout << "STUB: Received  " << size_counter << " bit over " << param_size << std::endl;
	
	
	// when the transmission is done, start the computation
	if (size_counter == param_size) {
		acc_param->state = COMPUTING;
		std::cout << "STUB: Computation start in t -" << unit_count << std::endl;
	}
	
	return 0;
}




void acc_function() {
	//debug stuff
	std::cout << "I dati che computo sono:" << std::endl;
	std::cout << " --> stato = " << acc_param->state << std::endl;
	std::cout << " --> param1 = " << acc_param->param1 << std::endl;
	std::cout << " --> param2 = " << acc_param->param2 << std::endl;
	std::cout << " --> risultato = " << acc_param->return_value << std::endl;
	
	
	//do the computation
	acc_param->return_value = acc_param->param1 + acc_param->param2;
	
	std::cout << "=> risultato = " << acc_param->return_value << std::endl;
	
	acc_param->state = DONE;
	unit_count = COMPUTATION_DELAY;
}








int main(int argc, char* argv[]) {

  // checking the argument passed at the simulator
  char* defaultargv[4];
  defaultargv[0] = const_cast<char *>("./simulatore");
  defaultargv[1] = const_cast<char *>("-f");
  defaultargv[2] = const_cast<char *>("or1ksim.cfg");
  defaultargv[3] = const_cast<char *>("vmlinux");
  
  if(argc=1){
	argc = 4;
	argv = defaultargv;
  }
  
  
  // allocate the mem for the parameter + state
  acc_param = (acc_parameter*) malloc(sizeof(struct acc_parameter));
  memset(acc_param, 0, sizeof(struct acc_parameter));
  
  
  
  
  if ( or1ksim_init(argc, argv, NULL, &my_read_function, &my_write_function) == OR1KSIM_RC_OK) {
 
 	//main simulator cycle   
  	while(1){
  		or1ksim_run(UNIT_DELAY);
 
  		if (acc_param->state == COMPUTING) {
  			unit_count--;
  			std::cout << "STUB: Computation start in t -" << unit_count << std::endl;
  			if (unit_count == 0) acc_function();
  		}
  	}
  } else {
  	std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }
  

  
}
