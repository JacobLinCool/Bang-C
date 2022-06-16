#ifndef __CORE_GAME_H
#define __CORE_GAME_H

#include "../utils/all.h"
#include "../web/jsonify.h"
#include "../web/server.h"
#include "cards.h"
#include "constants.h"
#include "player.h"
#include "roles.h"
#include "types.h"

i32 debug_num = 0;
i32 debug_stop = 0;

void print_status(Game *game, FILE *fp);
bool equip_weapon(Game *game, i32 player_id, Card *card);
bool valid_assign_role(Role *role, i32 player_total) {
    if (role == NULL) return false;
    // printf("assign: %s\n", role_name[role->type]);
    if (game_start_role_in_players[player_total - 4][role->type] <= 0) return false;
    game_start_role_in_players[player_total - 4][role->type]--;
    return true;
}
void game_join(Game *game, const char *name, bool is_computer) {
    Player *player = $(calloc(1, sizeof(Player)));
    player->name = $(strdup(name));
    player->hands = create_Cards();
    if (is_computer) {
        player->play = computer_player;
        player->choose_enemy = computer_choose_enemy;
        player->select = computer_player_select;
        player->request = computer_player_request;
        player->take = computer_player_take;
        player->ramirez = computer_player_ramirez;
    } else {
        player->play = real_player;
        player->choose_enemy = player_choose_enemy;
        player->select = real_player_select;
        player->request = real_player_request;
        player->take = real_player_take;
        player->ramirez = real_player_ramirez;
    }
    game->players->push(game->players, player);
    Event.emit(EVT_GAME_PLAYER_JOIN, &(struct {
                   Game   *game;
                   Player *player;
               }){game, player});
}

void game_start(Game *game) {
    // shuffle cards, roles, characters
    VectorShuffle(game->deck);
    VectorShuffle(game->roles);
    VectorShuffle(game->characters);
    // assign card, role, character to players
    for (int i = 0; i < game->players->size; i++) {
        Player *nowPlayer = game->players->data[i];
        while (!valid_assign_role(nowPlayer->role, game->players->size)) {
            nowPlayer->role = game->roles->pop(game->roles);
        }
        nowPlayer->character = game->characters->pop(game->characters);
        nowPlayer->bullet = nowPlayer->character->health + (nowPlayer->role->type == Sheriff);
        nowPlayer->hp = nowPlayer->bullet;
        nowPlayer->dead = false;
        if (nowPlayer->role->type == Sheriff && i != 0) game->players->swap(game->players, 0, i);
        for (int j = 0; j < nowPlayer->bullet; j++) {
            nowPlayer->hands->push(nowPlayer->hands, game->deck->pop(game->deck));
        }
    }
    for (int i = 0; i < game->players->size; i++) {
        Player *nowPlayer = game->players->data[i];
        nowPlayer->id = i;
    }
    // ai initialize
    for (int i = 0; i < game->players->size; i++) {
        ai_initialize(game, i);
    }

    for (int i = 0; i < game->players->size; i++) {
        respond(clients->get(clients, i), "game_start", game_jsonify(game, clients->get(clients, i)->player_id));
    }
}

