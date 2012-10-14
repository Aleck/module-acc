module-acc
==========


########## Modulo del device-driver ##########

Nella cartella module-build si trova l'occorrente per compilare il driver. Il mekefile presuppone che i sorgenti di linux siano presenti in ~/progetti/linux, se così non fosse è possibile configurare il Makefile con un altra directory.

- Per compilare il modulo eseguire make
- Per installare il modulo nel fs di linux eseguire make install
- Per il clean e uninstall del modulo eseguire make clean

Una volta installato il modulo, se si compila linux, l'eseguibile conterrà ed eseguirà all'avvio il modulo installato.

Nota: per compilare il modulo è necessario includere nel $PATH la toolchain per openrisc
      export PATH=/opt/openrisc/bin:$PATH




########## Simulatore #########

Nella cartella simulatore si trova l'occorrente per compilare ed eseguire il simulatore. I sorgenti per il makefile sono presenti nella cartella. 

- Per compilare il simulatore eseguire make
- Per eseguire il simulatore dare il comando ./simulatore -f <path1> <path2>, dove path1 e path2 sono i path relativi ai file or1ksim.cfg e 	vmlinux. Se viene dato solo il comando ./simulatore, il simulatore verrà correttamente eseguito solo se or1ksim.cfg e vmlinux sono 	    	presenti nella cartella corrente.
- Per uscire dal simulatore e il clean, ritornare sulla console del terminale e fare ctrl+c e poi q. La shell del simulatore si chiuderà     	 automaticamente. A questo punto eseguire make clean.


