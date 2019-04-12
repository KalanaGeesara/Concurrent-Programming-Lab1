/*
 * Compile : gcc -g -Wall -o serial serial.c my_rand.c
 * Run : ./serial
 *
 * */
#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include<time.h> 
#include "my_rand.h"

#define MAX_RANDOM 65535

// Number of nodes in the linked list
int inserts_in_main_p = 0;

// Number of random operations in the linked list
int total_ops = 0;

// Fractions of each operation
double member_percent, insert_percent, delete_percent;

struct list_node_s *head = NULL;

//Node definition
struct list_node_s {
    int data;
    struct list_node_s *next;
};

int Insert(int value, struct list_node_s **head_pp);

int Delete(int value, struct list_node_s **head_pp);

int Member(int value, struct list_node_s *head_p);

void getInput(int* inserts_in_main_p);

void Free_list(struct list_node_s **head_pp);

double getTime();

int main(int argc, char *argv[]) {

    
    //struct timeval time_begin, time_end;
    int inserts_in_main_p, rand_value;
    double start, finish, which_op;
    unsigned seed = time(NULL);

    //Getting the inputs
    getInput(&inserts_in_main_p);

    //Linked List Generation with Random values
    int i = 0;
    while (i < inserts_in_main_p) {
        if (Insert(my_rand(&seed) % MAX_RANDOM, &head) == 1)
            i++;
    }
	
    //Operations in the linked list
    float insert_num = insert_percent* total_ops;
    float member_num = member_percent * total_ops;
    float delete_num = total_ops -(member_num + insert_num);

    int count_member = 0;
    int count_insert = 0;
    int count_delete = 0;
    int count_tot = 0;


    //Get Start time
    start = getTime();

    while (count_tot < total_ops) {

        rand_value = my_rand(&seed) % MAX_RANDOM;
        which_op = my_rand(&seed) % 3;

        if (which_op == 0 && count_member < member_num) {
            Member(rand_value, head);
            count_member++;
        }

        else if (which_op == 1 && count_insert < insert_num) {
            Insert(rand_value, &head);
            count_insert++;
        }

        else if (which_op == 2 && count_delete < delete_num) {
            Delete(rand_value, &head);
            count_delete++;
        }

        count_tot = count_insert + count_member + count_delete;
    }
    //Get finnish time
    finish = getTime();

    printf("Serial Linked List Time Spent : %.6f secs\n", finish-start);

    Free_list(&head);
   
    return 0;
}


//Linked List Membership function
int Member(int value, struct list_node_s *head_p) {
    struct list_node_s *current_p = head_p;

    while (current_p != NULL && current_p->data < value)
        current_p = current_p->next;

    if (current_p == NULL || current_p->data > value) {
        return 0;
    }
    else
        return 1;
}

//Linked List Insertion function
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

//Linked List Deletion function
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

   current = *head_pp; 
   following = current->next;
   while (following != NULL) {
      free(current);
      current = following;
      following = current->next;
   }
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

//Get time function
double getTime() {
   struct timeval t;
   gettimeofday(&t, NULL);
   return (double)t.tv_sec + t.tv_usec/1000000.0;
}
