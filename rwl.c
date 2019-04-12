/*
 * Compile : gcc -g -Wall -o rwl rwl.c my_rand.c -lpthread
 * Run : ./rwl <Thread_count>
 *
 * */
#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include "my_rand.h"

#define MAX_RANDOM 65535

// Number of nodes in the linked list
int inserts_in_main_p = 0;

// Number of random operations in the linked list
int total_ops = 0;

// Number of threads to execute
int thread_count = 0;

// Fractions of each operation
double insert_percent = 0.0, delete_percent = 0.0, member_percent = 0.0;

// Total number of each operation
int member_total=0, insert_total=0, delete_total=0;

struct list_node_s *head = NULL;
pthread_rwlock_t rwlock;

//Node definition
struct list_node_s {
    int data;
    struct list_node_s *next;
};

int Insert(int value, struct list_node_s **head_pp);

int Delete(int value, struct list_node_s **head_pp);

int Member(int value, struct list_node_s *head_p);

void Free_list(struct list_node_s **head_pp);

void getInput(int* inserts_in_main_p);

void* Thread_work(void* rank);

double getTime();

int main(int argc, char *argv[]) {

    double start, finish;

    // Obtaining the inputs
    getInput(&inserts_in_main_p);
    thread_count = strtol(argv[1],NULL,10);

    // Calculating the total number od each operation
    insert_total = insert_percent * total_ops;
    member_total = member_percent * total_ops;
    delete_total = total_ops -(member_total + insert_total);

    //Initializing thread handlers
    pthread_t *thread_handlers;
    thread_handlers = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);

    // Linked List Generation with Random values
    int i = 0;
    while (i < inserts_in_main_p) {
        if (Insert(rand() % MAX_RANDOM, &head) == 1)
            i++;
    }

    //Initializing read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    start = getTime();

    long k = 0;
    //Thread create
    while (k < thread_count) {
      pthread_create(&thread_handlers[k], NULL, Thread_work, (void*) k);        
	k++;
    }

    // Thread Join
    k = 0;
    while (k < thread_count) {
        pthread_join(thread_handlers[k], NULL);
        k++;
    }

    finish = getTime();
    
    //destroy read-write lock
    pthread_rwlock_destroy(&rwlock);

    printf("Linked List with read write locks Time Spent : %.6f\n", finish-start);

    Free_list(&head);

    return 0;
}


// Linked List Membership function
int Member(int value, struct list_node_s *head_p) {
    struct list_node_s *current_p = head_p;

    while (current_p != NULL && current_p->data < value)
        current_p = current_p->next;

    if (current_p == NULL || current_p->data > value) {
        return 0;
    }
    else {
        return 1;
    }

}

// Linked List Insertion function
int Insert(int value, struct list_node_s **head_pp) {
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;

        return 1;
    }
    else
        return 0;
}

// Linked List Deletion function
int Delete(int value, struct list_node_s **head_pp) {
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }

        return 1;

    }
    else
        return 0;
}

//Linked List Free Item function
void Free_list(struct list_node_s **head_pp) {
   struct list_node_s *current;
   struct list_node_s *following;

   //if (Is_empty()) return;
   current = *head_pp; 
   following = current->next;
   while (following != NULL) {

      //printf("Freeing %d\n", current->data);
      free(current);
      current = following;
      following = current->next;
   }

   //printf("Freeingss %d\n", current->data);

   free(current);
} 

//Get Input from user function
void getInput(int* inserts_in_main_p) {

	printf("How many keys should be inserted in the main thread (n)?\n");
   	scanf("%d", inserts_in_main_p);

   	printf("How many ops total should be executed (m)?\n");
   	scanf("%d", &total_ops);

   	printf("Percent of ops that should be searches? (between 0 and 1)\n");
   	scanf("%lf", &member_percent);

   	printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   	scanf("%lf", &insert_percent);

   	delete_percent = 1.0 - (member_percent + insert_percent);
}

// Thread Operations
void *Thread_work(void *rank) {

    long my_rank = (long)rank+1;
   
    //Local variables to store the number of opoerations each thread should perform
    int my_total_ops = 0;
    int my_insert_count = 0;
    int my_delete_count = 0;
    int my_member_count = 0;

    int count_tot = 0;
    int count_member = 0;
    int count_insert = 0;
    int count_delete = 0;
   
    //Local variable to indicate whether the thread is finished performing operations
    int finished_member = 0;
    int finished_insert = 0;
    int delete_finished = 0;

    unsigned seed = time(NULL);

    // Generate local no of insertion operations without loss
    if (insert_total % thread_count == 0 || insert_total % thread_count < my_rank) {
        my_insert_count = insert_total / thread_count;
    }
    else if (insert_total % thread_count >= my_rank) {
        my_insert_count = insert_total / thread_count + 1;
    }

    // Generate local no of member operations without loss
    if (member_total % thread_count == 0 || member_total % thread_count < my_rank) {
        my_member_count = member_total / thread_count;
    }
    else if (member_total % thread_count >= my_rank) {
        my_member_count = member_total / thread_count + 1;
    }

    // Generate local no of deletion operations without loss
    if (delete_total % thread_count == 0 || delete_total % thread_count < my_rank) {
        my_delete_count = delete_total / thread_count;
    }
    else if (delete_total % thread_count >= my_rank) {
        my_delete_count = delete_total / thread_count + 1;
    }
    my_total_ops = my_insert_count + my_delete_count + my_member_count;

    while (count_tot < my_total_ops) {

        int random_value = my_rand(&seed) % MAX_RANDOM;
        int which_op = my_rand(&seed) % 3;

        // Member operation
        if (which_op == 0 && finished_member == 0) {
            if (count_member < my_member_count) {
                pthread_rwlock_rdlock(&rwlock);
                Member(random_value, head);
                pthread_rwlock_unlock(&rwlock);
                count_member++;
            } else
                finished_member = 1;
        }

            // Insert Operation
        else if (which_op == 1 && finished_insert == 0) {
            if (count_insert < my_insert_count) {
                pthread_rwlock_wrlock(&rwlock);
                Insert(random_value, &head);
                pthread_rwlock_unlock(&rwlock);
                count_insert++;
            } else
                finished_insert = 1;
        }

            // Delete Operation
        else if (which_op == 2 && delete_finished == 0) {

            if (count_delete < my_delete_count) {
                pthread_rwlock_wrlock(&rwlock);
                Delete(random_value, &head);
                pthread_rwlock_unlock(&rwlock);
                count_delete++;
            } else
                delete_finished = 1;
        }
        count_tot = count_insert + count_member + count_delete;
 
    }
    return NULL;
}

//Get time function
double getTime() {
   struct timeval t;
   gettimeofday(&t, NULL);
   return (double)t.tv_sec + t.tv_usec/1000000.0;
}
