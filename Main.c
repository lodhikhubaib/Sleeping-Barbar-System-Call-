#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define MAX_CHAIRS 5 // maximum number of chairs in the waiting room

// global variables
static struct task_struct *barber_task;
static struct task_struct *customer_task;
static struct mutex waiting_room_mutex;
static int num_customers_waiting = 0;

// function to simulate the barber's behavior
static int barber_function(void *arg) {
    while (!kthread_should_stop()) {
        mutex_lock(&waiting_room_mutex);
        if (num_customers_waiting == 0) {
            mutex_unlock(&waiting_room_mutex);
            msleep(100);
            continue;
        }
        num_customers_waiting--;
        mutex_unlock(&waiting_room_mutex);
        printk(KERN_INFO "Barber cutting hair...\n");
        msleep(500);
    }
    return 0;
}

// function to simulate a customer's behavior
static int customer_function(void *arg) {
    while (!kthread_should_stop()) {
        mutex_lock(&waiting_room_mutex);
        if (num_customers_waiting < MAX_CHAIRS) {
            num_customers_waiting++;
            mutex_unlock(&waiting_room_mutex);
            printk(KERN_INFO "Customer taking a seat...\n");
            msleep(500);
        } else {
            mutex_unlock(&waiting_room_mutex);
            printk(KERN_INFO "No chairs available, customer leaving...\n");
            break;
        }
    }
    return 0;
}

// system call function for the sleeping barber problem
asmlinkage long sys_sleeping_barber(void) {
    mutex_init(&waiting_room_mutex);
    barber_task = kthread_create(barber_function, NULL, "barber");
    customer_task = kthread_create(customer_function, NULL, "customer");
    wake_up_process(barber_task);
    wake_up_process(customer_task);
    msleep(3000); // simulation time
    kthread_stop(customer_task);
    kthread_stop(barber_task);
    return 0;
}
