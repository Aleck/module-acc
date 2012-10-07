#ifndef ACC_HEADER
#define ACC_HEADER

//import necessari
#include <linux/cdev.h>
#include <linux/semaphore.h>



//per un minimo di riconoscimento degli errori
#ifndef ERR_ALLOC_FAIL
#define ERR_ALLOC_FAIL  1
#define ERR_FAIL_ADD    2
#endif



//per settare la grandezza di quanto posso leggere e scrivere (cosa molto stupida)
#ifndef HELLO_MAX_WRITABLE_SIZE
#define HELLO_MAX_WRITABLE_SIZE 100
#endif







//dichiaro le strutture necessarie
struct acc_dev {
	void *data;               /* Puntatore al dato            */
	int size;                 /* la dimensione del dato       */
	struct semaphore sem;     /* mutual exclusion semaphore   */
	struct cdev cdev;	  /* Char device structure	  */
};






//dichiaro i prototipi delle funzioni
void gestisci_errore(int errore);







//dichiaro i prototipi delle funzioni legate ai moduli
int acc_init_module(void);
void acc_cleanup_module(void);




#endif