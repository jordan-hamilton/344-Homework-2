/*
// Created by Jordan Hamilton on 10/27/19.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int getProcess() {
  int pid = getpid();
  return pid;
}

void makeDir(pid) {
  char* dirName = malloc(21 * sizeof(char));
  if (dirName == NULL)
    printf("An error occurred.\n");

  memset(dirName, '\0', 21);
  sprintf(dirName, "hamiltj2.rooms.%d", getProcess());
  mkdir(dirName, S_IRWXU);

  free(dirName);
}



int main() {
  char* thing1;
  printf("%d\n", getProcess());
  makeDir();


  printf("Buildrooms\n");

  thing1 = malloc(10 * sizeof(char));
  strcpy(thing1, "Testing");
  printf(thing1);
  free(thing1);
  return 0;
}