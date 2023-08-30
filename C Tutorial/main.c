#include <stdio.h>
#include<string.h> 
#include <stdlib.h> 
#include "list.h"

void list_print_item(void *v) {
    char *item = (char*) v;
    printf("%s", item);
}

int main(int argc, char* argv[]) {
    list_t l;
    list_init(&l, NULL, free);

    FILE *file = fopen(argv[1], "r");

    if ( file == 0 ) {
        perror("Could not open file");
        exit(-1);
    }
    else {
        if (argc > 2) {
            char line[1024];
            if (strcmp(argv[2], "echo") == 0) { 
                // display the input, one word (continguous string of non-whitespace characters) per line, and exit
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
                list_visit_items(&l, list_print_item);
            }
            else if (strcmp(argv[2], "tail-remove") == 0){
                while  (fgets(line, sizeof(line), file) != NULL) {
                    char *line_copy = strdup(line);
                    list_insert_tail(&l, line_copy);
                }
                for (int i = 0; i <= 3; i++) {
                    list_remove_head(&l);
                }
                list_visit_items(&l, list_print_item);
            }
            else {
                printf("That is not a recognized command. Please provide a second parameter, either \'echo\', \'tail\', or \'tail-remove\'.\n");
            }
            fclose(file);
            exit(0);
        } else {
            printf("Please provide a second parameter, either \'echo\', \'tail\', or \'tail-remove\'.\n");
            fclose(file);
            exit(-1);
        }  
    }
    return 0;
}