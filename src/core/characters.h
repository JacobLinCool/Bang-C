#ifndef __CORE_CHARACTERS_H
#define __CORE_CHARACTERS_H

#include "../utils/all.h"
#include "types.h"
#include "utils.h"

void bart_cassidy(Game* game, i32 player_id) {
    player_draw_deck(game, player_id, 1);
    return;
}

void el_gringo(Game* game, i32 player_id) {}

void sid_ketchum(Game* game, i32 player_id) {
    // messenge?
    /*
    if(request(game, player_id, 0) && request(game, player_id, 0)){
        recover(game, player_id);
    }
    */
    return;
}
#endif  // __CORE_CHARACTERS_H
