#ifndef __CORE_UTILS_H
#define __CORE_UTILS_H

#include "../third/cimple/all.h"
#include "constants.h"
#include "types.h"
bool El_Gringo_active = false;

void check_deck(Game* game) {
    for (int i = 0; i < game->deck->size; i++) {
        if (game->deck->data[i] == NULL) {
            DEBUG_PRINT("NULL on deck[%d]\n", i);
            exit(1);
        }
    }
}

void check_discard(Game* game) {
    for (int i = 0; i < game->discard->size; i++) {
        if (game->discard->data[i] == NULL) {
            DEBUG_PRINT("NULL on discard[%d]\n", i);
            exit(1);
        }
    }
}

/**
 * @brief Transfer all cards from a Cards to another Cards.
 *
 * @param from source
 * @param to dist
 */
void transfer(Cards* from, Cards* to) {
    while (from->size) to->push(to, from->pop(from));
}

/**
 * @brief Calculate the distance between two players.
 *
 * @param game
 * @param me_id
 * @param enemy_id
 * @return i32
 */
i32 distance(Game* game, i32 me_id, i32 enemy_id) {
    // initial distance (exclude special card)
    i32 front_dis = 1;
    i32 step = 1;
    while (true) {
        if ((me_id + step) % game->players->size == enemy_id) break;
        if (game->players->data[(me_id + step) % game->players->size]->hp > 0) front_dis++;
        step++;
    }
    i32 back_dis = 1;
    step = 1;
    while (true) {
        if ((game->players->size + me_id - step) % game->players->size == enemy_id) break;
        if (game->players->data[(game->players->size + me_id - step) % game->players->size]->hp > 0)
            back_dis++;
        step++;
    }
    i32 special_card_dis = ((game->players->data[enemy_id]->mustang != NULL) -
                            (game->players->data[me_id]->scope != NULL));
    i32 special_character_dis = (game->players->data[enemy_id]->character->type == Paul_Regret) -
                                (game->players->data[me_id]->character->type == Rose_Doolan);
    // DEBUG_PRINT("f:%d,b:%d,card:%d,char:%d\n", front_dis, back_dis, special_card_dis,
    //             special_character_dis);
    return special_character_dis + special_card_dis + (front_dis < back_dis ? front_dis : back_dis);
}

/**
 * @brief Check if all players of the role is alive.
 *
 * @param game
 * @param role The role to check
 * @return true Someone is alive
 * @return false Nobody is alive
 */
bool alive(Game* game, RoleType role) {
    bool exist = false;

    for (i32 i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->role->type == role && game->players->data[i]->hp > 0) {
            exist = true;
            break;
        }
    }

    return exist;
}

/**
 * @brief Get the top card of the deck.
 *
 * @param game
 * @return Card*
 */
Card* get_deck_top(Game* game) {
    check_discard(game);
    if (game->deck->size == 0) {
        transfer(game->discard, game->deck);
        game->deck->shuffle(game->deck);
    }
    Card* top_card = game->deck->pop(game->deck);
    return top_card;
}

// draw many cards to hands
bool player_draw_deck(Game* game, i32 me_id, i32 count) {
    Player* me = game->players->data[me_id];
    while (count--) {
        me->hands->push(me->hands, get_deck_top(game));
        respond_all(game, "status");
    }
    return 0;
}

/**
 * @brief Draw card(s) from deck and judge the result.
 *
 * @param game
 * @param me_id
 * @param lower
 * @param higher
 * @return true
 * @return false
 */
bool judge(Game* game, i32 me_id, i32 lower, i32 higher, CardType type) {
    Card* top_card = get_deck_top(game);
    respond_all(game, "status");
    CharacterType char_type = game->players->data[me_id]->character->type;
    DEBUG_PRINT("result: %u,", top_card->priority);
    if (lower <= top_card->priority && top_card->priority <= higher) {
        DEBUG_PRINT("In range\n");
        if (char_type == Lucky_Duke) {
            if (type != Dynamite) {
                game->discard->push(game->discard, top_card);
                respond_all(game, "status");
                return SUCCESS;
            }
        } else {
            game->discard->push(game->discard, top_card);
            respond_all(game, "status");
            return SUCCESS;
        }
    } else if (char_type == Lucky_Duke && type == Dynamite) {
        game->discard->push(game->discard, top_card);
        respond_all(game, "status");
        return FAIL;
    }
    game->discard->push(game->discard, top_card);
    if (char_type == Lucky_Duke) {
        top_card = get_deck_top(game);
        respond_all(game, "status");
        if (lower <= top_card->priority && top_card->priority <= higher) {
            game->discard->push(game->discard, top_card);
            respond_all(game, "status");
            DEBUG_PRINT("In range\n");
            return SUCCESS;
        }
        game->discard->push(game->discard, top_card);
    }
    DEBUG_PRINT("Out of range\n");
    respond_all(game, "status");
    return FAIL;
}

// Why not just draw from the first card? It's for FUN!
bool draw_from_player(Game* game, i32 me_id, i32 enemy_id) {
    Player* me = game->players->data[me_id];
    Player* enemy = game->players->data[enemy_id];
    if (enemy->hp <= 0) return FAIL;

    El_Gringo_active = false;
    Card* selected = NULL;
    while (!selected) {
        selected = me->take(game, me_id, enemy_id);
        respond_all(game, "status");
    }
    DEBUG_PRINT("%d->%d select [%s]\n", me_id, enemy_id, card_name[selected->type]);

    me->hands->push(me->hands, selected);
    respond_all(game, "status");

    return SUCCESS;
}

// Why not just draw from the first card? It's for FUN!
bool discard_from_enemy(Game* game, i32 me_id, i32 enemy_id) {
    Player* me = game->players->data[me_id];
    Player* enemy = game->players->data[enemy_id];
    if (enemy->hp <= 0) return FAIL;

    El_Gringo_active = false;
    Card* selected = NULL;
    while (!selected) {
        selected = me->take(game, me_id, enemy_id);
        respond_all(game, "status");
    }
    DEBUG_PRINT("%d->%d: select [%s]\n", me_id, enemy_id, card_name[selected->type]);

    game->discard->push(game->discard, selected);
    respond_all(game, "status");

    return SUCCESS;
}

void recover(Game* game, i32 me_id) {
    if (game->players->data[me_id]->hp == game->players->data[me_id]->character->health +
                                              (game->players->data[me_id]->role->type == Sheriff))
        return;
    game->players->data[me_id]->hp++;
    respond_all(game, "status");
    return;
}

i32 get_player_hp(Game* game, i32 id) { return game->players->data[id]->hp; }

#endif  // __CORE_UTILS_H
