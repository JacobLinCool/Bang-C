#ifndef __CORE_UTILS_H
#define __CORE_UTILS_H

#include "../third/cimple/all.h"
#include "constants.h"
#include "types.h"

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
    while (true) {
        if ((me_id + front_dis) % game->players->size == enemy_id) break;

        if (game->players->data[(me_id + front_dis) % game->players->size]->hp > 0) front_dis++;
    }
    i32 back_dis = 1;
    while (true) {
        if ((game->players->size + me_id - back_dis) % game->players->size == enemy_id) break;
        if (game->players->data[(game->players->size + me_id - back_dis) % game->players->size]
                ->hp > 0)
            back_dis++;
    }
    i32 special_card_dis = ((game->players->data[enemy_id]->mustang != NULL) -
                            (game->players->data[me_id]->scope != NULL));
    i32 special_character_dis = (game->players->data[enemy_id]->character->type == Paul_Regret) -
                                (game->players->data[enemy_id]->character->type == Rose_Doolan);
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
bool judge(Game* game, i32 me_id, i32 lower, i32 higher) {
    Card* top_card = get_deck_top(game);
    if (lower <= top_card->priority && top_card->priority <= higher) {
        game->discard->push(game->discard, top_card);
        return SUCCESS;
    }
    game->discard->push(game->discard, top_card);
    if (game->players->data[me_id]->character->type == Lucky_Duke) {
        top_card = get_deck_top(game);
        if (lower <= top_card->priority && top_card->priority <= higher) {
            game->discard->push(game->discard, top_card);
            return SUCCESS;
        }
        game->discard->push(game->discard, top_card);
    }
    return FAIL;
}

// Why not just draw from the first card? It's for FUN!
bool draw_from_player(Game* game, i32 me_id, i32 enemy_id) {
    // player died?
    if (game->players->data[enemy_id]->hp <= 0) return FAIL;

    i8 total_card = game->players->data[enemy_id]->hands->size;
    if (total_card == 0) return SUCCESS;
    i8 me_choose;  //=func(game,me_id,total_card) View layer Todo: call function
    game->players->data[me_id]->hands->push(
        game->players->data[me_id]->hands,
        game->players->data[enemy_id]->hands->data[me_choose - 1]);
    game->players->data[enemy_id]->hands->remove(game->players->data[enemy_id]->hands,
                                                 me_choose - 1);
    return SUCCESS;
}

void recover(Game* game, i32 me_id) {
    if (game->players->data[me_id]->hp == game->players->data[me_id]->character->health +
                                              (game->players->data[me_id]->role->type == Sheriff))
        return;
    game->players->data[me_id]->hp++;
    return;
}

i32 get_player_hp(Game* game, i32 id) { return game->players->data[id]->hp; }

#endif  // __CORE_UTILS_H
