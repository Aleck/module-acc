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




########## Simulatore ##########