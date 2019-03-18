#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "tshashmap.h"
#include "tsiterator.h"
#include "tslinkedlist.h"
#include "tstreeset.h"
#include "tsuqueue.h"
#include "tsarraylist.h"

/*
Syd Lynch
slynch2
CIS 415
Extra Credit Project

This is my own work except for the ADTs and the scmp function which were provided by Joseph Sventek.
I discussed project's concepts with 415 students Robert Macy, Danny Lu, and Jeremy Unck,
but all code other than what was stated above was written entirely by me.

*/

char *CPATH;
int CRAWLER_THREADS;
TSArrayList *directories;
TSUQueue *work_queue;
TSHashMap *dependencies;

static int scmp(void *a, void *b) {
    return strcmp((char *)a, (char *)b);
}

void strip_newline(char *word) {
	int i;
	int length = strlen(word);
	for (i = 0; i < length; i++) {
		if (word[i] == '\n' || word[i] == '\r') {
			word[i] = '\0';
		}
	}
}

FILE *open_file(char *afile) {
  FILE *fp;
  char *thefile;
  int i;
  char *directory;
  long size = tsal_size(directories);

  for (i = 0; i < size; i++) {
    if (tsal_get(directories, i, (void **)&directory) == 1) {
      thefile = malloc(sizeof(afile) + sizeof(directory));
      strcpy(thefile, directory);
      strcat(thefile, afile);
      fp = fopen(thefile, "r");
      if (fp != NULL) {
        return fp;
      }
    }
  }

  return NULL;
}

void print_dependencies(TSHashMap *dependencies, TSTreeSet *printed, TSLinkedList *to_process) {
  int i;
  char *fn;
  tsll_removeFirst(to_process, (void**)&fn);
  while (fn != NULL) {
    TSLinkedList *ll;
    tshm_get(dependencies, fn, (void**)&ll);
    int size = tsll_size(ll);
    for (i = 0; i < size; i++) {
      char *name;
      tsll_get(ll, i, (void**)&name);
      if (tsts_contains(printed, name) == 0) {
        printf(" %s", name);
        tsts_add(printed, name);
        tsll_add(to_process, name);
      }
    }
    if (tsll_removeFirst(to_process, (void**)&fn) == 0) {
      fn = NULL;
    }
  }
}

int process(char *afile, TSLinkedList *deps, TSHashMap *dependencies, TSUQueue *work_queue) {
  char line_buffer[1024];
  char *temp;
  FILE *fp;
  fp = open_file(afile);
  if (fp == NULL) {
    printf("Unable to open file\n");
    return 0;
  }
  while (fgets(line_buffer, 1024, fp) != NULL) {
    temp = line_buffer;
    while ((temp[0] == ' ' || temp[0] == '\t') && temp[0] != '\0') {
      temp++;
    }
    if (strncmp("#include", temp, 8) == 0) {
      temp += 8;
      while ((temp[0] == ' ' || temp[0] == '\t') && temp[0] != '\0') {
        temp++;
      }
      if (temp[0] == '"') {
        temp++;
        int i = 0;
        while (temp[i] != '\0') {
          i++;
          if (temp[i] == '"') {
            TSLinkedList *element;
            strip_newline(temp);
            temp[i] = '\0';
            tsll_add(deps, strdup(temp));
            if (tshm_containsKey(dependencies, temp) == 0) {
              void *previous;
              element = tsll_create();
              if (tshm_put(dependencies, strdup(temp), element, &previous)) {
                free(previous);
                tsuq_add(work_queue, strdup(temp));
              }
            }
          }
        }
      }
    }
  }
  fclose(fp);

  return 1;
}

void free_entries(char *entry) {
  free(entry);
}

