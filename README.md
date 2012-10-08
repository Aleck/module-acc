module-acc
==========


############ Cose da fare per realizzare il prototipo

- ampliare il modulo per fargli leggere e scrive un char
- capire come funziona il simulatore e provare il SoC
- fare il porting del modulo per oppenrisc (con Makefile incluso)
- attaccare al modulo la parte di comunicazione con l'acceleratore
- scrivere l'handler dell'interrupt
- test


############ Cose fatte

- scrivere un modulo che carica un device nel kernel.
- aggiungere al device la controparte in /dev/* e gestirla

############ Cose da fare nell'immediato futuro


- supportare almeno le operazioni di scrittura e lettura


############ Cose da sistemare

- nella funzione "acc_init_module(void)", le istruzioni "cdev_init(&acc_device->cdev, &acc_fops);" e "acc_device->cdev.ops = &acc_fops;" sembrano fare la stessa cosa, ciò non sembra creare problemi.

- nel codice i tipi "dev_t" e "int" sembrano possano essere usati in maniera intercambiabile, verificare che ciò non comporti dei side effect.

- non riesco a scrivere