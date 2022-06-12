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
    // TODO: get new card
    player->play(game, game->turn % game->players->size);
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
