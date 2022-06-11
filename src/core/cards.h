#ifndef __CORE_CARD_H
#define __CORE_CARD_H
#define FAIL 0
#define SUCCESS 1
#include "../utils/all.h"
#include "player.h"
#include "types.h"

// draw a card
Card* draw_one_deck(Game* game) {
    if (game->deck->size == 0) {
        game->deck->concat(game->deck, game->discard);
        VectorShuffle(game->deck);
        game->discard->clear;
    }
    Card* top_card = game->deck->pop(game->deck);
    if (game->deck->size == 0) {
        game->deck->concat(game->deck, game->discard);
        VectorShuffle(game->deck);
        game->discard->clear;
    }
    return top_card;
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

// Todo: bang can only use once, request
bool bang(Game* game, i32 me_id, i32 enemy_id) {
    // is enemy = me?
    if (me_id == enemy_id) return FAIL;
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
    // if (request(game, enemy_id, Missed)) return 0;
    return attack_player(game, me_id, enemy_id);
}

bool gatling(Game* game, i32 me_id) {
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp <= 0 || me_id == i) continue;
        bang(game, me_id, i);
    }
    return SUCCESS;
}

// Todo: request
bool indians(Game* game, i32 me_id) {
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp <= 0 || me_id == i) continue;
        // if(request(game, i, Bang))continue;
        attack_player(game, me_id, i);
    }
    return SUCCESS;
}

bool panic(Game* game, i32 me_id, i32 enemy_id) {
    // is enemy = me?
    if (me_id == enemy_id) return FAIL;
    // enemy died?
    if (game->players->data[enemy_id]->hp <= 0) return FAIL;
    // calculate distance between me and enemy
    i32 enemy_distance = distance(game, me_id, enemy_id);
    if (enemy_distance > 1) return FAIL;
    // Todo: draw a card from the enemy
    draw_from_player(game, me_id, enemy_id);
    return SUCCESS;
}

bool stagecoach(Game* game, i32 me_id) {
    player_draw_deck(game, me_id, 2);
    return SUCCESS;
}

bool wells_fargo(Game* game, i32 me_id) {
    player_draw_deck(game, me_id, 3);
    return SUCCESS;
}

bool beer(Game* game, i32 me_id) {
    if (game->players->size <= 2) return FAIL;
    if (game->players->data[me_id]->hp == game->players->data[me_id]->character->health +
                                              (game->players->data[me_id]->role->type == Sheriff))
        return FAIL;
    game->players->data[me_id]->hp++;
    return SUCCESS;
}

bool saloon(Game* game, i32 me_id) {
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp <= 0) continue;
        beer(game, i);
    }
    return SUCCESS;
}
#endif  // __CORE_CARD_H