#ifndef __CORE_AI_H
#define __CORE_AI_H
#include <stdlib.h>

#include "../utils/all.h"
#include "utils.h"

#ifdef DEBUG
i32 speed = 10;
#else
i32 speed = 1;
#endif

#define BAD_GUY 6
#define AI_PLAY 0
#define AI_DISCARD 1
#define AI_SPECIFY 2
#define AI_FORCE_PLAY 3

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct _Weight {
    i32 weight;
    i32 target;
    i32 id;
} Weight;
i32      hate[7][7];
i32      ai_target;
i8       ai_request_type;  // 0: play 1: discard
CardType ai_request_card;
i8       ai_bang_use;
i8       ai_respond_error;

i32 ai_card_weight(Game* game, Cards* cards, i32 ai_id, i32 card_id, i32 max_hate[10],
                   i32 max_hate_id[10]);
i32 equip_total(Game* game, i32 me_id, i32 player_id);
i32 card_count(Game* game, i32 player_id, i32 card);
i32 ai_player_cnt(Game* game);

void print_weight_card(Game* game, i32 me_id, Weight weight[], Cards* candi_card) {
    for (int i = 0; i < 4; i++) {
        printf("%d(%s): %d,", i, role_name[game->players->data[i]->role->type], hate[me_id][i]);
    }
    printf("\n");
    for (int i = 0; i < candi_card->size; i++) {
        printf("[%s] weight: %d,", card_name[candi_card->data[weight[i].id]->type],
               weight[i].weight);
    }
    printf("\n");
}

/**
 * @brief AI request setting
 *
 * @param type AI_PLAY(0) AI_DISCARD(1) AI_SPECIFY(2) AI_FORCE_PLAY(3)
 * @param card if type is AI_SPECIFY(2), then need it
 */
void ai_request_setting(i8 type, CardType card) {
    if (type == AI_SPECIFY) {
        ai_request_card = card;
    }
    ai_request_type = type;
}
int ai_weight_cmp(const void* a, const void* b) {
    return (*(Weight*)b).weight - (*(Weight*)a).weight;
}

void ai_hate_change(Game* game, i32 ai_id, i32 target_id, i32 value) {
    if (ai_id < 0 || target_id < 0) return;
    hate[target_id][ai_id] += value;
    if (target_id == Sheriff) {
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->role->type == Deputy &&
                game->players->data[i]->role->type != Sheriff) {
                // Deputy hate who attack Sheriff
                hate[i][ai_id] += value;
            } else if (game->players->data[i]->role->type != Sheriff) {
                hate[i][ai_id] -= value;
            }
        }
    }
    return;
}

void ai_saloon(Game* game, i32 ai_id, i32 helper_id) {
    i32 max_hp = -1;
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp > max_hp) {
            max_hp = game->players->data[i]->hp;
        }
    }
    if (max_hp - game->players->data[ai_id]->hp > 2 || game->players->data[ai_id]->hp == 1) {
        hate[ai_id][helper_id]--;
    }
}

void ai_initialize(Game* game, i32 player_id) {
    // hate initialize
    /*
        警長	    others: 2
        歹徒	    警長:5
        叛徒	    警長:A 歹徒:5	A預設6，針對他一次後減少1
    */
    for (int i = 0; i < 7; i++) hate[player_id][i] = 0;
    Player* my = game->players->data[player_id];
    switch (my->role->type) {
        case Sheriff:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    hate[player_id][i] = -10000;
                } else {
                    hate[player_id][i] = 4;
                }
            }
            break;
        case Deputy:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    hate[player_id][i] = -10000;
                } else if (game->players->data[i]->role->type == Sheriff) {
                    hate[player_id][i] = -1000;
                } else {
                    hate[player_id][i] = 5;
                }
            }
            break;
        case Criminal:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    hate[player_id][i] = -10000;
                } else if (game->players->data[i]->role->type == Sheriff) {
                    hate[player_id][i] = 5;
                }
            }
            break;
        case Traitor:
            for (int i = 0; i < game->players->size; i++) {
                if (i == player_id) {
                    hate[player_id][i] = -10000;
                } else if (game->players->data[i]->role->type == Sheriff) {
                    if (game->players->size > 4) {
                        hate[player_id][i] = 0;
                    } else {
                        hate[player_id][i] = 6;
                    }
                } else {
                    hate[player_id][i] = 5;
                }
            }
            break;
    }
    return;
}

