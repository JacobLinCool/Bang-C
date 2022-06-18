#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H
#include <pthread.h>
#include <semaphore.h>

#include "../utils/all.h"
#include "../web/jsonify.h"
#include "../web/server.h"
#include "ai.h"

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
// [return value] -2: Wrong Player or Disconnected, -1: give up selecting
i32 player_choose_enemy(Game* game, i32 me_id) {
    i32 enemy_id;
    i32 player_size = game->players->size;

    respond_client(game, "choose_enemy", me_id);
    lws_set_timer_usecs(find_client_by_id(me_id)->instance, TIME_OUT_SECONDS * LWS_US_PER_SEC);

    sem_wait(&waiting_for_input);
    enemy_id = share_num;

    if (enemy_id == -1) return -1;
    if (enemy_id == -2) return -2;
    if ((enemy_id < 0 || enemy_id >= player_size) || get_player_hp(game, enemy_id) <= 0 ||
        enemy_id == me_id) {
        enemy_id = -2;
        respond_error(find_client_by_id(me_id), "Wrong Player!\n");
    }

    // determine AI hate value
    ai_hate_change(game, me_id, enemy_id, 1);
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

    // determine AI hate value
    ai_hate_change(game, me_id, ai_target, 1);
    DEBUG_PRINT("ai choose enemy %d\n", ai_target);
    return ai_target;
}

bool real_player_select(Game* game, i32 player_id, Cards* cards) {
    Player* player = game->players->get(game->players, player_id);

    respond_client_with_cards(game, "select_card", player_id, cards);
    lws_set_timer_usecs(find_client_by_id(player_id)->instance, TIME_OUT_SECONDS * LWS_US_PER_SEC);

    sem_wait(&waiting_for_input);
    i64 offset = (i64)share_offset;

    if (offset == -2) {
        return false;
    } else if (offset == -1) {
        respond_error(find_client_by_id(player_id), "You should select a card\n");
        return false;
    }

    i32 input = -1;
    for (int i = 0; i < cards->size; i++) {
        if (cards->data[i] == (Card*)(card_base + offset)) {
            input = i;
            break;
        }
    }

    if (input < 0 || input >= cards->size) {
        respond_error(find_client_by_id(player_id), "Wrong Card!\n");
        return false;
    }

    player->hands->push(player->hands, cards->remove(cards, input));

    return true;
}

bool computer_player_select(Game* game, i32 player_id, Cards* cards) {
    Player* player = game->players->get(game->players, player_id);
    /*i32     random = rand() % cards->size;*/

    i32 choose = ai_request(game, player_id, cards);
    DEBUG_PRINT("player %d select: [%s]\n", player_id, card_name[cards->data[choose]->type]);

    player->hands->push(player->hands, cards->remove(cards, choose));

    return true;
}

Card* computer_player_request(Game* game, i32 player_id);

Card* real_player_request(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);

    respond_client(game, "request_card", player_id);
    lws_set_timer_usecs(find_client_by_id(player_id)->instance, TIME_OUT_SECONDS * LWS_US_PER_SEC);

    sem_wait(&waiting_for_input);
    i64 offset = (i64)share_offset;

    if (offset == -2) {
        return computer_player_request(game, player_id);
    } else if (offset == -1) {
        respond_chat(find_client_by_id(player_id), "You drop selecting the card\n");
        return NULL;
    }
    i32 input = -1;
    for (int i = 0; i < player->hands->size; i++) {
        if (player->hands->data[i] == (Card*)(card_base + offset)) {
            input = i;
            break;
        }
    }

    if (input < 1 || input > player->hands->size) {
        respond_error(find_client_by_id(player_id), "Wrong Card!\n");
        return NULL;
    }
    if (player->hands->size == 1 && player->character->type == Suzy_Lafayette) {
        player_draw_deck(game, player->id, 1);
    }

    return player->hands->remove(player->hands, input);
}

Card* computer_player_request(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);
    // i32     random = rand() % player->hands->size;
    if (ai_request_type == AI_SPECIFY) {
        DEBUG_PRINT("Specify to player %d: [%s]\n", player_id, card_name[ai_request_card]);
        for (int i = 0; i < player->hands->size; i++) {
            if (player->hands->data[i]->type == ai_request_card) {
                DEBUG_PRINT("Return: [%s](id:%d)\n", card_name[player->hands->data[i]->type], i);
                return player->hands->remove(player->hands, i);
            }
        }
        DEBUG_PRINT("Return: [NULL]\n");
        return NULL;
    }
    i32 choose = ai_request(game, player_id, player->hands);
    //  DEBUG_PRINT("Choose: %s\n", choose < 0 ? "NULL" : card_name[choose]);
    if (player->hands->size == 1 && player->character->type == Suzy_Lafayette) {
        player_draw_deck(game, player->id, 1);
    }
    if (choose < 0) return NULL;
    return player->hands->remove(player->hands, choose);
}

