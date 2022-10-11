#include <stdio.h>
#include <stdlib.h>

#define MAX_TEXT_SIZE 1000

char *read_file(char *filename) {
  char *text = (char*) malloc(sizeof(char*) * MAX_TEXT_SIZE);
  FILE *fp;

  if((fp = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "file open failed.\n");
    exit(EXIT_FAILURE);
  }

  int i = 0;
  while((text[i] = fgetc(fp)) != EOF){
    i++;

    if(i >= MAX_TEXT_SIZE) {
      fprintf(stderr, "file size is over %d.", MAX_TEXT_SIZE);
      exit(-1);
    }
  }

  text[i] = '\0';

  fclose(fp);

  return text;
}
