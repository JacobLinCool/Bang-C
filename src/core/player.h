#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H
#include "../utils/all.h"
#include "cards.h"
#include "constants.h"
#include "game.h"
#include "types.h"
#include "utils.h"

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

//choose enemy, if not vaild people, return -1
i32 player_choose_enemy(Game* game, i32 me_id) {
    i32 enemy_id;
    i32 plyaer_size = game->player->size;
    printf("Choose enemy: \n");
    printf("-1)cancel\n");
    for(i32 i = 0, k = 1; i < plyaer_size; i++) {
        if(me_id != i && get_player_hp(game, i) > 0) {
            printf("%d)Player %d hp: %d\n", k++, i, get_player_hp(game, i));
        }
    }
    sancf("%d", &enemy_id);
    if( enemy_id < plyaer_size && get_player_hp(game, enemy_id) > 0 && enemy_id != me_id ) {
        enemy_id = -1;
        printf( "Wrong Player id!\n" );
    }
    return enemy_id;
}

i32 computer_choose_enemy*(Game* game, i32 me_id) {
    i32 enemy_id;
    i32 plyaer_size = game->player->size;
    while(1) {
        enemy_id = rand() % plyaer_size;
        if( enemy_id != me_id && get_player_hp(game, enemy_id) > 0 ) break;
    }
    return enemy_id;
}

// TODO: player play func
// NOTE: when use the card jail

#endif  // __CORE_PLAYER_H
