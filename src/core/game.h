#ifndef __CORE_GAME_H
#define __CORE_GAME_H

#include "../utils/all.h"
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
    // TODO: shuffle cards, roles, characters
    // TODO: assign card, role, character to players
}

void game_next(Game *game) {
    game->turn++;
    Player *player = game->players->data[game->turn % game->players->size];
    // TODO: determine bomb and jail, may just skip this turn
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
    game->discard = create_Cards();
    game->roles = create_Roles();
    game->characters = create_Characters();

    game->start = game_start;
    game->next = game_next;
    game->join = game_join;

    Event.emit(EVT_GAME_LOAD, game);
    return game;
}

#endif  // __CORE_GAME_H
