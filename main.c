/* COP 3502C Assignment 2
This program is written by: Heng Hong */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define EMPTY -1
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct queue {
    struct node *front;
    struct node *back;
} queue;

typedef struct node {
    struct customer *data;
    struct node *next;
} node;

typedef struct customer {
    char *name;
    int numTickets;
    int timeEnter;
    int numLine;
} customer;

customer *createCustomer(int timeEnter, int numLine, char *name,
                         int numTickets) {

    customer *customer = malloc(sizeof(customer));
    customer->name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(customer->name, name);

    customer->numTickets = numTickets;
    customer->numLine = numLine;
    customer->timeEnter = timeEnter;

    return customer;
}

void init(struct queue *qPtr) {
    qPtr->front = NULL;
    qPtr->back = NULL;
}

void enqueue(struct queue *qPtr, customer *info) {

    struct node *temp = malloc(sizeof(struct node));
    temp->data = info;
    temp->next = NULL;

    // still check the temp because make sure malloc successfully
    if (temp != NULL) {
        if (qPtr->back)
            qPtr->back->next = temp;

        qPtr->back = temp;

        if (qPtr->front == NULL)
            qPtr->front = temp;
        // return1 ;
    }
}

customer *dequque(struct queue *qPtr) {
    // int saveval;

    if (qPtr->front == NULL) {
        return NULL;
    }

    struct node *temp = qPtr->front;

    qPtr->front = qPtr->front->next;

    free(temp);

    if (qPtr->front == NULL)
        qPtr->back = NULL;

    return NULL;
}

int peek(struct queue *qPtr) {
    if (qPtr->front != NULL) {
        return qPtr->front->data->numTickets;
    } else {
        return EMPTY;
    }
}

bool empty(struct queue *qPtr) { return qPtr->front == NULL; }

node *insert_end(node *root, customer *info) {

    node *temp = (node *)malloc(sizeof(node));
    temp->data = info;
    temp->next = NULL;

    if (root == NULL) {
        root = temp;
    } else {
        node *t = root;
        while (t->next != NULL) {
            t = t->next;
        }
        t->next = temp;
    }
    return root;
}

void freeList(struct node* root){
    struct node* tmpe;
    while (root != NULL){
        tmpe = root;
        free(tmpe->data->name);
        free(tmpe->data);
        root = root->next;
        free(tmpe);
    }
}

int main(void) {
    atexit(report_mem_leak);

    queue *myQueue[12];

    int testCase;
    int customerCount = 0;

    char name[16];
    int numTickets;
    int timeEnter;
    int numLine;

    int minTicket;
    int timeProcess = 1e9 + 1;

    for (int i = 0; i < 12; i++) {
        myQueue[i] = (struct queue *)malloc(sizeof(struct queue));
        init(myQueue[i]);
    }

    scanf("%d", &testCase);

    if (testCase <= 25) { // 2
        for (int i = 0; i < testCase; i++) {

            // set/reset the list to null
            node *root = NULL;

            // set time process
            timeProcess = 0;
            scanf("%d", &customerCount);
            int minTime = 1e9 + 1;
            if (customerCount <= 500000) {
                for (int k = 0; k < customerCount; k++) { // each cus
                    scanf("%d %d %s %d", &timeEnter, &numLine, name, &numTickets);

                    // find the minTime
                    if (timeEnter < minTime)
                        minTime = timeEnter;

                    // passing the value from read-in to createCustome function and return
                    // a pointer.
                    customer *temp = createCustomer(timeEnter, numLine, name, numTickets);

                    // passing the pointer as data into the node.
                    root = insert_end(root, temp);
                }
            }

            // walking all the linked list grab the line number
            for (node *t = root; t != NULL; t = t->next) {
                int a = t->data->numLine;

                // enq the pointer into correct position q
                enqueue(myQueue[a - 1], t->data);
            }

            // check the first few lines is empty or not
            for (int k = 0; k < 12; k++) {
                if (!empty(myQueue[k]) &&
                    myQueue[k]->front->data->timeEnter == minTime) {
                    // printf("%s\n", myQueue[k]->front->data->name);

                    // calc the time for the second line use
                    timeProcess = (myQueue[k]->front->data->timeEnter) + 30 +
                                  (5 * myQueue[k]->front->data->numTickets);

                    printf("%s from line %d checks out at time %d.\n",
                           myQueue[k]->front->data->name,
                           myQueue[k]->front->data->numLine, timeProcess);
                    dequque(myQueue[k]);
                    break;
                }
            }
            // jump into the loop to process else
            bool flag = true;

            while (flag) {
                minTicket = 101;
                flag = false;

                // check all lines and grab the mini ticket.
                for (int j = 0; j < 12; j++) {

                    // skip the line if empty using the empty function
                    if (empty(myQueue[j])) {
                        continue;
                    }
                    flag = true;
                    minTicket = min(peek(myQueue[j]), minTicket);
                }

                for (int j = 0; j < 12; j++) {
                    if (empty(myQueue[j])) {
                        continue;
                    }
                    flag = true;

                    // get the lines with less ticket
                    if (peek(myQueue[j]) == minTicket) {

                        //process time less than previous
                        if (timeProcess < myQueue[j]->front->data->timeEnter) {
                            timeProcess = myQueue[j]->front->data->timeEnter + 30 +
                                          5 * myQueue[j]->front->data->numTickets;
                        } else {
                            timeProcess =
                                    timeProcess + 30 + 5 * myQueue[j]->front->data->numTickets;
                        }
                        printf("%s from line %d checks out at time %d.\n",
                               myQueue[j]->front->data->name,
                               myQueue[j]->front->data->numLine, timeProcess);

                        dequque(myQueue[j]);
                        break;
                    }//process line with less tick if statement end
                }//for loop process the line
            }//while loop end
            freeList(root);

        }// end while with two for loop
    } // if statement end

    for (int i = 0; i < 12; i++) {
        free(myQueue[i]);
    }

    return 0;
}//main end
