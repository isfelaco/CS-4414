#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>
#include "inode.h"
#include <string.h>
#include <time.h> // needed for the random() function
#include <stdint.h>
#include <stddef.h>

#define TOTAL_BLOCKS (10*1024) // need to change this because this will change with N 
#define INODE_SZ sizeof(struct inode)

static unsigned char rawdata[TOTAL_BLOCKS*BLOCK_SZ];
static char bitmap[TOTAL_BLOCKS];

int get_free_block()
{
  int blockno = 0; // temp

  while (blockno < TOTAL_BLOCKS && bitmap[blockno]) {
      blockno++;
  }

  assert(blockno < TOTAL_BLOCKS);
  assert(!bitmap[blockno]);

  bitmap[blockno] = 1;

  return blockno;
}

void write_int(int pos, int val)
{
  int *ptr = (int *)&rawdata[pos];
  *ptr = val;
}

void place_file(char *file, int uid, int gid)
{
  int i, nbytes = 0;
  int i2block_index, i3block_index;
  struct inode *ip;
  FILE *fpr;
  unsigned char buf[BLOCK_SZ];

  fpr = fopen(file, "rb");
  if (!fpr) {
    perror(file);
    exit(-1);
  }

  // allocate memory for the inode
  ip = (struct inode *)malloc(sizeof(struct inode));
  if (!ip) {
    perror("malloc");
    free(ip);
    exit(-1);
  }

  ip->mode = 0;
  ip->nlink = 1;
  ip->uid = uid;
  ip->gid = gid;
  ip->ctime = random();
  ip->mtime = random();
  ip->atime = random();

  // allocate direct blocks
  for (i = 0; i < N_DBLOCKS; i++) {
    int blockno = get_free_block();
    if (blockno == -1) {
      fprintf(stderr, "no free blocks available\n");
      free(ip);
      exit(-1);
    }
    ip->dblocks[i] = blockno;

    // fill in the blocks with file data
    fread(&rawdata[BLOCK_SZ * blockno], 1, BLOCK_SZ, fpr);
    nbytes += BLOCK_SZ;
  }

  // allocate indirect blocks if needed
  if (nbytes >= N_DBLOCKS * BLOCK_SZ) {
    int indirect_block = get_free_block();
    if (indirect_block == -1) {
      fprintf(stderr, "no free blocks available\n");
      free(ip);
      exit(-1);
    }
    ip->iblocks[0] = indirect_block;

    for (i = N_DBLOCKS; nbytes < ip->size; i++) {
      int blockno = get_free_block();
      if (blockno == -1) {
        fprintf(stderr, "no free blocks available\n");
        free(ip);
        exit(-1);
      }
      // store block numbers as pointers in the indirect block
      write_int(BLOCK_SZ * indirect_block + (i - N_DBLOCKS) * sizeof(int), blockno);
      nbytes += BLOCK_SZ;
    }
  }

  ip->size = nbytes;  // total number of data bytes written for file
  printf("successfully wrote %d bytes of file %s\n", nbytes, file);

  free(ip); // free the memory
  fclose(fpr); // close the file
}

void extract_files(char *imagefile, int uid, int gid, char *path) {
  FILE* image = fopen(imagefile, "rb");
  if (!image) {
    perror("error opening disk image file");
    exit(-1);
  }

  /* read the disk image */
  size_t bytesRead = fread(rawdata, 1, sizeof(rawdata), image);
  fclose(image);
  if (bytesRead != sizeof(rawdata)) {
    perror("error reading disk image");
    fclose(image);
    exit(-1);
  }
  
  for (int inodeIndex = 0; inodeIndex < TOTAL_BLOCKS; inodeIndex++) {
    struct inode *ip = (struct inode*)&rawdata[inodeIndex * BLOCK_SZ];

    // check if inode is used
    /*
      issue: ip->uid != uid && ip->gid != gid even though they should
      i temporarily removed the rest of the condition so i could test the rest
      printf("Debug: bitmap[%d] = %d, uid = %d, gid = %d\n", inodeIndex, bitmap[inodeIndex], ip->uid, ip->gid);
    */
    if (ip->size > 0 && ip->uid == uid && ip->gid == gid) {
      for (int i = 0; i < N_DBLOCKS; i++) {
        int blockno = ip->dblocks[i];
        if (blockno >= 0) {
          char filepath[256];
          snprintf(filepath, sizeof(filepath), "%s/file_%d_%d_%d", path, uid, gid, inodeIndex);          

          // create a new file
          FILE *outputfile = fopen(filepath, "wb");
          if (!outputfile) {
            perror("error creating output file");
            exit(-1);
          }

          // write contents to the file
          fwrite(&rawdata[blockno * BLOCK_SZ], 1, BLOCK_SZ, outputfile);

          // close the output file
          fclose(outputfile);

          printf("file found at inode in block %d, file size %d\n", blockno, ip->size);

          bitmap[blockno] = 1;
        }
      }
    }
  }

  /* output list of unused blocks */
  char filepath[256];
  snprintf(filepath, sizeof(filepath), "%s/UNUSED_BLOCKS", path);          

  // create a new file
  FILE *outputfile = fopen(filepath, "wb");
  if (!outputfile) {
    perror("error creating output file");
    exit(-1);
  }

  // write numbers of unused blocks to the file
  for (int blockno = 0; blockno < TOTAL_BLOCKS; blockno++) {
    if (bitmap[blockno] == 0) { // block is unused
      fprintf(outputfile, "%d\n", blockno);
    }
  }

  // close the output file
  fclose(outputfile);
}

