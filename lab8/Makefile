CC = gcc

disk_image: fs_skeleton.o
	$(CC) -o $@ $^
	
fs_skeleton.o: inode.h fs_skeleton.c
	$(CC) -g -c fs_skeleton.c


.PHONY: clean

clean:
	$(RM) *.o disk_image