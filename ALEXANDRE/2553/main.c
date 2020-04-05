/* Sample Kernel module with /dev interface
*   NOTE : the /dev entry must be created before loading this module with the command
*       mknod /dev/caesar c 60 0
*   executed as root.
* This program is GPL.
*/

#define DEV_NAME "caesar"

/** Includes **/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> // printk
#include <linux/fs.h>
#include <linux/uaccess.h> /* copy_from/to_user */

/** Informations du driver **/
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Caesar Cipher module with /dev interface");
MODULE_AUTHOR("Alexandre SANNIER <alexandre.sannier@groupe-esigelec.org > - Arthur PERRIN < arthur.perrin@groupe-esigelec.org>");
MODULE_SUPPORTED_DEVICE(DEV_NAME);

static int major = 61;
static int key = 0;
static char encryptedMsg = 0;

/* Prototypes */
static ssize_t caesarRead(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t caesarOpen(struct inode *inode, struct file *file);
static ssize_t caesarRelease(struct inode *inode, struct file *file);
static ssize_t caesarWrite(struct file *file, const char *buf, size_t count, loff_t *ppos);

static struct file_operations caesar_ops=
{
    .read = caesarRead,
    .open = caesarOpen,
    .release = caesarRelease,
    .write =caesarWrite
};


/*
 * Fonctions lancées au chargement / déchargement du module
 */
static int __init caesar_init(void)
{
    printk(KERN_ALERT "caesar Loading v2\n");

    register_chrdev(major,DEV_NAME,&caesar_ops);

    printk(KERN_ALERT "MAJOR = %d\n" , major);

    return 0;
};

static void __exit caesar_exit(void)
{
    printk(KERN_ALERT "caesar Unloading ...\n");
    unregister_chrdev(major,DEV_NAME);
};

// Fait connaitre ces fonctions au noyau
module_init(caesar_init);
module_exit(caesar_exit);

/*
 * Fonctions lancées lors des accès au fichier /dev/caesar
 */
// Lecture du fichier
static ssize_t caesarRead(struct file *file, char *buf, size_t count, loff_t *ppos)
{

   char mesg[] = " \n";
    mesg[0] = encryptedMsg + key;
   printk(KERN_ALERT "read()\n");
    if ( mesg[(*ppos)] != '\0' ){

      copy_to_user( buf , (mesg+(*ppos)) , 1 );
      *ppos += 1;
      return 1;
    }else{
      return 0;
    }

}

static ssize_t caesarOpen(struct inode *inode, struct file *file)
{

        printk(KERN_ALERT "open()\n");
    return 0;

}

static ssize_t caesarRelease(struct inode *inode, struct file *file)
{
        printk(KERN_ALERT "release()\n");
    return 0;
}

static ssize_t caesarWrite(struct file *file, const char *buf, size_t count, loff_t *pposin)
{
char var_recue[]= " ";

copy_from_user(var_recue + *pposin, buf ,count);

printk(KERN_ALERT "write()\n");



    if (*pposin==0 && ( var_recue[(*pposin)] >= '0' && var_recue[(*pposin)] <= '9')){
        key = var_recue[0] - 48;
        printk(KERN_ALERT "Key added %d \n",key);

    }

    if ( var_recue[(*pposin)]  >= 'A' && var_recue[(*pposin)] <= 'Z'){
        encryptedMsg = var_recue[(*pposin)] ;
    }
    if ( var_recue[(*pposin)]  >= 'a' && var_recue[(*pposin)] <= 'z'){
            encryptedMsg = var_recue[(*pposin)] ;
        }

        return count;

}
