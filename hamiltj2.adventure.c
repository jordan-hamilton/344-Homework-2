/*
// Created by Jordan Hamilton on 10/27/19.
*/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void getLatestDir();

int main() {
  printf("Adventure\n");
  getLatestDir();
  return 0;
}

void getLatestDir() {
  int timestamp;
  DIR* currentDir;
  struct dirent* subDir;
  struct stat attributes;

  char* prefix = "hamiltj2.rooms.";
  char latestDir[256];
  memset(latestDir, '\0', 256);

  currentDir  = opendir(".");
  if (!currentDir) {
    printf("An error occurred.\n");
  }

  while (subDir = readdir(currentDir)) {
    if (strstr(subDir->d_name, prefix)) {
      printf("Found the prefix: %s\n", subDir->d_name);
      stat(subDir->d_name, &attributes);

      if ((int) attributes.st_mtime > timestamp) {
        timestamp = (int) attributes.st_mtime;
        memset(latestDir, '\0', sizeof(latestDir));
        strcpy(latestDir, subDir->d_name);
        printf("Newer subdirectory: %s | New time: %d\n", subDir->d_name, timestamp);
      }

    }
  }

  closedir(currentDir);
  printf("%s | %d ", latestDir, timestamp);

}
