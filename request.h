/**
 * Header file for the linked list homework.
 * @author Eli Katz
 * ehkatz@gwu.edu
 */

#ifndef __LIST_HEADER__
#define __LIST_HEADER__


#include <stdio.h>
#include <stdlib.h>
#define MAX_CONCURRENCY 256

struct request
{
    void *fd;
    struct request* next;
};
struct request *requests;

struct request* req_create(void *fd)
{
    struct request *newList = (struct request *) malloc(sizeof(struct request));
    if (newList == NULL)
        return NULL;
    newList->fd = fd;
    newList->next = NULL;
    return newList;
}
struct request *get_request(void) 
{
    while (requests == NULL) ;
    
    struct request *ret = requests;
    requests = requests->next;
    return ret;
}
void put_request(struct request *r) 
{
    if (requests == NULL) {
        printf("NEW NODE AND SHIT!\n");
        requests = r;
        return;
    }
    struct request *index = requests;
    int count = 0;
    while (index->next != NULL) {
        index = index->next;
        count++;
    }
    if (count < MAX_CONCURRENCY)
         index = r;
}



#endif
