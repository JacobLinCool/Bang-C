#ifndef __CORE_CARD_H
#define __CORE_CARD_H
#define FAIL 1
#include "../utils/all.h"
#include "player.h"
#include "types.h"

Card deck[80] = {{.type = Bang, .priority = 101, .use = NULL},
                 {.type = Bang, .priority = 201, .use = NULL},
                 {.type = Bang, .priority = 212, .use = NULL},
                 {.type = Bang, .priority = 213, .use = NULL},
                 {.type = Bang, .priority = 301, .use = NULL},
                 {.type = Bang, .priority = 302, .use = NULL},
                 {.type = Bang, .priority = 303, .use = NULL},
                 {.type = Bang, .priority = 304, .use = NULL},
                 {.type = Bang, .priority = 305, .use = NULL},
                 {.type = Bang, .priority = 306, .use = NULL},
                 {.type = Bang, .priority = 307, .use = NULL},
                 {.type = Bang, .priority = 308, .use = NULL},
                 {.type = Bang, .priority = 309, .use = NULL},
                 {.type = Bang, .priority = 310, .use = NULL},
                 {.type = Bang, .priority = 311, .use = NULL},
                 {.type = Bang, .priority = 312, .use = NULL},
                 {.type = Bang, .priority = 313, .use = NULL},
                 {.type = Bang, .priority = 402, .use = NULL},
                 {.type = Bang, .priority = 403, .use = NULL},
                 {.type = Bang, .priority = 404, .use = NULL},
                 {.type = Bang, .priority = 405, .use = NULL},
                 {.type = Bang, .priority = 406, .use = NULL},
                 {.type = Bang, .priority = 407, .use = NULL},
                 {.type = Bang, .priority = 408, .use = NULL},
                 {.type = Bang, .priority = 409, .use = NULL},
                 {.type = Missed, .priority = 102, .use = NULL},
                 {.type = Missed, .priority = 103, .use = NULL},
                 {.type = Missed, .priority = 104, .use = NULL},
                 {.type = Missed, .priority = 105, .use = NULL},
                 {.type = Missed, .priority = 106, .use = NULL},
                 {.type = Missed, .priority = 107, .use = NULL},
                 {.type = Missed, .priority = 108, .use = NULL},
                 {.type = Missed, .priority = 401, .use = NULL},
                 {.type = Missed, .priority = 410, .use = NULL},
                 {.type = Missed, .priority = 411, .use = NULL},
                 {.type = Missed, .priority = 412, .use = NULL},
                 {.type = Missed, .priority = 413, .use = NULL},
                 {.type = Gatling, .priority = 210, .use = NULL},
                 {.type = Indians, .priority = 301, .use = NULL},
                 {.type = Indians, .priority = 313, .use = NULL},
                 {.type = Panic, .priority = 201, .use = NULL},
                 {.type = Panic, .priority = 211, .use = NULL},
                 {.type = Panic, .priority = 212, .use = NULL},
                 {.type = Panic, .priority = 308, .use = NULL},
                 {.type = Cat_Balou, .priority = 213, .use = NULL},
                 {.type = Cat_Balou, .priority = 309, .use = NULL},
                 {.type = Cat_Balou, .priority = 310, .use = NULL},
                 {.type = Cat_Balou, .priority = 311, .use = NULL},
                 {.type = Stagecoach, .priority = 109, .use = NULL},
                 {.type = Stagecoach, .priority = 109, .use = NULL},
                 {.type = Wells_Fargo, .priority = 203, .use = NULL},
                 {.type = General_Store, .priority = 112, .use = NULL},
                 {.type = General_Store, .priority = 409, .use = NULL},
                 {.type = Beer, .priority = 206, .use = NULL},
                 {.type = Beer, .priority = 207, .use = NULL},
                 {.type = Beer, .priority = 208, .use = NULL},
                 {.type = Beer, .priority = 209, .use = NULL},
                 {.type = Beer, .priority = 210, .use = NULL},
                 {.type = Beer, .priority = 211, .use = NULL},
                 {.type = Saloon, .priority = 205, .use = NULL},
                 {.type = Duel, .priority = 111, .use = NULL},
                 {.type = Duel, .priority = 312, .use = NULL},
                 {.type = Duel, .priority = 408, .use = NULL},
                 {.type = Barrel, .priority = 112, .use = NULL},
                 {.type = Barrel, .priority = 113, .use = NULL},
                 {.type = Scope, .priority = 101, .use = NULL},
                 {.type = Mustang, .priority = 208, .use = NULL},
                 {.type = Mustang, .priority = 209, .use = NULL},
                 {.type = Jail, .priority = 110, .use = NULL},
                 {.type = Jail, .priority = 111, .use = NULL},
                 {.type = Jail, .priority = 204, .use = NULL},
                 {.type = Dynamite, .priority = 202, .use = NULL},
                 {.type = Volcanic, .priority = 110, .use = NULL},
                 {.type = Volcanic, .priority = 410, .use = NULL},
                 {.type = Schofield, .priority = 113, .use = NULL},
                 {.type = Schofield, .priority = 411, .use = NULL},
                 {.type = Schofield, .priority = 412, .use = NULL},
                 {.type = Remington, .priority = 413, .use = NULL},
                 {.type = Rev_Carabine, .priority = 401, .use = NULL},
                 {.type = Winchester, .priority = 108, .use = NULL}};

