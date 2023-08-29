#include <stdio.h>

typedef struct list_item {
    struct list_item *pred, *next;
    void *datum;
} list_item_t;

typedef struct list {
    list_item_t *head, *tail;
    unsigned length;
    int (*compare) (const void *key, const void *with);
    void (*datum_delete) (void *);
} list_t;

/*
    list constructor
*/
void list_init(list_t *l, int (*compare) (const void *key, const void *with), void (*datum_delete)(void *datum)) {
    l->head = NULL;
    l->tail = NULL;
    l->length = 0;
    l->compare = compare;
    l->datum_delete = datum_delete;
}

/*
    takes a pointer to a function `visitor` and calls it on each member of l in order from l->head to l->tail
    visitor will need to be the address of a function that can display the list-item contents
*/
void list_visit_items(list_t *l, void (*visitor) (void *v)) {
    // 
    list_item_t *current = l->head;
    while (current != NULL) {
        visitor(current->datum);
        current = current->next;
    }
}

/*
    insert item into list at the tail
*/
void list_insert_tail(list_t *l, void *v) {
    list_item_t *new_item = (list_item_t*)malloc(sizeof(list_item_t));
    new_item->datum = v;
    new_item->next = NULL;

    if (l->tail == NULL) { // list is empty, make this the tail and head
        new_item->pred = NULL;
        l->head = new_item;
        l->tail = new_item;
    } else { // insert at the tail
        new_item->pred = l->tail;   // move the tail before the new item
        l->tail->next = new_item;   // link the tail to the new item
        l->tail = new_item;         // replace list tail with new item
    }
    l->length++;
}

/*
    remove item from the head of the list
*/
void list_remove_head(list_t *l) {
    list_item_t *current = l->head;
    l->head = current->next;     // next item in list is the new head
    l->head->pred = NULL;        // head should have no item before it
    l->datum_delete(current);    // free up the space
    l->length--;
}