#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H

#include "../utils/all.h"
#include "types.h"

void real_player(Game* game, i32 player_id) {
    printf("We didn't implement anything yet, so just enter anything: ");
    char* input = $(calloc(1024, sizeof(char)));
    scanf("%*[\n]");
    scanf("%[^\n]%*c", input);
}

void computer_player(Game* game, i32 player_id) {
    Console.gray("My name is %s.", game->players->data[player_id]->name);
    Console.gray("I am a computer, I will do nothing.");
}

#endif  // __CORE_PLAYER_H