// draw a card
Card* draw_one_deck(Game* game) {
    if (game->deck->size == 0) {
        game->deck->concat(game->deck, game->discard);
        game->discard->clear;
    }
    Card* top_card = game->deck->pop(game->deck);
    if (game->deck->size == 0) {
        game->deck->concat(game->deck, game->discard);
        game->discard->clear;
    }
    return top_card;
}
// draw many cards to hands
bool draw_from_deck(Game* game, i32 me_id, i32 time) {
    Player* me = game->players->data[me_id];
    while (time--) {
        me->hands->push(me->hands, draw_one_deck(game));
    }
    return 0;
}
i32 distance(Game* game, i32 me_id, i32 enemy_id) {
    // initial distance (exclude special card)
    i32 front_dis = 1;
    while (1) {
        if ((me_id + front_dis) % game->players->size == enemy_id) break;

        if (game->players->data[(me_id + front_dis) % game->players->size]->hp > 0) front_dis++;
    }
    i32 back_dis = 1;
    while (1) {
        if ((game->players->size + me_id - back_dis) % game->players->size == enemy_id) break;
        if (game->players->data[(game->players->size + me_id - back_dis) % game->players->size]
                ->hp > 0)
            back_dis++;
    }
    i32 special_card_dis = ((game->players->data[enemy_id]->horse != NULL) -
                            (game->players->data[me_id]->scope != NULL));
    return special_card_dis + (front_dis < back_dis ? front_dis : back_dis);
}

bool bang(Game* game, i32 me_id, i32 enemy_id) {
    // enemy died?
    if (game->players->data[enemy_id]->hp <= 0) return FAIL;

    // calculate distance between me and enemy
    i32 enemy_distance = distance(game, me_id, enemy_id);
    // my weapon distance
    i32 weapon_distance = 1;
    if (game->players->data[me_id]->weapon != NULL)
        weapon_distance += game->players->data[me_id]->weapon->type - Volcanic;
    // whether my weapon can reach the enemy

    if (weapon_distance < enemy_distance) return FAIL;
    // switch to enemy, ask if he want to use the card
    // request(game: Game, target: Player, card: CardID): boolean
    if (request(game, enemy_id, Missed)) return 0;
    // Todo: attack
    if (attack_player(game, me_id, enemy_id)) return FAIL;
    return 0;
}

bool panic(Game* game, i32 me_id, i32 enemy_id) {
    // enemy died?
    if (game->players->data[enemy_id]->hp <= 0) return FAIL;
    // calculate distance between me and enemy
    i32 enemy_distance = distance(game, me_id, enemy_id);
    if (enemy_distance > 1) return FAIL;
    // Todo: draw a card from the enemy
    draw_from_player(game, me_id, enemy_id);
    return 0;
}

bool beer(Game* game, i32 me_id) {
    if (game->players->size <= 2) return FAIL;
    if (game->players->data[me_id]->hp == game->players->data[me_id]->character->health +
                                              (game->players->data[me_id]->role->type == Sheriff))
        return FAIL;
    game->players->data[me_id]->hp++;
    return 0;
}
#endif  // __CORE_CARD_H