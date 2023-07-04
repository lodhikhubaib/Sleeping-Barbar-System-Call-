#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#define sys_sleeping_barber 336 //this is the number where we have added the system call.
#define MAX_CHAIRS 5  //Total Number of chairs for customer

pthread_mutex_t waiting_room_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_ready_cond = PTHREAD_COND_INITIALIZER;

int num_customers_waiting = 0;
int barber_ready = 0;

static void* barber_function(void *arg) {
    while (1) {
        pthread_mutex_lock(&waiting_room_mutex);
        if (num_customers_waiting == 0) {
            pthread_cond_wait(&barber_ready_cond, &waiting_room_mutex);
        }
        num_customers_waiting--;
        pthread_mutex_unlock(&waiting_room_mutex);
        printf("Barber cutting hair...\n");
        sleep(5);
    }
    return NULL;
}

static void* customer_function(void *arg) {
    while (1) {
        pthread_mutex_lock(&waiting_room_mutex);
        if (num_customers_waiting < MAX_CHAIRS) {
            num_customers_waiting++;
            if (barber_ready == 0) {
                barber_ready = 1;
                pthread_cond_signal(&barber_ready_cond);
            }
            pthread_mutex_unlock(&waiting_room_mutex);
            printf("Customer taking a seat...\n");
            sleep(2);
        } else {
            pthread_mutex_unlock(&waiting_room_mutex);
            printf("No chairs available, customer leaving...\n");
            break;
        }
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    syscall(sys_sleeping_barber);//for calling the system call
    pthread_create(&thread1, NULL, barber_function, NULL);
    pthread_create(&thread2, NULL, customer_function, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
