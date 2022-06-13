#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H
#include "../utils/all.h"
#include "ai.h"
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

// choose enemy, if not vaild people, return -1
i32 player_choose_enemy(Game* game, i32 me_id) {
    i32 enemy_id;
    i32 plyaer_size = game->players->size;
    printf("Choose enemy: \n");
    printf("-1)cancel\n");
    for (i32 i = 0, k = 1; i < plyaer_size; i++) {
        if (me_id != i && get_player_hp(game, i) > 0) {
            printf("%d)Player %d hp: %d\n", k++, i, get_player_hp(game, i));
        }
    }
    scanf("%d", &enemy_id);
    if (enemy_id < plyaer_size && get_player_hp(game, enemy_id) > 0 && enemy_id != me_id) {
        enemy_id = -1;
        printf("Wrong Player id!\n");
    }
    // determine AI disgust value
    ai_disgust_change(enemy_id, me_id, 1);
    return enemy_id;
}

i32 computer_choose_enemy(Game* game, i32 me_id) {
    /*i32 enemy_id;
    i32 plyaer_size = game->players->size;
    while (1) {
        enemy_id = rand() % plyaer_size;
        if (enemy_id != me_id && get_player_hp(game, enemy_id) > 0) break;
    }
    return enemy_id;*/

    // determine AI disgust value
    ai_disgust_change(ai_target, me_id, 1);
    return ai_target;
}

bool real_player_select(Game* game, i32 player_id, Cards* cards) {
    Player* player = game->players->get(game->players, player_id);

    Console.cyan("Please select a card: ");
    for (i32 i = 0; i < cards->size; i++) {
        Console.yellow("%d. %s\n", i + 1, card_name[cards->data[i]->type]);
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
    /*i32     random = rand() % cards->size;*/

    i32 choose = ai_request(game, player_id, cards);

    player->hands->push(player->hands, cards->remove(cards, choose));

    return true;
}

Card* real_player_request(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);

    Console.cyan("Please select a card from your hand: ");
    for (i32 i = 0; i < player->hands->size; i++) {
        Console.yellow("%d. %s\n", i + 1, card_name[player->hands->data[i]->type]);
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
    // i32     random = rand() % player->hands->size;
    i32 choose = ai_request(game, player_id, player->hands);
    if (choose == -1) return NULL;
    return player->hands->remove(player->hands, choose);
}

Card* real_player_take(Game* game, i32 player_id, i32 target_id) {
    Player* player = game->players->get(game->players, player_id);
    Player* target = game->players->get(game->players, target_id);

    Console.cyan("Please select a card from target: ");
    i32 i = 1;
    for (; i <= player->hands->size; i++) {
        Console.yellow("%d. %s\n", i, card_name[0]);
    }
    if (player->barrel) Console.yellow("%d. %s\n", i++, card_name[player->barrel->type]);
    if (player->mustang) Console.yellow("%d. %s\n", i++, card_name[player->mustang->type]);
    if (player->scope) Console.yellow("%d. %s\n", i++, card_name[player->scope->type]);
    if (player->weapon) Console.yellow("%d. %s\n", i++, card_name[player->weapon->type]);
    if (player->jail) Console.yellow("%d. %s\n", i++, card_name[player->jail->type]);
    if (player->dynamite) Console.yellow("%d. %s\n", i++, card_name[player->dynamite->type]);

    i32 input = 0;
    scanf("%d", &input);
    if (input < 1 || input >= i) {
        return NULL;
    }

    if (input <= player->hands->size) {
        return player->hands->remove(player->hands, input - 1);
    } else {
        input -= player->hands->size;

        if (input == 0 && player->barrel) {
            Card* x = player->barrel;
            player->barrel = NULL;
            return x;
        } else if (input == 1 && player->mustang) {
            Card* x = player->mustang;
            player->mustang = NULL;
            return x;
        } else if (input == 2 && player->scope) {
            Card* x = player->scope;
            player->scope = NULL;
            return x;
        } else if (input == 3 && player->weapon) {
            Card* x = player->weapon;
            player->weapon = NULL;
            return x;
        } else if (input == 4 && player->jail) {
            Card* x = player->jail;
            player->jail = NULL;
            return x;
        } else if (input == 5 && player->dynamite) {
            Card* x = player->dynamite;
            player->dynamite = NULL;
            return x;
        }
    }

    return NULL;
}

Card* computer_player_take(Game* game, i32 player_id, i32 target_id) {
    Player* player = game->players->get(game->players, player_id);
    Player* target = game->players->get(game->players, target_id);

    if (target->barrel && !player->barrel) {
        Card* x = target->barrel;
        target->barrel = NULL;
        return x;
    } else if (target->mustang && !player->mustang) {
        Card* x = target->mustang;
        target->mustang = NULL;
        return x;
    } else if (target->weapon != NULL) {
        Card* x = target->weapon;
        target->weapon = NULL;
        return x;
    }

    i32 random = rand() % target->hands->size;
    return target->hands->remove(target->hands, random);
}

// TODO: player play func
// NOTE: when use the card jail

#endif  // __CORE_PLAYER_H
