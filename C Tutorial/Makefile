CC = gcc

# Compile main.o and list.o into list.
# $@ is a macro for the target name (i.e., list_harness).
# $^ is a macro for the dependency list (i.e., main.o and list.o).
list_harness: main.o list.o
	$(CC) -o $@ $^
	
# Compile main.c into main.o.
# main.o depends on list.h (main.c #includes list.h),
# so we include in the dependency list as well.
main.o: list.h main.c
	$(CC) -g -c main.c

list.o: list.h list.c
	$(CC) -g -c list.c

# The clean target is not really a file to create.
.PHONY: clean

# Clean up object files and the list binary
clean:
	$(RM) *.o list_harness