#ifndef __CORE_GAME_H
#define __CORE_GAME_H

#include "../utils/all.h"
#include "types.h"

void game_join(Game *game, const char *name, Agent agent) {
    Player *player = $(calloc(1, sizeof(Player)));
    player->name = $(strdup(name));
    player->play = agent;
    game->players->push(game->players, player);
}

void game_start(Game *game) {
    // TODO: shuffle cards, roles, characters
    // TODO: assign card, role, character to players

    Event.emit($(String.format("%s-start", game->name)), (void *)&(GameEvent){GAME_START, game});
}

void game_next(Game *game) {
    game->turn++;
    char *event = $(String.format("%s-player-changed", game->name));
    Event.emit(event, (void *)&(GameEvent){PLAYER_CHANGED, game});
    // TODO: determine bomb and jail, may just skip this turn
    // TODO: get new card
    game->players->data[game->turn % game->players->size]->play(game,
                                                                game->turn % game->players->size);
}

Game *new_game(const char *name) {
    Game *game = $(calloc(1, sizeof(Game)));

    game->name = $(strdup(name));
    game->players = create_Players();
    game->turn = 0;
    game->finished = false;
    game->start = game_start;
    game->next = game_next;
    game->join = game_join;

    return game;
}

#endif  // __CORE_GAME_H
