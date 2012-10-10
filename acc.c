#include <linux/module.h>	
#include "acc.h"
#include <linux/moduleparam.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/cdev.h>         /* cdev */
#include <linux/semaphore.h>	/* semafori */
#include <linux/fs.h>           /* almeno i numeri */
#include <linux/slab.h>		/* kmalloc() */


#include <asm/uaccess.h>        /* per letture e scritture */






//******************************************** parte dichiarativa ************************************
// sbroffate in più che non fanno male
MODULE_AUTHOR("Alberto Lucchetti, Davide Gadioli");
MODULE_LICENSE("GPL");

// dichiaro i numeri del modulo
int acc_major = 0;    //ricordati che è solo una dichiarazione
int acc_minor = 0;    //ricordati che è solo una dichiarazione


//giusto per farlo parametrico
int numero_dispositivi = 1;
int grandezza_massima_scrivibile = ACC_MAX_WRITABLE_SIZE;
int numero_accessi_simultanei = 1;


//il device vero e proprio
struct acc_dev *acc_device;




//dichiaro le operazioni consentite (per ora nessuna)
struct file_operations acc_fops = {
	.owner =    THIS_MODULE,
	.read  =     acc_read,
	.write =     acc_write,
	.open  =     acc_open,
};




//****************************************************************************************************





//*************** parte che dice al kernel come usare questo casino che chiamano modulo **************
module_init(acc_init_module);
module_exit(acc_cleanup_module);

//definisco i parametri che possono essere cambiati a loadtime
module_param(grandezza_massima_scrivibile, int, S_IRUGO);
//*****************************************************************************************************



//********************************** oerazioni sul device ********************************************


int acc_trim(struct acc_dev *dev) {
  
  if (dev->data) {
    kfree(dev->data);
  }
  
  dev->data = NULL;
  
  return 0;
}


int acc_open (struct inode *inode, struct file *filp) {
  
 //prendo le informazioni del device che ha chiesto l'apertura
 struct acc_dev *dev;
 dev = container_of(inode->i_cdev, struct acc_dev, cdev);
 
 //lo azzero solo se è stato aperto come WRITE_ONLY
 if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
   if (down_interruptible(&dev->sem)) {
     return -ERESTARTSYS;
   }
   acc_trim(dev);
   up(&dev->sem);
 }
   
 /* and use filp->private_data to point to the device data */
 filp->private_data = dev;
 return 0;
}



int acc_release (struct inode *inode, struct file *filp) {
  return 0;
}


ssize_t acc_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
  
  struct acc_dev *dev = filp->private_data; /* the first listitem */
  ssize_t retval = 0;
  
  
  //ottengo l'accesso esclusivo
  if (down_interruptible(&dev->sem)) {
    return -ERESTARTSYS;
  }
  
  //se non ci sono dati ritorno subito
  if (!dev->data) {
    up(&dev->sem);
    return retval;
  }
  
  //non è intelligente mettere un offset > della grandezza dei dati
  if (*f_pos > dev->size) {
    up(&dev->sem);
    return retval;
  }
  
  //tronco quanti dati posso leggere
  if (*f_pos + count > dev->size)
    count = dev->size - *f_pos;
  
  //la lettura vera e propria
  if (copy_to_user(buf, dev->data + *f_pos, count)) {
    retval = -EFAULT;
    up(&dev->sem);
    return retval;
  }
  
  //ho finito la copia
  up(&dev->sem);
  *f_pos += count;
  return count;
  
}



ssize_t acc_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
  
  struct acc_dev *dev = filp->private_data; /* the first listitem */
  ssize_t retval = -ENOMEM; /* our most likely error */
  
  //ottengo l'accesso esclusivo
  if (down_interruptible(&dev->sem)) {
    return -ERESTARTSYS;
  }
  
  //controllo che il count è adeguato
  if (count > ACC_MAX_WRITABLE_SIZE) {
    up(&dev->sem);
    return retval;
  }
  
  
  if (*f_pos + count > ACC_MAX_WRITABLE_SIZE)
    count = ACC_MAX_WRITABLE_SIZE - *f_pos;
  
  
  //controlliamo la prima scrittura
  if (!dev->data) {
    dev->data = kmalloc(sizeof(void*), GFP_KERNEL);
    if (!dev->data) {
      up(&dev->sem);
      return retval;
    }
  }
  
  
  //scriviamo gli zeri nella memoria
  memset(dev->data, 0, ACC_MAX_WRITABLE_SIZE);
  
  //ora scriviamo i dati
  if (copy_from_user(dev->data + *f_pos, buf, count)) {
    retval = -EFAULT;
    up(&dev->sem);
    return retval;
  }
  
  //aggiorno la grandezza
  *f_pos += count;
  if (dev->size < *f_pos) {
    dev->size = *f_pos;
  }
  
  
  up(&dev->sem);
  return count;
}




//*****************************************************************************************************










//***************** funzioni che riguardano il modulo in se non le operazioni ***********************

// funzione che inizializza il nostro modulo
int acc_init_module(void)
{	
	//dichiarazione del dispositivo del modulo
	int result = 0;  //usato per ottenere il risultato delle nostre applicazioni
	dev_t device_number = 0;  //usato per ottenere i numeri MAJOR e basta
	
	
	
	//chiedo dinamicamente un device
	result = alloc_chrdev_region(&device_number, acc_minor, numero_dispositivi, "acc");
	if (result < 0) {
	  printk(KERN_ALERT "Ho fallito ad ottenere un numero maggiore!\n");
	  return result;
	}
	
	//se non è fallita ottengo i numeri maggiori e minori
	acc_major = MAJOR(device_number);
	acc_minor = MINOR(device_number);
	
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
	acc_device->size = 0;
	//inizializzo il semaforo
	sema_init(&acc_device->sem, numero_accessi_simultanei);
	//inizializzo il cdev e gli attacca le operazioni
	cdev_init(&acc_device->cdev, &acc_fops);
	//setto il propietario del cdev
	acc_device->cdev.owner = THIS_MODULE;
	
	
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