i32 ai_request(Game* game, i32 player_id, Cards* candi_card) {
    if (candi_card->size == 0) return -1;
    Player* ai = game->players->data[player_id];
    i32     max_hate[10] = {0};
    i32     max_hate_id[10] = {0};
    for (int i = 0; i < game->players->size; i++) {
        if (ai_player_cnt(game) == 2 && i != player_id) hate[player_id][i] = 10;
    }
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp <= 0 || i == player_id) continue;
        i32 dist = (distance(game, player_id, i) > 0 ? distance(game, player_id, i) : 0);
        if (hate[player_id][i] > max_hate[dist]) {
            max_hate[dist] = hate[player_id][i];
            max_hate_id[dist] = i;
        }
    }
    for (int i = 1; i < 10; i++) {
        if (max_hate[i - 1] > max_hate[i]) {
            max_hate[i] = max_hate[i - 1];
            max_hate_id[i] = max_hate_id[i - 1];
        }
    }
    Weight weight[candi_card->size];
    for (int i = 0; i < candi_card->size; i++) {
        // DEBUG_PRINT("(size: %d,ai: %d)card: %s\n", candi_card->size, player_id,
        //             card_name[candi_card->data[i]->type]);
        weight[i].weight = ai_card_weight(game, candi_card, player_id, i, max_hate, max_hate_id);
        weight[i].id = i;
        weight[i].target = ai_target;
    }
    qsort(weight, candi_card->size, sizeof(Weight), ai_weight_cmp);  // sort from big to small
    // print_weight_card(game, player_id, weight, candi_card);
    //  if event is discard cards
    //  DEBUG_PRINT("request_type: %d\n", ai_request_type);

    if (ai_request_type == AI_DISCARD) {
        // for real thinking time
        i32 card_hard = (600 + 50 * weight[candi_card->size - 1].weight) / 50;
        if (card_hard < 5) card_hard = 5;
        if (card_hard > 20) card_hard = 20;
        usleep(500 * (rand() % 1000 + candi_card->size * game->players->size * card_hard) / speed);
        i32 missed_cnt = 0;
        for (int i = candi_card->size - 1; i >= 0; i--) {
            if (candi_card->data[weight[i].id]->type == Missed && missed_cnt < 2) {
                // prevent discard Missed, reserve at most 2 Missed
                missed_cnt++;
                continue;
            }
            return weight[i].id;
        }
        return weight[candi_card->size - 1].id;
    }

    // if event is play cards
    if (ai_request_type != AI_FORCE_PLAY && weight[0].weight <= 7) {
        // for real thinking time
        i32 card_hard = (600 + 50 * weight[0].weight) / 25;
        if (card_hard < 10) card_hard = 10;
        if (card_hard > 40) card_hard = 40;
        usleep(1000 * (rand() % 1000 + candi_card->size * game->players->size * card_hard) / speed);
        return -1;
    }
    // for real thinking time
    i32 card_hard = (1000 - 50 * weight[0].weight) / 25;
    if (card_hard < 10) card_hard = 10;
    if (card_hard > 40) card_hard = 40;
    usleep(1000 * (rand() % 1000 + candi_card->size * game->players->size * card_hard) / speed);

    ai_target = weight[0].target;
    if (ai->role->type == Traitor && game->players->data[ai_target]->role->type == Sheriff) {
        i32 card_type = candi_card->data[weight[0].id]->type;
        if (card_type == Bang || card_type == Indians || card_type == Duel) {
            hate[player_id][ai_target] -= 2;
        }
    }
    return weight[((0 + ai_respond_error) < candi_card->size ? (0 + ai_respond_error)
                                                             : candi_card->size - 1)]
        .id;
}

i32 ai_player_cnt(Game* game) {
    i32 cnt = 0;
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp > 0) cnt++;
    }
    return cnt;
}

