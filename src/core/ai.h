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
i32 ai_target;
i32 ai_card_weight(Game* game, i32 ai_id, i32 card_id, i32 max_disgust[10], i32 max_dist_id[10]);
i32 equip_total(Game* game, i32 me_id, i32 player_id);
i32 card_count(Game* game, i32 player_id, i32 card);

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
    i32     max_dist_id[10];
    for (int i = 0; i < game->players->size; i++) {
        i32 dist = (distance(game, player_id, i) > 0 ? distance(game, player_id, i) : 0);
        if (disgust[player_id][i] > max_disgust[dist]) {
            max_disgust[dist] = disgust[player_id][i];
            max_dist_id[dist] = i;
        }
    }
    for (int i = 1; i < 10; i++) {
        if (max_disgust[i - 1] > max_disgust[i]) {
            max_disgust[i] = max_disgust[i - 1];
            max_dist_id[i] = max_dist_id[i - 1];
        }
    }
    i32 weight[ai->hands->size];
    for (int i = 0; i < ai->hands->size; i++) {
        weight[i] = ai_card_weight(game, player_id, i, max_disgust, max_dist_id);
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

i32 ai_card_weight(Game* game, i32 ai_id, i32 card_id, i32 max_disgust[10], i32 max_dist_id[10]) {
    Player*  ai = game->players->data[ai_id];
    CardType card = ai->hands->data[card_id]->type;
    i32      Sheriff_id;
    i32      player_cnt = ai_player_cnt(game);
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->role->type == Sheriff) Sheriff_id = i;
    }
    i32 max_distance = 1;
    if (ai->weapon != NULL) max_distance += ai->weapon->type - Volcanic;

    // many many cards
    if (card == Bang) {
        ai_target = max_dist_id[max_distance];
        return 6 + max_disgust[max_distance];
    }
    if (card == Missed) return 0;
    if (card == Gatling) {
        if (ai->role->type == Sheriff) return 20;
        if (ai->role->type == Traitor) {
            if (player_cnt <= 2) return 20;
            if (game->players->data[Sheriff_id]->hp >
                (4 + Sheriff_id - ai_id) % game->players->size)
                return 20;
            return 0;
        }
        if (ai->role->type == Criminal) {
            for (int i = 0; i < game->players->size; i++) {
                if (i != ai_id && disgust[ai_id][i] < 5) return 0;
                return 20;
            }
        }
    }
    if (card == Indians) {
        ai_target = max_dist_id[9];
        return max_disgust[9] * (4 - game->players->data[max_dist_id[9]]->hands->size) *
               (4 - game->players->data[max_dist_id[9]]->hands->size);
    }
    if (card == Panic || card == Cat_Balou) {
        ai_target = max_dist_id[1];
        return max_disgust[1] * equip_total(game, ai_id, max_dist_id[1]);
    }
    if (card == Stagecoach) return 5 * (ai->hp - ai->hands->size - 1);
    if (card == Wells_Fargo) return 5 * (ai->hp - ai->hands->size - 2);
    if (card == General_Store) {
        i32 total = 0;
        for (int i = 0; i < game->players->size; i++) {
            if (disgust[ai_id][i] < 5) {
                total += (i == ai_id ? 5 : 2) * (5 - game->players->data[i]->hands->size);
            }
        }
        return total;
    }
    if (card == Beer) return (game->players->size > 2 ? 5 * (ai->bullet - ai->hp) : 0);
    if (card == Saloon) {
        i32 total = 0;
        for (int i = 0; i < game->players->size; i++) {
            if (disgust[ai_id][i] < 5) {
                total += (i == ai_id ? 5 : 2) * (5 - game->players->data[i]->hp);
            }
        }
        return total;
    }
    if (card == Duel) {
        i32 ai_bang_cnt = card_count(game, ai_id, Bang);
        ai_target = max_dist_id[9];
        if (ai_bang_cnt < 1) return 0;
        i32 enemy_hands = game->players->data[max_dist_id[9]]->hands->size;
        if (enemy_hands < 2) return 50;
        return (3 * (ai_bang_cnt + 1) - enemy_hands) * (3 * (ai_bang_cnt + 1) - enemy_hands);
    }
    if (card == Barrel) return 500;
    if (card == Scope) return 400;
    if (card == Mustang) return 400;
    if (card == Jail) {
        if (max_disgust[9] <= 4) return 0;
        ai_target = max_dist_id[9];
        return 2 * max_disgust[9];
    }
    if (card == Volcanic) return 200;
    if (card == Schofield) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_disgust[1] > 4) return 0;
        return 120 * ((ai->weapon != NULL ? ai->weapon->type : 0) < card);
    }
    if (card == Remington) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_disgust[1] > 4) return 0;
        return 130 * ((ai->weapon != NULL ? ai->weapon->type : 0) < card);
    }
    if (card == Rev_Carabine) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_disgust[1] > 4) return 0;
        return 140 * ((ai->weapon != NULL ? ai->weapon->type : 0) < card);
    }
    if (card == Winchester) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_disgust[1] > 4) return 0;
        return 150 * ((ai->weapon != NULL ? ai->weapon->type : 0) < card);
    }
}

i32 equip_total(Game* game, i32 me_id, i32 player_id) {
    Player* player = game->players->data[player_id];
    Player* my = game->players->data[me_id];
    i32     total = 0;
    if (player->weapon->type == Volcanic) total += 200;
    if (player->weapon->type == Schofield)
        total += 120 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    if (player->weapon->type == Remington)
        total += 130 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    ;
    if (player->weapon->type == Rev_Carabine)
        total += 140 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    ;
    if (player->weapon->type == Winchester)
        total += 150 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    ;
    if (player->barrel != NULL) total += 500;
    if (player->scope != NULL) total += 100;
    return total;
}

i32 card_count(Game* game, i32 player_id, i32 card) {
    Player* player = game->players->data[player_id];
    i32     count = 0;
    for (int i = 0; i < player->hands->size; i++) {
        if (player->hands->data[i]->type == card) count++;
    }
    return count;
}
#endif  // __CORE_AI_H