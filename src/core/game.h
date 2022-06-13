#ifndef __CORE_GAME_H
#define __CORE_GAME_H

#include "../utils/all.h"
#include "cards.h"
#include "constants.h"
#include "player.h"
#include "roles.h"
#include "types.h"

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
    } else {
        player->play = real_player;
        player->choose_enemy = player_choose_enemy;
        player->select = real_player_select;
        player->request = real_player_request;
        player->take = real_player_take;
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
        nowPlayer->role = game->roles->pop(game->roles);
        nowPlayer->character = game->characters->pop(game->characters);
        nowPlayer->bullet = nowPlayer->character->health + (nowPlayer->role->type == Sheriff);
        nowPlayer->hp = nowPlayer->bullet;
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
    if (player->dynamite != NULL) {
        if (dynamite_judge(game, player->id)) {
            Card *card = player->dynamite;
            player->dynamite = NULL;
            game->discard->push(game->discard, card);
        }
    }
    if (player->hp <= 0) return;
    if (player->jail != NULL) {
        Card *card = player->jail;
        player->jail = NULL;
        game->discard->push(game->discard, card);
        if (!jail_judge(game, player->id)) {
            return;
        }
    }
    Event.emit(EVT_GAME_PLAYER_CHANGED, &(struct {
                   Game   *game;
                   Player *player;
               }){game, player});

    // 1.Draw two cards
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
        player->select(game, player->id, cards);
        player->select(game, player->id, cards);
        game->deck->insert(game->deck, 0, cards->pop(cards));
        cards->free(cards);
    } else if (player->character->type == Pedro_Ramirez) {
    } else {
        player_draw_deck(game, player->id, 2);
    }

    // 2.Play any number of cards
    bool bang_used = 0;
    while (true) {
        play_or_discard = AI_PLAY;
        Card *select_card = player->request(game, player->id);
        if (select_card == NULL) break;

        // 1.restriction detection
        if (select_card->type == Bang) {
            // only one BANG! card may be played per turn
            if (bang_used && player->character->type != Willy_the_Kid) {
                player->hands->push(player->hands, select_card);
                continue;
            } else {
                bang_used = true;
            }
        }
        // 2.use card
        // (a)blue card
        if (is_weapon(select_card)) {
            equip_weapon(game, player->id, select_card);
            continue;
        }
        // (b)brown card
        if (select_card->use(game, player->id) == SUCCESS) {
            if (select_card->type == Missed && player->character->type == Calamity_Janet) {
                bang(game, player->id);
            }
            game->discard->push(game->discard, select_card);
        }

        // 3.check if someone died(only brown card used)
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->hp <= 0) died_player(game, player->id, i);
        }
    }

    //  3.Discard excess cards
    while (player->hands->size > player->hp) {
        play_or_discard = AI_DISCARD;
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
