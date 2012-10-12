#include <iostream>
#include "simulatore.h"
#include <or1ksim.h>



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
                      unsigned char      rdata[],
                      int                data_len) {
    
  
  
  
  return 0;
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
  
  if ( or1ksim_init(argc, argv, NULL, NULL, NULL) == OR1KSIM_RC_OK) {
    std::cout << "Riuscita inizializzazione simulatore" << std::endl;
    
    while(1){
    or1ksim_run(-1);
  }
  
    
    
  } else {
    std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }
  

  //simulazione acceleratore acc d'esempio
  


  
}
