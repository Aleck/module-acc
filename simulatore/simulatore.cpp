#include <iostream>
#include "simulatore.h"
#include <or1ksim.h>



int funione_lettura(void              *class_ptr,
                    unsigned long int  addr,
                    unsigned char      mask[],
                    unsigned char      rdata[],
                    int                data_len) {
  
  
  
  
  
}



int funione_scrittura(void              *class_ptr,
                      unsigned long int  addr,
                      unsigned char      mask[],
                      unsigned char      rdata[],
                      int                data_len) {
    
  
  
  
  
}








int main(int argc, char* argv[]) {
  
  std::cout << "Inizializzo il simulatore" << std::endl;
  
  if ( or1ksim_init(argc, argv, NULL, funione_lettura, funione_scrittura) == OR1KSIM_RC_OK) {
    std::cout << "Fallita inizializzazione simulatore" << std::endl;
  } else {
    std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }
  
  
  
  
}
