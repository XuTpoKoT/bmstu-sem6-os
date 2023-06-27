#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h>  
#include <linux/vmalloc.h>
#include <linux/proc_fs.h> 
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Morozov Dmitry");

#define MAX_BUF_SIZE PAGE_SIZE

#define DIRNAME "fortune_dir"
#define FILENAME "fortune"
#define SYMLINK "fortune_link"
#define FILEPATH DIRNAME "/" FILENAME

static struct proc_dir_entry *fortune_dir = NULL;
static struct proc_dir_entry *fortune_file = NULL;
static struct proc_dir_entry *fortune_link = NULL;

static char *cookie_buffer;
static int write_index;
static int read_index;

static char tmp[MAX_BUF_SIZE];

ssize_t fortune_read(struct file *file_, char __user *buf, size_t count, loff_t *offp) {
    printk("+: called read\n");
    if (*offp > 0 || !write_index) {
        printk("+: offp case");
        return 0;
    }
    if (read_index >= write_index)
        read_index = 0;
    int len = snprintf(tmp, MAX_BUF_SIZE, "%s\n", &cookie_buffer[read_index]);
    if (copy_to_user(buf, tmp, len)) {
        printk(KERN_ERR "+: copy_to_user error\n");
        return -EFAULT;
    }
    read_index += len;
    *offp += len;
    return len;
}

ssize_t fortune_write(struct file *file_, const char __user *buf, size_t len, loff_t *offp) {
    printk(KERN_INFO "+: called write\n");
    if (len > MAX_BUF_SIZE - write_index +1) {
        printk(KERN_ERR "+: cookie_buffer overflow error\n");
        return -1;
    }
    if (copy_from_user(&cookie_buffer[write_index], buf, len)) {
        printk(KERN_ERR "+: copy_to_user error\n");
        return -1;
    }
    write_index += len;
    cookie_buffer[write_index - 1] = '\0';
    return len;
}

int fortune_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "+: called open\n");
    return 0;
}

int fortune_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "+: called release\n");
    return 0;
}

static const struct proc_ops fops = {
    proc_read: fortune_read, 
    proc_write: fortune_write, 
    proc_open: fortune_open, 
    proc_release: fortune_release
};

static void freemem(void) {
    if (fortune_link)
        remove_proc_entry(SYMLINK, NULL);
    if (fortune_file)
        remove_proc_entry(FILENAME, fortune_dir);
    if (fortune_dir)
        remove_proc_entry(DIRNAME, NULL);
    if (cookie_buffer)
        vfree(cookie_buffer);
}

static int __init fortune_init(void) {
    if (!(cookie_buffer = vmalloc(MAX_BUF_SIZE))) {
        freemem();
        printk(KERN_ERR "+: error during vmalloc\n");
        return -1;
    }
    memset(cookie_buffer, 0, MAX_BUF_SIZE);

    if (!(fortune_dir = proc_mkdir(DIRNAME, NULL))) {
        freemem();
        printk(KERN_ERR "+: error during directory creation\n");
        return -1;
    }
    if (!(fortune_file = proc_create(FILENAME, 0666, fortune_dir, &fops))) {
        freemem();
        printk(KERN_ERR "+: error during file creation\n");
        return -1;
    } 
    if (!(fortune_link = proc_symlink(SYMLINK, NULL, FILEPATH))) {
        freemem();
        printk(KERN_ERR "+: error during symlink creation\n");
        return -1;
    }

    write_index = 0;
    read_index = 0;

    printk(KERN_INFO "+: module loaded\n");
    return 0;
}

static void __exit fortune_exit(void) {
    freemem();
    printk(KERN_INFO "+: module unloaded\n");
}

module_init(fortune_init)
module_exit(fortune_exit)


