#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>
#include "inode.h"
#include <string.h>

#define TOTAL_BLOCKS (10*1024) // need to change this because this will change with N 

static unsigned char rawdata[TOTAL_BLOCKS*BLOCK_SZ];
static char bitmap[TOTAL_BLOCKS];

int get_free_block()
{
  // fill in here
  /*
  assert(blockno < TOTAL_BLOCKS);
  assert(bitmap[blockno]);
  return blockno;
  commented out to test arguments
  */
  return 0; // to test arguments, actual one will return blockno
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

  ip->mode = 0;
  ip->nlink = 1;
  ip->uid = uid;
  ip->gid = gid;
  ip->ctime = random();
  ip->mtime = random();
  ip->atime = random();

  fpr = fopen(file, "rb");
  if (!fpr) {
    perror(file);
    exit(-1);
  }

  for (i = 0; i < N_DBLOCKS; i++) {
    int blockno = get_free_block();
    ip->dblocks[i] = blockno;
    // fill in here
  }

  // fill in here if IBLOCKS needed
  // if so, you will first need to get an empty block to use for your IBLOCK

  ip->size = nbytes;  // total number of data bytes written for file
  printf("successfully wrote %d bytes of file %s\n", nbytes, file);
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

  char* mode; // for -create, -extract, -insert

  // getting arguments 
  for (int i = 0; i < argc; i++) {
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

    } else if (strcmp(argv[i], "-create") == 0) {
      mode = "create";

    } else if (strcmp(argv[i], "-extract") == 0) {
      mode = "extract";

    } else if (strcmp(argv[i], "-insert") == 0) {
      mode = "insert";
    }


    // printf("here is argument %d : %s\n", i, argv[i]); for testing

  }
  printf("the mode is %s\n", mode);

  // TODO: create, then extract, then insert (so we can use extract to test create)

  /* // commented out for testing getting arguments, uncomment when ready
  outfile = fopen(output_filename, "wb");
  if (!outfile) {
    perror("datafile open");
    exit(-1);
  }

  // fill in here to place file 

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

  printf("Done.\n");
  */
  return;
}