void main(int argc, char* argv[]) // add argument handling
{
  int i;
  FILE *outfile;

  char* imagefile;
  int nblocks;
  int iblocks;
  char* inputfile;
  int uid;
  int gid;
  int block;
  int inodepos;
  char* path;

  char* mode; // for -create, -extract, -insert

  // getting arguments 
  for (int i = 1; i < argc; i += 2) {
    if (strcmp(argv[i], "-image") == 0) {
      imagefile = argv[i+1]; // get name of image as char array/string

    } else if (strcmp(argv[i], "-nblocks") == 0) {
      nblocks = atoi(argv[i+1]); // get number of blocks 
      // can calculate TOTAL_BLOCKS with this * 1024

    } else if (strcmp(argv[i], "-iblocks") == 0) {
      iblocks = atoi(argv[i+1]); // number of blocks used for inodes
      // denoted as M in assignment 

    } else if (strcmp(argv[i], "-inputfile") == 0) {
      inputfile = argv[i+1]; // file to be added 

    } else if (strcmp(argv[i], "-u") == 0) {
      uid = atoi(argv[i+1]); // uid 

    } else if (strcmp(argv[i], "-g") == 0) {
      gid = atoi(argv[i+1]); // gid 

    } else if (strcmp(argv[i], "-block") == 0) {
      block = atoi(argv[i+1]); // inode is placed in this block
      // called D, must be smaller than M!

    } else if (strcmp(argv[i], "-inodepos") == 0) {
      inodepos = atoi(argv[i+1]); // position within block (in inodes, not bytes)
      // listed as I on assignment

    } else if (strcmp(argv[i], "-o") == 0) {
      path = argv[i+1];

    } else if (strcmp(argv[i], "-create") == 0) {
      mode = "create";
      i = i - 1;

    } else if (strcmp(argv[i], "-extract") == 0) {
      mode = "extract";
      i = i - 1;

    } else if (strcmp(argv[i], "-insert") == 0) {
      mode = "insert";
      i = i - 1;

    } else {
      printf("arg %d : %s\n", i, argv[i]); // for testing
      fprintf(stderr, "unknown arg\n");
      exit(-1);
    }

  }
  
  if (strcmp(mode, "create") == 0) {
    // ex command: ./disk_image -create -image output_disk_image.img -nblocks 100 -iblocks 5 -inputfile laptop_image -u 10578 -g 1231 -block 2 -inodepos 0
    
    // check that block is smaller than iblocks and inodepos is within bounds
    if (block >= iblocks || inodepos >= N_DBLOCKS) {
      fprintf(stderr, "invalid block or inode position\n");
      exit(-1);
    }

    if (inputfile == NULL || uid == 0 || gid == 0) {
      fprintf(stderr, "missing required parameters for create mode\n");
      exit(-1);
    }
    place_file(inputfile, uid, gid);

    // output to 'imagefile'
    outfile = fopen(imagefile, "wb");
    if (!outfile) {
      perror("datafile open");
      exit(-1);
    }

    i = fwrite(rawdata, 1, TOTAL_BLOCKS*BLOCK_SZ, outfile);
    if (i != TOTAL_BLOCKS*BLOCK_SZ) {
      perror("fwrite");
      exit(-1);
    }

    i = fclose(outfile);
    if (i) {
      perror("datafile close");
      exit(-1);
    }
  } else if (strcmp(mode, "extract") == 0) {
    // ex command: ./disk_image -extract -image output_disk_image.img -u 10578 -g 1231 -o test
    if (imagefile == NULL || path == NULL) {
      fprintf(stderr, "missing required parameters for extract mode\n");
      exit(-1);
    }
    extract_files(imagefile, uid, gid, path); // TODO

  } else if (strcmp(mode, "insert") == 0) {
    /*
      read a disk image IMAGE_FILE with N total blocks, of which the first M are inode blocks
      insert a file FILE, with a specified uid and gid
      new file should be placed in block D at position I within that block
      check that D < M and that I will fit in one block,
        and that the specified inode position isn't already occupied
    */
  } else {
    fprintf(stderr, "unknown mode\n");
    exit(-1);
  }

  printf("Done.\n");
  return;
}
