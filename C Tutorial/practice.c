#include <stdio.h>
#include<string.h> 
#include <stdlib.h> 

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

int int_compar(const void *arg1, const void *arg2) {
    int *pleft = (int)arg1;
    int *pright = (int)arg2;

    if (*pleft < *pright) return -1;
    else if (*pleft == *pright) return 0;
    else return 1;
}

void delete_int(void *data) {
    free(data);
}

void list_init(list_t *l, int (*compare) (const void *key, const void *with), void (*datum_delete)(void *datum)) {
    l->head = NULL;
    l->tail = NULL;
    l->length = 0;
    l->compare = compare;
    l->datum_delete = datum_delete;
}

/* 
delete later
*/
void list_print(list_t *l) {
    list_item_t *current = l->head;
    while (current != NULL) {
        char *num = (char*)current->datum;
        printf("%s", num);
        current = current->next;
    }
}

void visitor(char *nums) {
	printf("%s", nums);
}

/*
TO-DO:
should iterate through the list similar to list_print above
but instead of printf(), call the function visitor on the item
*/
void list_visit_items(list_t *l, void (*visitor) (char *nums)) {
    // takes a pointer to a function `visitor` 
    // and calls it on each member of l in order from l->head to l->tail
    // visitor will need to be the address of a function that can display
    // the list-item contents
    
    list_item_t *current = l->head;
    while (current != NULL) {
        char *num = (char*)current->datum;
        visitor(num);
        current = current->next;
    }
}

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

void list_remove_head(list_t *l) {
    list_item_t *current = l->head;
    l->head = current->next;    // next item in list is the new head
    l->head->pred = NULL;        // head should have no item before it
    free(current);              // free up the space
}


int main(int argc, char* argv[]) {
    list_t l;
    list_init(&l, int_compar, delete_int);
    void (*visit)(char *nums) = visitor;

    FILE *file = fopen(argv[1], "r");

    if ( file == 0 ) {
        printf( "Could not open file\n" );
    }
    else {
        if (argc > 2) {
            char line[1024];
            if (strcmp(argv[2], "echo") == 0) { 
                // display the input, one word 
                // (continguous string of non-whitespace characters) 
                // per line, and exit
                while  (fgets(line, sizeof(line), file) != NULL) {
                    printf("%s", line);
                }
            }
            else if (strcmp(argv[2], "tail") == 0) {
                // after input has been added to list, print contents
                while  (fgets(line, sizeof(line), file) != NULL) {
                    char *line_copy = strdup(line);
                    list_insert_tail(&l, line_copy);
                }
                // use list_items_visit for this
                //list_print(&l);
                list_visit_items(&l, visit);
            }
            else if (strcmp(argv[2], "tail-remove") == 0){
                while  (fgets(line, sizeof(line), file) != NULL) {
                    char *line_copy = strdup(line);
                    list_insert_tail(&l, line_copy);
                }
                list_remove_head(&l);
                list_remove_head(&l);
                list_remove_head(&l);
                // use list_items_visit for this
                //list_print(&l);
                list_visit_items(&l, visit);
            }
            fclose(file);
        }   
    }
    return 0;
}
