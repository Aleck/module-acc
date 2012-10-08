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
#ifndef ACC_MAX_WRITABLE_SIZE
#define ACC_MAX_WRITABLE_SIZE 100
#endif







//dichiaro le strutture necessarie
struct acc_dev {
	void *data;               /* Puntatore al dato              */
	int size;                 /* la dimensione del dato corrente*/
	struct semaphore sem;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure	    */
};






//dichiaro i prototipi delle funzioni
void gestisci_errore(int errore);







//dichiaro i prototipi delle funzioni legate ai moduli
int acc_init_module(void);
void acc_cleanup_module(void);
int acc_open (struct inode *inode, struct file *filp);
int acc_trim(struct acc_dev *dev);
int acc_release (struct inode *inode, struct file *filp);
ssize_t acc_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t acc_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);




#endif