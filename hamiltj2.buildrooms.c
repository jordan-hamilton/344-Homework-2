/*
// Created by Jordan Hamilton on 10/27/19.
*/
#define TOTAL_ROOMS 10
#define ROOMS_TO_SELECT 7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

struct room {
  int id;
  char* name;
  int numConnections;
};

int getProcess();
int getRand(int min, int max);
void initRooms(struct room*);
void makeDir();
void nameRooms(struct room*);
void makeFiles(struct room*);

int main() {
  struct room gameRooms[ROOMS_TO_SELECT];
  srand(time(0));

  makeDir();
  initRooms(gameRooms);
  makeFiles(gameRooms);

  return 0;
}

int getProcess() {
  int pid = getpid();
  return pid;
}

int getRand(int min, int max) {
  /* Get a random number that's at most the difference between min and max, then add that to the min to get a number
   * in the provided range */
  int randomNum = (rand() % (1 + max - min)) + min;
  return randomNum;
}

void initRooms(struct room* rooms) {
  int i;
  for (i = 0; i < ROOMS_TO_SELECT; i++) {
    rooms[i].id = i;
    rooms[i].numConnections = 0;
  }
  nameRooms(rooms);
}

void makeDir() {
  char* dirName = malloc(21 * sizeof(char));
  if (dirName == NULL)
    printf("An error occurred.\n");

  memset(dirName, '\0', 21);
  sprintf(dirName, "hamiltj2.rooms.%d", getProcess());
  mkdir(dirName, S_IRWXU);

  free(dirName);
}

void makeFiles(struct room* gameRooms) {
  int i;
  for (i = 0; i < ROOMS_TO_SELECT; i++)
    printf("Index: %d | Room name: %s\n", gameRooms[i].id, gameRooms[i].name);
}

void nameRooms(struct room* rooms) {
  int roomsAdded = 0;
  char* roomNames[TOTAL_ROOMS];

  roomNames[0] = "Kitchen";
  roomNames[1] = "Hallway";
  roomNames[2] = "Bathroom";
  roomNames[3] = "Den";
  roomNames[4] = "Closet";
  roomNames[5] = "Cellar";
  roomNames[6] = "Attic";
  roomNames[7] = "Pantry";
  roomNames[8] = "Bedroom";
  roomNames[9] = "Foyer";

  while (roomsAdded < ROOMS_TO_SELECT) {
    int roomToAdd = getRand(0, TOTAL_ROOMS - 1);

    if (roomNames[roomToAdd] != NULL) {
      rooms[roomsAdded].name = roomNames[roomToAdd];
      roomNames[roomToAdd] = NULL;
      printf("Added %s to index %d\n", rooms[roomsAdded].name, roomsAdded);
      roomsAdded++;
    }
  }
}
