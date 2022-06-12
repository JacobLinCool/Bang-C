#ifndef __CORE_GAME_H
#define __CORE_GAME_H

#include "../utils/all.h"
#include "cards.h"
#include "characters.h"
#include "constants.h"
#include "player.h"
#include "roles.h"
#include "types.h"

void game_join(Game *game, const char *name, Agent agent) {
    Player *player = $(calloc(1, sizeof(Player)));
    player->name = $(strdup(name));
    player->play = agent;
    game->players->push(game->players, player);
    Event.emit(EVT_GAME_PLAYER_JOIN, &(struct {
                   Game   *game;
                   Player *player;
               }){game, player});
}

void game_start(Game *game) {
    // Todo: bullet initialize
    // shuffle cards, roles, characters
    VectorShuffle(game->deck);
    VectorShuffle(game->roles);
    VectorShuffle(game->characters);
    // assign card, role, character to players
    for (int i = 0; i < game->players->size; i++) {
        Player *nowPlayer = game->players->data[i];
        for (int j = 0; j < nowPlayer->bullet; j++) {
            nowPlayer->hands->push(nowPlayer->hands, game->deck->pop(game->deck));
        }
        nowPlayer->role = game->roles->pop(game->roles);
        nowPlayer->character = game->characters->pop(game->characters);
    }
}

bool repeat_card(Game *game, i32 player_id, Card *select_card) {
    Player *player = game->players->data[player_id];

    // no player can ever have two identical cards face up in front of him.
    if (select_card->type == Barrel && player->barrel != NULL) return true;
    if (select_card->type == Mustang && player->mustang != NULL) return true;
    if (select_card->type == Scope && player->scope != NULL) return true;
    if (select_card->type == Jail && player->jail != NULL) return true;
    if (select_card->type == Dynamite && player->dynamite != NULL) return true;
    return false;
}

void equip_weapon(Game *game, i32 player_id, Card *card) {
    Player *player = game->players->data[player_id];
    if (card->type == Barrel || card->type == Mustang || card->type == Scope) {
        if (card->type == Barrel) player->barrel = card;
        if (card->type == Mustang) player->mustang = card;
        if (card->type == Scope) player->scope = card;
        return;
    }
    if (player->weapon != NULL) {
        game->discard->push(game->discard, player->weapon);
    }
    player->weapon = card;
    return;
}

void game_next(Game *game) {
    game->turn++;
    Player *player = game->players->data[game->turn % game->players->size];
    // if player has died, then skip.
    i32 t = 0;
    while (player->hp <= 0) {
        player = game->players->data[(game->turn + (++t)) % game->players->size];
    }
    // determine bomb and jail, may just skip this turn
    if (player->mustang != NULL) dynamite_judge(game, player->id);
    if (player->hp <= 0) return;
    if (player->jail != NULL) {
        if (!jail_judge(game, player->id)) return;
    }
    Event.emit(EVT_GAME_PLAYER_CHANGED, &(struct {
                   Game   *game;
                   Player *player;
               }){game, player});

    // 1.Draw two cards
    player_draw_deck(game, player->id, 2);

    // 2.Play any number of cards
    bool bang_used = 0;
    while (1) {
        Card *select_card = player->request(game, player->id);
        if (select_card == NULL) break;

        // 1.restriction detection
        if (select_card->type == Bang) {
            // only one BANG! card may be played per turn;
            if (bang_used) {
                player->hands->push(player->hands, select_card);
                continue;
            } else {
                bang_used = true;
            }
        } else if (repeat_card(game, player->id, select_card)) {
            // no player can ever have two identical cards face up in front of him.
            continue;
        }
        // 2.use card
        // (a)blue card
        if (is_weapon(select_card)) {
            equip_weapon(game, player->id, select_card);
            continue;
        }
        // (b)brown card
        if (select_card->use == SUCCESS) game->discard->push(game->discard, select_card);
    }

    //  3.Discard excess cards
    while (player->hands->size > player->hp) {
        Card *select_card = player->request(game, player->id);
        game->discard->push(game->discard, select_card);
    }
}

Game *new_game() {
    Game *game = $(calloc(1, sizeof(Game)));

    game->players = create_Players();
    game->turn = 0;
    game->finished = false;
    game->deck = create_Cards();
    for (size_t i = 0; i < CARD_COUNT; i++) {
        game->deck->push(game->deck, &decks[i]);
    }
    game->discard = create_Cards();
    game->roles = create_Roles();
    for (size_t i = 0; i < ROLE_COUNT; i++) {
        game->roles->push(game->roles, &roles[i]);
    }
    game->characters = create_Characters();
    for (size_t i = 0; i < CHARACTER_COUNT; i++) {
        game->characters->push(game->characters, &chars[i]);
    }

    game->start = game_start;
    game->next = game_next;
    game->join = game_join;

    Event.emit(EVT_GAME_LOAD, game);
    return game;
}

#endif  // __CORE_GAME_H
