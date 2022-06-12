#ifndef __CORE_AI_H
#define __CORE_AI_H
#include "../utils/all.h"
#include "cards.h"
#include "constants.h"
#include "player.h"
#include "roles.h"
#include "types.h"
#include "utils.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

i32 disgust[4][4];
i32 ai_card_weight(Game* game, i32 player_id, i32 card_id, i32 max_disgust[10]);

void ai_initialize(Game* game, i32 player_id) {
    // disgust initialize
    /*
        警長	    others: 2
        歹徒	    警長:5
        叛徒	    警長:A 歹徒:5	A預設6，針對他一次後變為0
    */
    for (int i = 0; i < 4; i++) disgust[player_id][i] = 0;
    Player* my = game->players->data[player_id];
    switch (my->role->type) {
        case Sheriff:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    disgust[player_id][i] = -10000;
                } else {
                    disgust[player_id][i] = 2;
                }
            }
            break;
        case Criminal:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    disgust[player_id][i] = -10000;
                } else if (game->players->data[i]->role->type == Sheriff) {
                    disgust[player_id][i] = 5;
                }
            }
            break;
        case Traitor:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    disgust[player_id][i] = -10000;
                } else if (game->players->data[i]->role->type == Sheriff) {
                    disgust[player_id][i] = 6;
                } else {
                    disgust[player_id][i] = 5;
                }
            }
            break;
    }
    return;
}

i32 ai_request(Game* game, i32 player_id) {
    Player* ai = game->players->data[player_id];
    i32     max_disgust[10];
    for (int i = 0; i < game->players->size; i++) {
        max_disgust[(distance(game, player_id, i) > 0 ? distance(game, player_id, i) : 0)] =
            MAX(max_disgust[(distance(game, player_id, i) > 0 ? distance(game, player_id, i) : 0)],
                disgust[player_id][i]);
    }
    for (int i = 1; i < 10; i++) {
        max_disgust[i] = MAX(max_disgust[i], max_disgust[i - 1]);
    }
    i32 weight[ai->hands->size];
    for (int i = 0; i < ai->hands->size; i++) {
        weight[i] = ai_card_weight(game, player_id, i, max_disgust);
    }
}

i32 ai_player_cnt(Game* game) {
    i32 cnt = 0;
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp > 0) cnt++;
    }
    return cnt;
}

i32 ai_find_disgust_id(Game* game, i32 max_num, i32 max_disgust[10]) {
    for (int i = 0; i < game->players->size; i++) {
        if (max_num == max_disgust[i]) return i;
    }
    return 0;
}

i32 ai_card_weight(Game* game, i32 player_id, i32 card_id, i32 max_disgust[10]) {
    Player*  ai = game->players->data[player_id];
    CardType card = ai->hands->data[card_id]->type;
    i32      max_distance = 1;
    i32      Sheriff_id;
    i32      player_cnt = ai_player_cnt(game);
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->role->type == Sheriff) Sheriff_id = i;
    }
    if (ai->weapon != NULL) max_distance += ai->weapon->type - Volcanic;

    // many many cards
    if (card == Bang) return 6 + max_disgust[max_distance];
    if (card == Gatling) {
        if (ai->role->type == Sheriff) return 20;
        if (ai->role->type == Traitor) {
            if (player_cnt <= 2) return 20;
            if (game->players->data[Sheriff_id]->hp >
                (4 + Sheriff_id - player_id) % game->players->size)
                return 20;
            return 0;
        }
        if (ai->role->type == Criminal) {
            for (int i = 0; i < game->players->size; i++) {
                if (i != player_id && disgust[player_id][i] < 5) return 0;
                return 20;
            }
        }
    }
    if (card == Indians)
        return max_disgust[9] *
               (4 - game->players->data[ai_find_disgust_id(game, max_disgust[9], max_disgust)]
                        ->hands->size);
}

#endif  // __CORE_AI_H