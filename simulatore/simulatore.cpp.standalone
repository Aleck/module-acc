#include <iostream>
#include "simulatore.h"
#include <or1ksim.h>


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
  
  if ( or1ksim_init(argc, argv, NULL, NULL, NULL) == OR1KSIM_RC_OK) {
    std::cout << "Riuscita inizializzazione simulatore" << std::endl;
    
    while(1){
    or1ksim_run(-1);
    }
  } else {
    std::cout << "Fallita inizializzazione simulatore" << std::endl;
  }  
}
