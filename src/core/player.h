#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H

#include "../utils/all.h"
#include "types.h"

void real_player(Game* game, i32 player_id) {
    printf("We didn't implement anything yet, so just enter anything: ");
    string input = calloc(1024, sizeof(char));
    scanf("%[^\n]%*c", input);
    free(input);
}

void computer_player(Game* game, i32 player_id) {
    printf("My name is %s.\n", game->players->data[player_id]->name);
    printf("I am a computer, I will do nothing.\n");
}

#endif  // __CORE_PLAYER_H
