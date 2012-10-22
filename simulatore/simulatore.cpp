#include <iostream>
#include <string.h>
#include <or1ksim.h>
#include <stdlib.h>
#include <stdio.h>

#define BASE_ADDR  0x30000000



// state value
#define READY 0
#define COMPUTING 1
#define DONE 2


//the computation delay 
#define COMPUTATION_DELAY 2
#define UNIT_DELAY 0.5 //(in ms)




//the struct used by the module
struct acc_parameter {
	int state;
	int return_value;
	int param1;
	int param2;
};




int param_size = sizeof(struct acc_parameter);
int size_counter = 0;
struct acc_parameter* acc_param;

//the computation tick delay ( 1 tick = 100ms)
int unit_count = COMPUTATION_DELAY;






void acc_function() {
	//debug stuff
	std::cout << "******** let's do my job ********" << std::endl;
	std::cout << "My data are:" << std::endl;
	std::cout << " --> state = " << acc_param->state << std::endl;
	std::cout << " --> param1 = " << acc_param->param1 << std::endl;
	std::cout << " --> param2 = " << acc_param->param2 << std::endl;
	std::cout << " --> return_value = " << acc_param->return_value << std::endl;
	
	
	//do the computation
	acc_param->return_value = acc_param->param1 + acc_param->param2;
	
	std::cout << "=> i got " << acc_param->return_value << std::endl;
	
	acc_param->state = DONE;
	unit_count = COMPUTATION_DELAY;
	size_counter = 0;
}







int my_read_function(void              *class_ptr,
                    unsigned long int  addr,
                    unsigned char      mask[],
                    unsigned char      rdata[],
                    int                data_len) {
	std::cout << "************ read started *************" << std::endl;
	// get the offset
        unsigned long int offset = addr - BASE_ADDR;
        unsigned char read_element = 0;
        
        // set the data_len
        data_len = 4;
        
	switch(offset) {
		case 0:
			std::cout << "STUB: read at offset  " << offset << " so it's the state" << std::endl;
			std::cout << "STUB: i want send " << acc_param->state << std::endl;
			for(int index = 0; index < data_len; index++) {
				read_element = acc_param->state >> index*8;
				std::cout << "STUB: the element " << index << " is " << (int)read_element << std::endl;
				rdata[index] = (int)read_element;
        		}
        		break;
        	case 4:
			std::cout << "STUB: read at offset  " << offset << " so it's the return_value" << std::endl;
			std::cout << "STUB: i want send " << acc_param->return_value << std::endl;
			for(int index = 0; index < data_len; index++) {
				read_element = acc_param->return_value >> index*8;
				std::cout << "STUB: the element " << index << " is " << (int)read_element << std::endl;
				rdata[index] = (int)read_element;
        		}
        		
        		acc_param->state = READY;
        		
        		break;
		default:
			std::cout << "Unreachable statement" << std::endl;
	}
	std::cout << "STUB: send char " << (int) rdata[0] << " " << (int) rdata[1] << " " <<  (int) rdata[2] << " " << (int) rdata[3] << std::endl;
	std::cout << "************ read done *************" << std::endl;
	return 0;
}



int my_write_function(void              *class_ptr,
                      unsigned long int  addr,
                      unsigned char      mask[],
                      unsigned char      wdata[],
                      int                data_len) {
                      
                      
        std::cout << "************ write started *************" << std::endl;
        
        // get the offset
        unsigned long int offset = addr - BASE_ADDR;
        
        
        std::cout << "STUB: received char " << (int) wdata[0] << " " << (int) wdata[1] << " " <<  (int) wdata[2] << " " << (int) wdata[3] << std::endl;
        switch(offset) {
        	case 0:
        		std::cout << "STUB: at offset  " << offset << " so it's the state" << std::endl;
        		acc_param->state = 0;
        		for(int index = 0; index < data_len; index++) {
        			acc_param->return_value += (int)wdata[data_len - (index + 1)] << index*8;
        		}
        		break;
        	case 4:
        		std::cout << "STUB: at offset  " << offset << " so it's return_value" << std::endl;
        		acc_param->return_value = 0;
        		for(int index = 0; index < data_len; index++) {
        			acc_param->return_value += (int)wdata[data_len - (index + 1)] << index*8;
        		}
        		break;
        	case 8:
        		std::cout << "STUB: at offset  " << offset << " so it's param1" << std::endl;
        		acc_param->param1 = 0;
        		for(int index = 0; index < data_len; index++) {
        			acc_param->param1 += (int)wdata[data_len - (index + 1)] << index*8;
        		}
        		break;
        	case 12:
        		std::cout << "STUB: at offset  " << offset << " so it's param2" << std::endl;
        		acc_param->param2 = 0;
        		for(int index = 0; index < data_len; index++) {
        			acc_param->param2 += (int)wdata[data_len - (index + 1)] << index*8;
        		}
        		break;
        	default:
        		std::cout << "Unreachable statement" << std::endl;
        }
        
        //update the size counter
        size_counter += data_len;
        
	//print the written memory
	std::cout << "STUB: Received  " << size_counter << " byte over " << param_size << std::endl;
	
	std::cout << "My new data are:" << std::endl;
	std::cout << " --> state = " << acc_param->state << std::endl;
	std::cout << " --> param1 = " << acc_param->param1 << std::endl;
	std::cout << " --> param2 = " << acc_param->param2 << std::endl;
	std::cout << " --> return_value = " << acc_param->return_value << std::endl;
	
	// when the transmission is done, start the computation
	if (size_counter == param_size) {
		acc_param->state = COMPUTING;
		std::cout << "STUB: Computation start in t -" << unit_count << std::endl;
	}
	
	return 0;
}



void interrupt_function() {
	std::cout << "****** activate the interrupt *******" << std::endl;
	acc_param->state = READY;
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
  			if (unit_count <= 0) acc_function();
  				else std::cout << "STUB: Computation start in t -" << unit_count << std::endl;
  		}
  		
  	}
  } else {
  	std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }
  

  
}
