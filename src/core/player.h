#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H
#include "../utils/all.h"
#include "cards.h"
#include "constants.h"
#include "game.h"
#include "types.h"
#include "utils.h"

void real_player(Game* game, i32 player_id) {
    printf("We didn't implement anything yet, so just enter anything: ");
    char* input = $(calloc(1024, sizeof(char)));
    scanf("%*[\n]");
    scanf("%[^\n]%*c", input);
}

void computer_player(Game* game, i32 player_id) {
    Console.gray("My name is %s.", game->players->data[player_id]->name);
    Console.gray("I am a computer, I will do nothing.");
}

bool real_player_select(Game* game, i32 player_id, Cards* cards) {
    Player* player = game->players->get(game->players, player_id);

    Console.cyan("Please select a card: ");
    for (i32 i = 0; i < cards->size; i++) {
        Console.yellow("%d. %d\n", i + 1, cards->data[i]->type);
    }
    i32 input = 0;
    scanf("%d", &input);
    if (input < 1 || input > cards->size) {
        return false;
    }

    player->hands->push(player->hands, cards->remove(cards, input - 1));

    return true;
}

bool computer_player_select(Game* game, i32 player_id, Cards* cards) {
    Player* player = game->players->get(game->players, player_id);
    i32     random = rand() % cards->size;

    player->hands->push(player->hands, cards->remove(cards, random));

    return true;
}

Card* real_player_request(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);

    Console.cyan("Please select a card from your hand: ");
    for (i32 i = 0; i < player->hands->size; i++) {
        Console.yellow("%d. %d\n", i + 1, player->hands->data[i]->type);
    }

    i32 input = 0;
    scanf("%d", &input);
    if (input < 1 || input > player->hands->size) {
        return NULL;
    }

    return player->hands->remove(player->hands, input - 1);
}

Card* computer_player_request(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);
    i32     random = rand() % player->hands->size;

    return player->hands->remove(player->hands, random);
}

// TODO: player play func
// NOTE: when use the card jail

#endif  // __CORE_PLAYER_H
