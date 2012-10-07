#include <linux/module.h>	
#include <acc.h>
#include <linux/moduleparam.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/cdev.h>         /* cdev */
#include <linux/semaphore.h>	/* semafori */
#include <linux/fs.h>           /* almeno i numeri */
#include <linux/slab.h>		/* kmalloc() */







//******************************************** parte dichiarativa ************************************
// sbroffate in più che non fanno male
MODULE_AUTHOR("Alberto Lucchetti, Davide Gadioli");
MODULE_LICENSE("GPL");

// dichiaro i numeri del modulo
int acc_major = 0;    //ricordati che è solo una dichiarazione
int acc_minor = 0;    //ricordati che è solo una dichiarazione


//giusto per farlo parametrico
int numero_dispositivi = 1;
int grandezza_massima_scrivibile = HELLO_MAX_WRITABLE_SIZE;
int numero_accessi_simultantei = 1;


//il device vero e proprio
struct acc_dev *acc_device;




//dichiaro le operazioni consentite (per ora nessuna)
struct file_operations acc_fops = {
	.owner =    THIS_MODULE,
};




//****************************************************************************************************












//*************** parte che dice al kernel come usare questo casino che chiamano modulo **************
module_init(acc_init_module);
module_exit(acc_cleanup_module);

//definisco i parametri che possono essere cambiati a loadtime
module_param(grandezza_massima_scrivibile, int, S_IRUGO);
//*****************************************************************************************************

















//***************** funzioni che riguardano il modulo in se non le operazioni ***********************


// funzione che inizializza il nostro modulo
int acc_init_module(void)
{	
	//dichiarazione del dispositivo del modulo
	int result = 0;  //usato per ottenere il risultato delle nostre applicazioni
	dev_t dev = 0;  //usato per ottenere i numeri MAJOR e basta
	int device_number = 0; //il numero del device (maggioer + minore)
	
	
	
	//chiedo dinamicamente un device
	result = alloc_chrdev_region(&dev, acc_minor, numero_dispositivi, "dispositivo_acc");
	if (result < 0) {
	  printk(KERN_ALERT "Ho fallito ad ottenere un numero maggiore!\n");
	  return result;
	}
	
	//se non è fallita ottengo i numeri maggiori e minori
	acc_major = MAJOR(dev);
	acc_minor = MINOR(dev);
	
	//ora ottengo il numero del device totale (maggiore + totale)
	device_number = MKDEV(acc_major, acc_minor);
	
	
	//i numeri ora li posso mettere nel cassetto li utilizzo dopo
	
	
	//ora alloco veramente la memoria per il device
	acc_device = kmalloc( sizeof(struct acc_dev), GFP_KERNEL);
	if (!acc_device) {
	  gestisci_errore(ERR_ALLOC_FAIL);
	  return -1;
	}
	
	//scrivo tanti zeri per inizializzare la memoria, equivale a porlo uguale a zero
	memset(acc_device, 0, sizeof(struct acc_dev));
	
	
	//ora lo inizializzo
	
	//setto la grandezza dell'area da scrivere
	acc_device->size = grandezza_massima_scrivibile;
	//inizializzo il semaforo
	sema_init(&acc_device->sem, numero_accessi_simultantei);
	//inizializzo il cdev penso con tutto a null
	cdev_init(&acc_device->cdev, &acc_fops);
	//setto il propietario del cdev
	acc_device->cdev.owner = THIS_MODULE;
	//setto le operazioni consentite
	acc_device->cdev.ops = &acc_fops;	
	
	
	// ora aggiungo veramente il device usando il device number
	// NB: DA QUESTO MOMENTO TUTTI POSSONO FARCI SOPRA DELLE OPERAZIONI (se andato a buon fine)
	result = cdev_add(&acc_device->cdev, device_number, numero_dispositivi);
	
	if (result < 0) {
	  gestisci_errore(ERR_FAIL_ADD);
	  return result;
	}
	
	printk(KERN_INFO "acc: Modulo inizializzato correttamente\n");
	
	return 0;
}
 

 // funzione che pulisce il modulo
void acc_cleanup_module(void)
{
	
	dev_t devno = MKDEV(acc_major, acc_minor);
	
	if (acc_device) {
	  cdev_del(&acc_device->cdev);
	  kfree(acc_device);
	}
	
	unregister_chrdev_region(devno, numero_dispositivi);
	
	printk(KERN_INFO "acc: Modulo rimosso correttamente\n");
}


//se qualcosa non va penso che dovremmo fare qualcosa
void gestisci_errore(int errore) {
  
  //dico che c'è qualcosa che non va
  printk(KERN_ALERT "Oh mio dio es el pollo diablo!\n");
  
  
  //a seconda del caso lo gestisco differentemente
  switch (errore) {
      
    case ERR_ALLOC_FAIL:
      printk(KERN_ALERT "Non c'e' abbastanza memoria nel tuo baracco\n");
      break;    
      
    case ERR_FAIL_ADD:
      printk(KERN_ALERT "Non sono riusito ad aggiungere acc\n");
      break;
    
    default:
      printk(KERN_ALERT "PUOI INSULTARE LIBERAMENTE L'AUTORE DI QUESTO MODULO (defensive programming)\n");
  }
  
  // e per non sapere ne leggere ne scrivere pulisco il modulo
  acc_cleanup_module();
}








//****************************************************************************************************





