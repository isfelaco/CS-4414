#include <stdio.h>
#include<string.h> 
#include <stdlib.h> 
#include "list.c"

void list_print_item(void *v) {
    char *item = (char*) v;
    printf("%s", item);
}

int main(int argc, char* argv[]) {
    list_t l;
    list_init(&l, NULL, free);

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
                // exit
            }
            else if (strcmp(argv[2], "tail") == 0) {
                // after input has been added to list, print contents
                while  (fgets(line, sizeof(line), file) != NULL) {
                    char *line_copy = strdup(line);
                    list_insert_tail(&l, line_copy);
                }
                list_visit_items(&l, list_print_item);
                // exit
            }
            else if (strcmp(argv[2], "tail-remove") == 0){
                while  (fgets(line, sizeof(line), file) != NULL) {
                    char *line_copy = strdup(line);
                    list_insert_tail(&l, line_copy);
                }
                list_remove_head(&l);
                list_remove_head(&l);
                list_remove_head(&l);
                list_visit_items(&l, list_print_item);
                // exit
            }
            fclose(file);
        }   
    }
    return 0;
}