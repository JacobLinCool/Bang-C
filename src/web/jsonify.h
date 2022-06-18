#ifndef __JSONIFY_H
#define __JSONIFY_H

#ifdef DEBUG
#define DEBUG_DISPLAY true
#else
#define DEBUG_DISPLAY false
#endif

#include <pthread.h>

#include "../core/constants.h"
#include "../core/types.h"
#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"

cJSON *card_jsonlfy(Card *card, bool itself) {
    if (card == NULL) return NULL;
    cJSON *root = cJSON_CreateObject();

    cJSON *offset = cJSON_CreateNumber((u64)card - card_base);  // this may leak some informations
    cJSON *type = cJSON_CreateNumber(itself ? card->type : 0);
    cJSON *priority = cJSON_CreateNumber(itself ? card->priority : 0);

    cJSON_AddItemToObject(root, "x", offset);
    cJSON_AddItemToObject(root, "type", type);
    cJSON_AddItemToObject(root, "priority", priority);

    return root;
}

cJSON *player_jsonify(Player *player, bool itself) {
    if (!player) return NULL;
    cJSON *root = cJSON_CreateObject();

    cJSON *name = cJSON_CreateString(player->name);
    cJSON *id = cJSON_CreateNumber(player->id);
    cJSON *hp = cJSON_CreateNumber(player->hp);
    cJSON *role =
        cJSON_CreateNumber((itself || player->role->type == Sheriff) ? player->role->type : 0);
    cJSON *character = cJSON_CreateNumber(player->character->type);
    cJSON *weapon = card_jsonlfy(player->weapon, true);
    cJSON *barrel = card_jsonlfy(player->barrel, true);
    cJSON *mustang = card_jsonlfy(player->mustang, true);
    cJSON *scope = card_jsonlfy(player->scope, true);
    cJSON *jail = card_jsonlfy(player->jail, true);
    cJSON *dynamite = card_jsonlfy(player->dynamite, true);
    cJSON *hands = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "name", name);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "hp", hp);
    cJSON_AddItemToObject(root, "role", role);
    cJSON_AddItemToObject(root, "character", character);
    cJSON_AddItemToObject(root, "weapon", weapon);
    cJSON_AddItemToObject(root, "barrel", barrel);
    cJSON_AddItemToObject(root, "mustang", mustang);
    cJSON_AddItemToObject(root, "scope", scope);
    cJSON_AddItemToObject(root, "jail", jail);
    cJSON_AddItemToObject(root, "dynamite", dynamite);
    cJSON_AddItemToObject(root, "hands", hands);

    for (i32 i = 0; i < player->hands->size; i++) {
        Card  *cur_card = player->hands->get(player->hands, i);
        cJSON *json_card = card_jsonlfy(cur_card, itself);
        cJSON_AddItemToArray(hands, json_card);
    }

    return root;
}

Client *find_client_by_id(int id) {
    for (int i = 0; i < clients->size; i++) {
        Client *client = clients->data[i];
        if (client->player_id == id) {
            return client;
        }
    }
    return NULL;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

cJSON *game_jsonify(Game *game, i32 player_id) {
    pthread_mutex_lock(&mutex);
    if (!game) {
        return NULL;
    }

    cJSON *root = cJSON_CreateObject();

    cJSON *players = cJSON_CreateArray();
    cJSON *turn = cJSON_CreateNumber(game->turn);
    cJSON *finished = cJSON_CreateBool(game->finished);
    cJSON *deck_size = cJSON_CreateNumber(game->deck->size);
    cJSON *discards = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "players", players);
    cJSON_AddItemToObject(root, "turn", turn);
    cJSON_AddItemToObject(root, "finished", finished);
    cJSON_AddItemToObject(root, "deck_size", deck_size);
    cJSON_AddItemToObject(root, "discards", discards);

    for (i32 i = 0; i < game->players->size; i++) {
        Player *cur_player = game->players->get(game->players, i);
        cJSON  *json_player = player_jsonify(cur_player, (player_id == i) || DEBUG_DISPLAY);

        cJSON_AddItemToArray(players, json_player);
    }

    for (i32 i = 0; i < game->discard->size; i++) {
        Card  *cur_discard = game->discard->get(game->discard, i);
        cJSON *json_discard = card_jsonlfy(cur_discard, true);
        cJSON_AddItemToArray(discards, json_discard);
    }

    pthread_mutex_unlock(&mutex);
    return root;
}

cJSON *cards_jsonify(Game *game, i32 player_id, Cards *pool) {
    cJSON *root = cJSON_CreateObject();

    cJSON *cards = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "cards", cards);

    for (i32 i = 0; i < pool->size; i++) {
        Card  *cur_card = pool->get(pool, i);
        cJSON *json_card = card_jsonlfy(cur_card, true);
        cJSON_AddItemToArray(cards, json_card);
    }

    return root;
}

void respond_all(Game *game, char *type) {
    for (int i = 0; i < clients->size; i++) {
        cJSON *base = cJSON_CreateObject();
        cJSON_AddItemToObject(base, "game",
                              game_jsonify(game, clients->get(clients, i)->player_id));
        respond(clients->get(clients, i), type, base);
    }
}

void respond_all_end(Game *game, char *type, i32 winner) {
    for (int i = 0; i < clients->size; i++) {
        cJSON *base = cJSON_CreateObject();
        cJSON *win = cJSON_CreateNumber(winner);

        cJSON_AddItemToObject(base, "winner", win);
        cJSON_AddItemToObject(base, "game",
                              game_jsonify(game, clients->get(clients, i)->player_id));
        respond(clients->get(clients, i), type, base);
    }
}

void respond_client(Game *game, char *type, i32 player_id) {
    cJSON *base = cJSON_CreateObject();
    cJSON_AddItemToObject(base, "game", game_jsonify(game, player_id));
    Client *client = find_client_by_id(player_id);
    if (client != NULL) {
        respond(client, type, base);
    }
}

void respond_client_with_cards(Game *game, char *type, i32 player_id, Cards *cards) {
    cJSON *base = cJSON_CreateObject();
    cJSON_AddItemToObject(base, "cards", cards_jsonify(game, player_id, cards));
    cJSON_AddItemToObject(base, "game", game_jsonify(game, player_id));
    Client *client = find_client_by_id(player_id);
    if (client != NULL) {
        respond(client, type, base);
    }
}

#endif  // __JSONIFY_H