void game_next(Game *game) {
    FILE *fp;
    fp = fopen("/dev/pts/5", "w+");

    Player *player = game->players->data[game->turn % game->players->size];
    // if player has died, then skip.
    i32 t = 0;
    while (player->hp <= 0) {
        game->turn++;
        player = game->players->data[(game->turn) % game->players->size];
    }
    game->turn++;

    DEBUG_PRINT("It's player %d turn!!!\n", player->id);
    // print_status(game);

    // determine bomb and jail, may just skip this turn
    if (player->dynamite != NULL) {
        DEBUG_PRINT("judge: dynamite\n");
        dynamite_judge(game, player->id);
    }
    if (player->hp <= 0) return;
    if (player->jail != NULL) {
        DEBUG_PRINT("judge: jail\n");
        if (!jail_judge(game, player->id)) {
            return;
        }
    }
    /*Event.emit(EVT_GAME_PLAYER_CHANGED, &(struct {
                   Game   *game;
                   Player *player;
               }){game, player});*/

    // 1.Draw two cards
    DEBUG_PRINT("1.Draw two cards\n");
    if (player->character->type == Black_Jack) {
        player_draw_deck(game, player->id, 1);
        Card *second_card = get_deck_top(game);
        player->hands->push(player->hands, second_card);
        if (second_card->priority / 100 == 2 || second_card->priority / 100 == 3) {
            player_draw_deck(game, player->id, 1);
        }
    } else if (player->character->type == Kit_Carlson) {
        Cards *cards = create_Cards();
        for (int i = 0; i < 3; i++) cards->push(cards, get_deck_top(game));
        ai_request_setting(AI_FORCE_PLAY, 0);
        player->select(game, player->id, cards);
        ai_request_setting(AI_FORCE_PLAY, 0);
        player->select(game, player->id, cards);
        Card *debug_card = cards->pop(cards);
        for (int i = 0; i < game->deck->size; i++) {
            if (game->deck->data[i] == NULL) {
                DEBUG_PRINT("NULLLLLLLLL!\n");
                exit(1);
            }
        }
        game->deck->insert(game->deck, 0, debug_card);
        cards->free(cards);
    } else if (player->character->type == Pedro_Ramirez) {
        ai_bang_use = 0;
        player_draw_deck(game, player->id, 2 - player->ramirez(game, player->id));
    } else {
        player_draw_deck(game, player->id, 2);
    }
    // print_status(game);
#if (DEBUG)
    fprintf(fp, "after draw card:\n");
    print_status(game, fp);
#endif
    // 2.Play any number of cards
    i8 bang_used = 0;
    ai_bang_use = 0;
    while (true) {
        DEBUG_PRINT("player %d, choose your card\n", player->id);
        ai_request_setting(AI_PLAY, 0);
        Card *select_card = player->request(game, player->id);
        if (select_card == NULL) break;
        // 1.restriction detection
        if (select_card->type == Bang) {
            // only one BANG! card may be played per turn
            if (bang_used && player->character->type != Willy_the_Kid) {
                player->hands->push(player->hands, select_card);
                continue;
            } else {
                bang_used++;
                ai_bang_use++;
            }
        }
        // 2.use card
        // (a)blue card
        DEBUG_PRINT("Use: %s\n", card_name[select_card->type]);
        if (is_weapon(select_card)) {
            if (equip_weapon(game, player->id, select_card) == FAIL) {
                DEBUG_PRINT("Error Use\n");
                player->hands->push(player->hands, select_card);
            }
            continue;
        }
        // (b)brown card
        if (select_card->use(game, player->id) == SUCCESS) {
            if (select_card->type == Missed && player->character->type == Calamity_Janet) {
                bang(game, player->id);
            }
            game->discard->push(game->discard, select_card);
        } else {
            DEBUG_PRINT("Error Use\n");
            if (select_card->type == Bang ||
                (select_card->type == Missed && player->character->type == Calamity_Janet)) {
                bang_used--;
                ai_bang_use--;
            }
            player->hands->push(player->hands, select_card);
        }
        // 3.check if someone died(only brown card used)
        for (int i = 0; i < game->players->size; i++) {
            if (!game->players->data[i]->dead && game->players->data[i]->hp <= 0)
                died_player(game, player->id, i);
        }
        if (game->finished) return;
#if (DEBUG)
        fprintf(fp, "after operation:\n");
        print_status(game, fp);
        fflush(fp);
        printf("Enter any key to continue.(%d)\n", debug_num);
        if (debug_num++ >= debug_stop) getchar();
#endif
    }
    //  3.Discard excess cards
    DEBUG_PRINT("Now: Discard cards.\n");

    while (player->hands->size > player->hp) {
        ai_request_setting(AI_DISCARD, 0);
        Card *select_card = player->request(game, player->id);
        DEBUG_PRINT("Discard: %s\n", select_card == NULL ? "NULL" : card_name[select_card->type]);

        if (select_card != NULL) game->discard->push(game->discard, select_card);
    }
#if (DEBUG)
    fprintf(fp, "after discard card:\n");
    print_status(game, fp);
    fflush(fp);
    printf("Enter any key to continue.(%d)\n", debug_num);
    if (debug_num++ >= debug_stop) getchar();
#endif
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

bool equip_weapon(Game *game, i32 player_id, Card *card) {
    Player *player = game->players->data[player_id];
    if (card->type == Barrel || card->type == Mustang || card->type == Scope ||
        card->type == Dynamite) {
        if (card->type == Barrel && player->barrel == NULL) {
            player->barrel = card;
            return SUCCESS;
        }
        if (card->type == Mustang && player->mustang == NULL) {
            player->mustang = card;
            return SUCCESS;
        }
        if (card->type == Scope && player->scope == NULL) {
            player->scope = card;
            return SUCCESS;
        }
        if (card->type == Dynamite && player->dynamite == NULL) {
            player->dynamite = card;
            return SUCCESS;
        }
        return FAIL;
    } else if (card->type == Jail) {
        i32 enemy_id = game->players->data[player_id]->choose_enemy(game, player_id);
        if (enemy_id == -1) return FAIL;
        if (game->players->data[enemy_id]->role->type == Sheriff) return FAIL;
        if (game->players->data[enemy_id]->jail != NULL) return FAIL;
        game->players->data[enemy_id]->jail = card;
        return SUCCESS;
    }
    if (player->weapon != NULL) {
        game->discard->push(game->discard, player->weapon);
    }
    player->weapon = card;
    return SUCCESS;
}

void print_status(Game *game, FILE *fp) {
    fprintf(fp, "------------------------------------------\n");
    for (int i = 0; i < game->players->size; i++) {
        Player *player = game->players->data[i];
        fprintf(fp, "player %d(%s),role: %s,character: %s:\n", player->id, player->name,
                role_name[player->role->type], character_name[player->character->type]);
        fprintf(fp, "hp: %d(max: %d)\n", player->hp, player->bullet);
        if (player->hands == NULL) return;
        fprintf(fp, "Cards(%ld):", player->hands->size);
        for (int i = 0; i < player->hands->size; i++) {
            fprintf(fp, "[%s] ", card_name[player->hands->data[i]->type]);
        }
        fprintf(fp, "\nWeapon: [%s]\n",
                player->weapon == NULL ? "N" : card_name[player->weapon->type]);
        fprintf(fp, "Barrel: %s,", player->barrel == NULL ? "N" : "Y");
        fprintf(fp, "Mustang: %s,", player->mustang == NULL ? "N" : "Y");
        fprintf(fp, "Scope: %s,", player->scope == NULL ? "N" : "Y");
        fprintf(fp, "Jail: %s,", player->jail == NULL ? "N" : "Y");
        fprintf(fp, "Dynamite: %s\n", player->dynamite == NULL ? "N" : "Y");
        fprintf(fp, "------------------------------------------\n");
    }
}
#endif  // __CORE_GAME_H
