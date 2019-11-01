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
};

int getProcess() {
  int pid = getpid();
  return pid;
}

int getRand(int max) {
  int randomNum = rand() % max;
  return randomNum;
}

/* DEBUG: Can't return this array
char** initRoomNames() {
  int i = 0;
  char* roomNames[10];
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
  for (i = 0; i < 10; i++)
    printf("%s\n", roomNames[i]);
  return roomNames;
}
*/

void makeDir() {
  char* dirName = malloc(21 * sizeof(char));
  if (dirName == NULL)
    printf("An error occurred.\n");

  memset(dirName, '\0', 21);
  sprintf(dirName, "hamiltj2.rooms.%d", getProcess());
  mkdir(dirName, S_IRWXU);

  free(dirName);
}

void makeFiles(struct room* gameRooms, int numOfFiles) {
  int i;
  for (i = 0; i < numOfFiles; i++)
    printf("Index: %d | Room name: %s\n", gameRooms[i].id, gameRooms[i].name);
}


int main() {
  int roomsAdded = 0;

  struct room gameRooms[ROOMS_TO_SELECT];
  char* roomNames[TOTAL_ROOMS];

  int i = 0;
  for (i = 0; i < ROOMS_TO_SELECT; i++) {
    gameRooms[i].id = i;
  }

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

  makeDir();

  srand(time(0));

  while (roomsAdded < ROOMS_TO_SELECT) {
    int roomToAdd = getRand(TOTAL_ROOMS);

    if (roomNames[roomToAdd] != NULL) {
      gameRooms[roomsAdded].name = roomNames[roomToAdd];
      roomNames[roomToAdd] = NULL;
      printf("Added %s to index %d\n", gameRooms[roomsAdded].name, roomsAdded); /*DEBUG*/
      roomsAdded++;
    }
  }

  makeFiles(gameRooms, ROOMS_TO_SELECT);

  return 0;
}
