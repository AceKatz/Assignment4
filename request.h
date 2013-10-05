/**
 * Header file for the linked list homework.
 * @author Eli Katz
 * ehkatz@gwu.edu
 */

#ifndef __LIST_HEADER__
#define __LIST_HEADER__


#include <stdio.h>
#include <stdlib.h>

struct list_node
{
    void *value;
    struct list_node* next;
};
struct request
{
    struct list_node* head;
};


struct request* ll_create(void)
{
    struct request *newList = (struct request *) malloc(sizeof(struct request));
    if (newList == NULL)
        return NULL;
    newList->head = NULL;
    return newList;
}

void ll_destroy(struct request *ll) 
{
    struct list_node *head = (ll->head);
    struct list_node *temp = (ll->head->next);
    while (ll->head != NULL) {
        free(ll->head);
        ll->head = temp;
        if (ll->head == NULL) break;
        else temp = ll->head->next;
    }
}

void ll_add(struct request *ll, void *value) 
{
    struct list_node *newNode = (struct list_node*) malloc(sizeof(struct list_node));
    newNode->value = value;
    
    if (ll->head == NULL) {
        newNode->next == NULL;
        ll->head = newNode;
        
    } else { 
        newNode->next = ll->head;
        ll->head = newNode;
        
        
    }    
}

int ll_length(struct request *ll)
{
    int length = 0;
    struct list_node *temp;
    temp = ll->head;
    while (temp != NULL) {
        length++;
        temp = temp->next;
    }
    return length;
}

void* ll_remove_first(struct request *ll)
{
    struct list_node *temp = ll->head->next;
    void *retVal = ll->head->value;
    free(ll->head);
    ll->head = temp;
    return retVal;
    
    
}

int ll_remove(struct request *ll, void *value)
{
    struct list_node *current = ll->head;
    struct list_node *next = current->next;

    void *print = current->value;
    if (print == value) {
        free(ll->head);
        ll->head = next;
        return 0;
    } else {
        while (next != NULL) {
            if (next->value == value) {
                current->next = next->next;
                free (next);
                return 0;
            } else {
                current = next;
                next = next->next;
            }    
        }
    }
    return -1;
        
}

int ll_contains(struct request *ll, void *value)
{
    struct list_node *current = (struct list_node *) malloc(sizeof(struct list_node));
    struct list_node *next = (struct list_node *) malloc(sizeof(struct list_node));
    int position = 1;
    current = ll->head;
    next = current->next;
    if (current->value == value) {
        return position;
    } else {
        position++;
        while (next != NULL) {
            if (next->value == value) {
                return position;
            } else {
                position++;
                current = next;
                next = next->next;
            }    
        }
    }
    return 0;
}

void ll_print(struct request *ll) 
{
    struct list_node *temp = (struct list_node *) malloc(sizeof(struct list_node));
    temp = ll->head;
    if (temp == NULL)
        printf("This list is empty.  There is nothing to print\n");
    while (temp != NULL) {
        int *print = (int *) temp->value;
        printf("%d\n", print);
        temp = temp->next;
    }
    printf ("\n");
}

#endif
