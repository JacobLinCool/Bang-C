#ifndef __CORE_GAME_H
#define __CORE_GAME_H

#include "../third/cimple/all.h"
#include "../utils/all.h"
#include "../web/jsonify.h"
#include "../web/server.h"
#include "cards.h"
#include "player.h"
#include "utils.h"

i32 debug_num = 0;
i32 debug_stop = 0;

void print_status(Game *game, FILE *fp);
bool equip_weapon(Game *game, i32 player_id, Card *card);
void game_start(Game *game);
void game_loop(Game *game);
void game_win(Game *game);

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

void game_loop(Game *game) {
    while (game->finished == false) {
        game->next(game);
        for (int i = 0; i < game->players->size; i++) {
            if (!game->players->data[i]->dead && game->players->data[i]->hp <= 0)
                died_player(game, i, i);
        }
        game->turn++;
    }
    game_win(game);
    pthread_exit(NULL);
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
}

void game_next(Game *game) {
#ifdef DEBUG
    FILE *fp = stdout;  // fopen(stdout, "w+");
#endif

    // P2S game status
    respond_all(game, "status");

    Player *player = game->players->data[game->turn % game->players->size];
    // if player has died, then skip.
    i32 t = 0;
    while (player->hp <= 0) {
        game->turn++;
        player = game->players->data[(game->turn) % game->players->size];
    }
    Console.info("Turn %" PRIu64, game->turn);

    DEBUG_PRINT("It's player %d turn!!!\n", player->id);
    // P2S someone round start
    respond_all(game, "round_start");
    respond_all_chat($(String.format("It's %s's turn!", player->name)));

    // determine bomb and jail, may just skip this turn
    if (player->dynamite != NULL) {
        DEBUG_PRINT("judge: dynamite\n");
        respond_all_chat("Will the dynamite explode?");
        dynamite_judge(game, player->id);
    }
    if (player->hp <= 0) return;
    if (player->jail != NULL) {
        DEBUG_PRINT("judge: jail\n");
        respond_all_chat($(String.format("Will %s escapes from the jail?", player->name)));
        if (!jail_judge(game, player->id)) {
            return;
        }
    }

    // 1.Draw two cards
    DEBUG_PRINT("1.Draw two cards\n");
    if (player->character->type == Black_Jack) {
        player_draw_deck(game, player->id, 1);
        Card *second_card = get_deck_top(game);
        player->hands->push(player->hands, second_card);
        respond_all_chat($(String.format("%s used Balck Jack's skill", player->name)));

        // P2S Black_Jack show second card
        respond_all_with_card(game, "show card", second_card);

        if (second_card->priority / 100 == 2 || second_card->priority / 100 == 3) {
            respond_all_chat(
                $(String.format("%s's second card is red, gets three cards", player->name)));
            player_draw_deck(game, player->id, 1);
        } else {
            respond_all_chat(
                $(String.format("%s's second card is black, gets two cards", player->name)));
        }
    } else if (player->character->type == Kit_Carlson) {
        respond_all_chat($(String.format("%s uses Kit Carlson's skill", player->name)));
        Cards *cards = create_Cards();
        for (int i = 0; i < 3; i++) cards->push(cards, get_deck_top(game));
        ai_request_setting(AI_FORCE_PLAY, 0);
        while (player->select(game, player->id, cards) == false)
            ;

        // P2S game status
        respond_all(game, "status");

        ai_request_setting(AI_FORCE_PLAY, 0);
        while (player->select(game, player->id, cards) == false)
            ;

        Card *debug_card = cards->pop(cards);
        for (int i = 0; i < game->deck->size; i++) {
            if (game->deck->data[i] == NULL) {
                DEBUG_PRINT("NULLLLLLLLL!\n");
                exit(1);
            }
        }
        game->deck->insert(game->deck, 0, debug_card);
        cards->free(cards);
        respond_all_chat($(String.format("%s gets two cards", player->name)));

    } else if (player->character->type == Pedro_Ramirez) {
        ai_bang_use = 0;
        player_draw_deck(game, player->id, 2 - player->ramirez(game, player->id));
        respond_all_chat($(String.format("%s gets two cards", player->name)));
    } else if (player->character->type == Jesse_Jones) {
        i32 enemy_id = -2;
        while (enemy_id < -1) {
            enemy_id = player->choose_enemy(game, player->id);
        }
        if (enemy_id == -1) {
            player_draw_deck(game, player->id, 2);
        } else {
            // choose an enemy
            Player *target = game->players->data[enemy_id];
            i32     random = rand() % target->hands->size;
            Card   *enemy_card = target->hands->remove(target->hands, random);
            player->hands->push(player->hands, enemy_card);
            player_draw_deck(game, player->id, 1);
        }
    } else {
        player_draw_deck(game, player->id, 2);
        respond_all_chat($(String.format("%s gets two cards", player->name)));
    }
    respond_all(game, "status");

#ifdef DEBUG

    // fprintf(fp, "after draw card:\n");
    // print_status(game, fp);
#endif
    // 2.Play any number of cards
    i8 bang_used = 0;
    ai_bang_use = 0;
    while (true) {
        DEBUG_PRINT("player %d, choose your card\n", player->id);
        ai_request_setting(AI_PLAY, 0);
        Card *select_card = player->request(game, player->id);

        if (select_card == NULL) {
            break;
        }
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

                // P2S player error use
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
            // P2S player error use
        }
        DEBUG_PRINT("Using Done\n");
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
        // if (debug_num++ >= debug_stop) getchar();
#endif
    }
    //  3.Discard excess cards
    DEBUG_PRINT("Now: Discard cards.\n");

    i32 discard_cnt = 0;

    while (1) {
        ai_request_setting(AI_DISCARD, 0);
        Card *select_card = player->request(game, player->id);
        DEBUG_PRINT("Discard: %s\n", select_card == NULL ? "NULL" : card_name[select_card->type]);
        if (select_card == NULL && player->hands->size <= player->hp) {
            break;
        }
        if (select_card != NULL) {
            discard_cnt++;
            game->discard->push(game->discard, select_card);
            if (player->character->type == Sid_Ketchum && discard_cnt % 2 == 0) {
                if (player->hp < player->character->health + (player->role->type == Sheriff))
                    player->hp++;
            }
        }
    }
#if (DEBUG)
    fprintf(fp, "after discard card:\n");
    print_status(game, fp);
    fflush(fp);
    printf("Enter any key to continue.(%d)\n", debug_num);
    // if (debug_num++ >= debug_stop) getchar();
#endif
}

void game_win(Game *game) {
    i32 live_player[5] = {0};
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp > 0) {
            live_player[game->players->data[i]->role->type]++;
        }
    }
    if (live_player[Sheriff] == 0) {
        if (live_player[Criminal] == 0 && live_player[Deputy] == 0) {
            Console.green("Traitor win!");
            respond_all_end(game, "end", Traitor);
        } else {
            Console.green("Criminal win!");
            respond_all_end(game, "end", Criminal);
        }
    } else {
        Console.green("Sheriff and Deputy win!");
        respond_all_end(game, "end", Sheriff);
    }
}

Game *new_game() {
    Game *game = $(calloc(1, sizeof(Game)));

    game->players = create_Players();
    game->turn = 0;
    game->finished = false;
    game->deck = create_Cards();
    card_base = (u64)(&decks[0]);
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
        if (enemy_id < 0) return FAIL;
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
