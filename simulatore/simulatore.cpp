#include <iostream>
#include "simulatore.h"
#include <or1ksim.h>



int funione_lettura(void              *class_ptr,
                    unsigned long int  addr,
                    unsigned char      mask[],
                    unsigned char      rdata[],
                    int                data_len) {
  
  
  
  return 0;
}



int funione_scrittura(void              *class_ptr,
                      unsigned long int  addr,
                      unsigned char      mask[],
                      unsigned char      rdata[],
                      int                data_len) {
    
  
  
  
  return 0;
}








int main(int argc, char* argv[]) {
  
  std::cout << "Inizializzo il simulatore" << std::endl;
  
  if ( or1ksim_init(argc, argv, NULL, NULL, NULL) == OR1KSIM_RC_OK) {
    std::cout << "Riuscita inizializzazione simulatore" << std::endl;
    
    
    or1ksim_run(-1);
    
    
  } else {
    std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }
  
  
}