Card* computer_player_take(Game* game, i32 player_id, i32 target_id);

Card* real_player_take(Game* game, i32 player_id, i32 target_id) {
    Player* player = game->players->get(game->players, player_id);
    Player* target = game->players->get(game->players, target_id);

    respond_client(game, "take_card", player_id);
    lws_set_timer_usecs(find_client_by_id(player_id)->instance, TIME_OUT_SECONDS * LWS_US_PER_SEC);

    sem_wait(&waiting_for_input);
    i64 offset = (i64)share_offset;

    if (offset == -2) {
        return computer_player_take(game, player_id, target_id);
    } else if (offset == -1) {
        respond_error(find_client_by_id(player_id), "You should choose a card\n");
        return NULL;
    }

    i32 input = -1;
    for (int i = 0; i < target->hands->size; i++) {
        if (target->hands->data[i] == (Card*)(card_base + offset)) {
            input = i;
            break;
        }
    }

    if (0 <= input && input < target->hands->size) {
        if (target->hands->size == 1 && target->character->type == Suzy_Lafayette) {
            player_draw_deck(game, target->id, 1);
        }
        return target->hands->remove(target->hands, input);
    } else {
        if (target->barrel && target->barrel == (Card*)(card_base + offset)) {
            Card* x = target->barrel;
            target->barrel = NULL;
            return x;
        } else if (target->mustang && target->mustang == (Card*)(card_base + offset)) {
            Card* x = target->mustang;
            target->mustang = NULL;
            return x;
        } else if (target->scope && target->scope == (Card*)(card_base + offset)) {
            Card* x = target->scope;
            target->scope = NULL;
            return x;
        } else if (target->weapon && target->weapon == (Card*)(card_base + offset)) {
            Card* x = target->weapon;
            target->weapon = NULL;
            return x;
        } else if (target->jail && target->jail == (Card*)(card_base + offset)) {
            Card* x = target->jail;
            target->jail = NULL;
            return x;
        } else if (target->dynamite && target->dynamite == (Card*)(card_base + offset)) {
            Card* x = target->dynamite;
            target->dynamite = NULL;
            return x;
        }
    }
    respond_error(find_client_by_id(player_id), "Wrong Card!\n");
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
    if (target->hands->size == 1 && target->character->type == Suzy_Lafayette) {
        player_draw_deck(game, target->id, 1);
    }
    i32 random = rand() % target->hands->size;
    return target->hands->remove(target->hands, random);
}

bool computer_player_ramirez(Game* game, i32 player_id);

bool real_player_ramirez(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);

    respond_client(game, "ramirez", player_id);
    lws_set_timer_usecs(find_client_by_id(player_id)->instance, TIME_OUT_SECONDS * LWS_US_PER_SEC);

    sem_wait(&waiting_for_input);
    i32 input = share_num;

    if (input == -2) {
        return computer_player_ramirez(game, player_id);
    }
    if (game->discard->size == 0) {
        respond_error(find_client_by_id(player_id), "Sadly, there is no card to take.\n");
        return false;
    }

    if (input == 1) {
        player->hands->push(player->hands, game->discard->pop(game->discard));
        return true;
    } else {
        return false;
    }

    return false;
}

bool computer_player_ramirez(Game* game, i32 player_id) {
    Player* player = game->players->get(game->players, player_id);

    if (game->discard->size == 0) {
        return false;
    }
    Cards* cards = create_Cards();
    DEBUG_PRINT("Top discard: [%s]\n", card_name[game->discard->back(game->discard)->type]);
    cards->push(cards, game->discard->back(game->discard));
    ai_request_setting(AI_PLAY, 0);
    i32 choose = ai_request(game, player_id, cards);
    if (choose != -1) {
        DEBUG_PRINT("Choose from discard\n");
        player->hands->push(player->hands, game->discard->pop(game->discard));
        return true;
    } else {
        DEBUG_PRINT("Don't choose from discard\n");
        return false;
    }
}

// TODO: player play func
// NOTE: when use the card jail

#endif  // __CORE_PLAYER_H