i32 ai_card_weight(Game* game, Cards* cards, i32 ai_id, i32 card_id, i32 max_hate[10],
                   i32 max_hate_id[10]) {
    Player*  ai = game->players->data[ai_id];
    CardType card = cards->data[card_id]->type;
    i32      Sheriff_id;
    i32      player_cnt = ai_player_cnt(game);
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->role->type == Sheriff) Sheriff_id = i;
    }
    i32 max_distance = 1;
    if (ai->weapon != NULL) max_distance += ai->weapon->type - Volcanic;
    max_distance += (game->players->data[ai_id]->scope != NULL) +
                    (game->players->data[ai_id]->character->type == Rose_Doolan);
    if (card == Bang) {
        if (ai_bang_use) {
            return -100;
        }
        int k = max_distance;
        while (k >= 0) {
            ai_target = max_hate_id[k];
            if (distance(game, ai_id, ai_target) <= max_distance) {
                break;
            }
            k--;
        }
        if (k < 0) return -100;

        if (player_cnt > 2 && ai->role->type == Traitor &&
            game->players->data[ai_target]->role->type == Sheriff) {
            if (game->players->data[ai_target]->hp < 4) return 0;
        }
        return max_hate[max_distance] * (6 - game->players->data[ai_target]->hands->size) *
               (6 - game->players->data[ai_target]->hands->size);
    }
    if (card == Missed) return 0;
    if (card == Gatling) {
        if (ai->role->type == Sheriff || ai->role->type == Deputy) return 20;
        if (ai->role->type == Traitor) {
            if (player_cnt <= 2) return 20;
            if (game->players->data[Sheriff_id]->hp >
                (4 + Sheriff_id - ai_id) % game->players->size)
                return 20;
            return 6;
        }
        if (ai->role->type == Criminal) {
            i32 total = 0;
            for (int i = 0; i < game->players->size; i++) {
                if (i != ai_id && hate[ai_id][i] < BAD_GUY) continue;
                if (game->players->data[i]->hands->size < 5) total++;
            }
            if (total > game->players->size / 2) return 20;
            return 6;
        }
    }
    if (card == Indians) {
        i32 total = 0;
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->hp <= 0) continue;
            if (hate[ai_id][i] >= BAD_GUY) {
                total += hate[ai_id][i] * (5 - game->players->data[i]->hands->size);
            }
        }
        return total;
    }
    if (card == Panic || card == Cat_Balou) {
        ai_target = max_hate_id[1];
        if (max_hate[1] < BAD_GUY) return 5;
        return 8 + equip_total(game, ai_id, max_hate_id[1]);
    }
    if (card == Stagecoach) return 5 * (ai->hp - ai->hands->size - 1);
    if (card == Wells_Fargo) return 5 * (ai->hp - ai->hands->size - 2);
    if (card == General_Store) {
        i32 total = 0;
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->hp <= 0) continue;
            if (hate[ai_id][i] < BAD_GUY) {
                total += (i == ai_id ? 5 : 2) * (5 - game->players->data[i]->hands->size);
            }
        }
        return total;
    }
    if (card == Beer) return (ai_player_cnt(game) > 2 ? 5 * (ai->bullet - ai->hp) : 0);
    if (card == Saloon) {
        i32 total = 0;
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->hp <= 0) continue;
            if (hate[ai_id][i] < BAD_GUY) {
                total += (i == ai_id ? 5 : 2) * (5 - game->players->data[i]->hp);
            }
        }
        return total;
    }
    if (card == Duel) {
        i32 ai_bang_cnt = card_count(game, ai_id, Bang);
        ai_target = max_hate_id[9];
        if (max_hate[9] < BAD_GUY) return 0;
        if (ai_bang_cnt < 1) return 0;
        i32 enemy_hands = game->players->data[max_hate_id[9]]->hands->size;
        if (enemy_hands < 2) return 50;
        return (3 * (ai_bang_cnt + 1) - enemy_hands) * (3 * (ai_bang_cnt + 1) - enemy_hands);
    }
    if (card == Barrel) {
        if (ai->barrel != NULL) return -1000;
        return 500;
    }
    if (card == Scope) {
        if (ai->scope != NULL) return -1000;
        return 400;
    }
    if (card == Mustang) {
        if (ai->mustang != NULL) return -1000;
        return 450;
    }
    if (card == Jail) {
        i32 max_jail = 0;
        if (max_hate[9] < BAD_GUY) return 0;
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->hp <= 0 || i == Sheriff_id ||
                (game->players->data[i]->jail != NULL))
                continue;
            if (hate[ai_id][i] >= BAD_GUY && 2 * hate[ai_id][i] > max_jail) {
                max_jail = 2 * hate[ai_id][i];
                ai_target = i;
            }
        }
        return 2 * max_jail;
    }
    if (card == Dynamite) return 10;
    if (card == Volcanic) return 200;
    if (card == Schofield) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_hate[1] >= BAD_GUY)
            return -100;
        return 120 * (1 - 2 * ((ai->weapon != NULL ? ai->weapon->type : 0) > card));
    }
    if (card == Remington) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_hate[1] >= BAD_GUY)
            return -100;
        return 130 * (1 - 2 * ((ai->weapon != NULL ? ai->weapon->type : 0) > card));
    }
    if (card == Rev_Carabine) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_hate[1] >= BAD_GUY)
            return -100;
        return 140 * (1 - 2 * ((ai->weapon != NULL ? ai->weapon->type : 0) > card));
    }
    if (card == Winchester) {
        if (ai->weapon != NULL && ai->weapon->type == Volcanic && max_hate[1] >= BAD_GUY)
            return -100;
        return 150 * (1 - 2 * ((ai->weapon != NULL ? ai->weapon->type : 0) > card));
    }
}

i32 equip_total(Game* game, i32 me_id, i32 player_id) {
    Player* player = game->players->data[player_id];
    Player* my = game->players->data[me_id];
    i32     total = 0;
    if (player->scope != NULL) total += 100;
    if (player->weapon != NULL && player->weapon->type == Schofield)
        total += 120 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    if (player->weapon != NULL && player->weapon->type == Remington)
        total += 130 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    ;
    if (player->weapon != NULL && player->weapon->type == Rev_Carabine)
        total += 140 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    ;
    if (player->weapon != NULL && player->weapon->type == Winchester)
        total += 150 * ((my->weapon != NULL ? my->weapon->type : 0) < player->weapon->type);
    ;
    if (player->weapon != NULL && player->weapon->type == Volcanic) total += 200;
    if (player->mustang != NULL) total += 450;
    if (player->barrel != NULL) total += 500;
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