void *process_file() {
  char *afile = NULL;
  while (1) {
    tsuq_lock(work_queue);
    if (tsuq_remove(work_queue, (void**)&afile) == 0) {
      afile = NULL;
    }
    tsuq_unlock(work_queue);
    if (afile != NULL) {
      TSLinkedList *deps = NULL;
      tshm_lock(dependencies);
      if (tshm_get(dependencies, afile, (void **)&deps) == 1) {
        tshm_unlock(dependencies);
        if (process(afile, deps, dependencies, work_queue) == 0) {
          printf("Unable to process.\n");
          exit(-1);
        }
      }
    }
    else {
      break;
    }
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  int i, length;
  int file_index = 1;
  char *directory;
  directories = tsal_create(0);
  work_queue = tsuq_create();
  dependencies = tshm_create(0, 0.0);

  tsal_add(directories, "./");

  // Check for CPATH environment variable and split into separate directories, add to arraylist
  if (getenv("CPATH") != NULL) {
    int length;
    CPATH = getenv("CPATH");
    char *temp;
    temp = strtok(CPATH, ":");
    while (temp != NULL) {
      length = strlen(temp);
      if (temp[length-1] != '/') {
        char *temp2 = malloc(sizeof(char) * length+1);
        strcpy(temp2, temp);
        strcat(temp2, "/");
        temp = temp2;
      }
      //printf("%s\n", temp);
      tsal_add(directories, strdup(temp));
      temp = strtok(NULL, ":");
    }
    free(temp);
  }

  // Check for CRAWLER_THREADS environment variable, if it doesn't exist set it to 2
  if (getenv("CRAWLER_THREADS") != NULL) {
    CRAWLER_THREADS = atoi(getenv("CRAWLER_THREADS"));
  }
  else {
    CRAWLER_THREADS = 2;
  }

  if (argc < 2) {
    printf("Invalid. Usage: ./include_crawler [-Idir] ... file.c|file.l|file.y ...  \n");
    return 0;
  }

  for (i = 1; i < argc; i++) {
    char *current_arg = argv[i];
    length = strlen(current_arg);

    if (strncmp(current_arg, "-I", 2) == 0) {
      if (current_arg[length-1] != '/') {
        char *temp = malloc(sizeof(char) * 2);
        strcpy(temp, current_arg);
        strcat(temp, "/");
        current_arg = temp;
      }
      current_arg = current_arg + 2;
      directory = current_arg;
      strip_newline(directory);
      tsal_add(directories, directory);
      file_index = i+1;
    }

    else {
      char *obj;
      if (strcmp(&current_arg[length-2], ".y") != 0 && strcmp(&current_arg[length-2], ".l") != 0 && strcmp(&current_arg[length-2], ".c") != 0) {
        printf("Invalid - Usage: ./include_crawler [-Idir] ... file.c|file.l|file.y ...  \n");
        return -1;
      }
      else {
        TSLinkedList *deps;
        void *previous;
        obj = strdup(current_arg);
        obj[length-1] = 'o';
        deps = tsll_create();
        tsll_add(deps, strdup(current_arg));
        tshm_put(dependencies, obj, deps, &previous);
        tsuq_add(work_queue, strdup(current_arg));
        deps = tsll_create();
        tshm_put(dependencies, current_arg, deps, &previous);
        free(previous);
      }
    }
  }

  // Create threads
  pthread_t pthreads[CRAWLER_THREADS];
  for (i = 0; i < CRAWLER_THREADS; i++) {
    int p;
    p = pthread_create(&pthreads[i], NULL, &process_file, NULL);
    if (p) {
      printf("Error in thread creation.\n");
      exit(-1);
    }
  }

  // Wait for threads to finish
  for (i = 0; i < CRAWLER_THREADS; i++) {
    pthread_join(pthreads[i], NULL);
  }

  // Output dependencies
  for (i = file_index; i < argc; i++) {
    char *obj;
    TSTreeSet *printed;
    TSLinkedList *to_process;
    int length = strlen(argv[i]);
    obj = strdup(argv[i]);
    obj[length-1] = 'o';
    printf("%s:", obj);
    printed = tsts_create(scmp);
    tsts_add(printed, strdup(obj));
    to_process = tsll_create();
    tsll_add(to_process, strdup(obj));
    print_dependencies(dependencies, printed, to_process);
    printf("\n");
  }

  return 0;
}
