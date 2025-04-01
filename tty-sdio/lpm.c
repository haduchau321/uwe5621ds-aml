#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/param.h>
#include <linux/bitops.h>
#include <linux/gpio.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/pm_wakeup.h>
#include <linux/export.h>
#include <marlin_platform.h>

#define VERSION         "marlin2 V0.1"
#define PROC_DIR        "bluetooth/sleep"

#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        1
#endif

struct proc_dir_entry *bluetooth_dir, *sleep_dir;
static struct wakeup_source *tx_ws;
static struct wakeup_source *rx_ws;

void host_wakeup_bt(void)
{
    __pm_stay_awake(tx_ws);
    marlin_set_sleep(MARLIN_BLUETOOTH, FALSE);
    marlin_set_wakeup(MARLIN_BLUETOOTH);
}

void bt_wakeup_host(void)
{
    __pm_relax(tx_ws);
    __pm_wakeup_event(rx_ws, 5000); // 5 seconds timeout
}

static ssize_t bluesleep_write_proc_btwrite(struct file *file,
    const char __user *buffer, size_t count, loff_t *pos)
{
    char b;

    if (count < 1)
        return -EINVAL;
    if (copy_from_user(&b, buffer, 1))
        return -EFAULT;
    pr_info("bluesleep_write_proc_btwrite=%d\n", b);
    if (b == '1')
        host_wakeup_bt();
    else if (b == '2') {
        marlin_set_sleep(MARLIN_BLUETOOTH, TRUE);
        __pm_relax(tx_ws);
    } else
        pr_err("bludroid pass a unsupported parameter");
    return count;
}

static int btwrite_proc_show(struct seq_file *m, void *v)
{
    pr_info("bluesleep_read_proc_lpm\n");
    seq_puts(m, "unsupported to read\n");
    return 0;
}

static int bluesleep_open_proc_btwrite(struct inode *inode, struct file *file)
{
    return single_open(file, btwrite_proc_show, PDE_DATA(inode));
}

static const struct file_operations lpm_proc_btwrite_fops = {
    .owner = THIS_MODULE,
    .open = bluesleep_open_proc_btwrite,
    .read = seq_read,
    .write = bluesleep_write_proc_btwrite,
    .release = single_release,
};

int bluesleep_init(void)
{
    int retval;
    struct proc_dir_entry *ent;

    bluetooth_dir = proc_mkdir("bluetooth", NULL);
    if (bluetooth_dir == NULL) {
        pr_info("Unable to create /proc/bluetooth directory");
        remove_proc_entry("bluetooth", NULL);
        return -ENOMEM;
    }
    sleep_dir = proc_mkdir("sleep", bluetooth_dir);
    if (sleep_dir == NULL) {
        pr_info("Unable to create /proc/%s directory", PROC_DIR);
        remove_proc_entry("bluetooth", NULL);
        return -ENOMEM;
    }

    ent = proc_create("btwrite", S_IRUGO | S_IWUSR | S_IWGRP, sleep_dir,
        &lpm_proc_btwrite_fops);
    if (ent == NULL) {
        pr_info("Unable to create /proc/%s/btwake entry", PROC_DIR);
        retval = -ENOMEM;
        goto fail;
    }

    tx_ws = wakeup_source_register(NULL, "BT_TX_wakelock");
    if (!tx_ws) {
        retval = -ENOMEM;
        goto fail;
    }

    rx_ws = wakeup_source_register(NULL, "BT_RX_wakelock");
    if (!rx_ws) {
        wakeup_source_unregister(tx_ws);
        retval = -ENOMEM;
        goto fail;
    }

    return 0;

fail:
    remove_proc_entry("btwrite", sleep_dir);
    remove_proc_entry("sleep", bluetooth_dir);
    remove_proc_entry("bluetooth", NULL);
    if (tx_ws)
        wakeup_source_unregister(tx_ws);
    if (rx_ws)
        wakeup_source_unregister(rx_ws);
    return retval;
}

void bluesleep_exit(void)
{
    remove_proc_entry("btwrite", sleep_dir);
    remove_proc_entry("sleep", bluetooth_dir);
    remove_proc_entry("bluetooth", NULL);
    wakeup_source_unregister(tx_ws);
    wakeup_source_unregister(rx_ws);
}

MODULE_DESCRIPTION("Bluetooth Sleep Mode Driver ver %s " VERSION);
MODULE_LICENSE("GPL");


