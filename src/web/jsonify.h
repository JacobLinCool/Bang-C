#ifndef __JSONIFY_H
#define __JSONIFY_H

#ifdef DEBUG
#define DEBUG_DISPLAY true
#else
#define DEBUG_DISPLAY false
#endif

#include "../core/cards.h"
#include "../core/types.h"
#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"

/*
card_json
{
    "address": number,
    "type": number, //0 is hidden
    "pirority": number
}
*/
cJSON *card_jsonlfy(Card **card, bool itself) {
    if (!(*card)) return NULL;
    cJSON *root = cJSON_CreateObject();

    u64    address = (u64)card;
    cJSON *pointer = cJSON_CreateNumber(address);
    cJSON *type = cJSON_CreateNumber(itself ? (*card)->type : -1);
    cJSON *priority = cJSON_CreateNumber((*card)->priority);

    cJSON_AddItemToObject(root, "address", pointer);
    cJSON_AddItemToObject(root, "type", type);
    cJSON_AddItemToObject(root, "priority", priority);

    return root;
}

/*
player json
{
    "name": string,
    "id": number,
    "hp": number,
    "dead": boolean,
    "role": number,//-1 is hidden
    "character": number,
    "weapon": boolean,
    "barrel": boolean,
    "mustang": boolean,
    "scope": boolean,
    "tail": boolean,
    "dynamite": boolean,
    "hands_size": number
    "display_hands" boolean
    "hands": [
        card_json,
        card_json,
        .
        .
    ]
}
*/
cJSON *player_jsonify(Player *player, bool itself) {
    if (!player) return NULL;
    cJSON *root = cJSON_CreateObject();

    cJSON *name = cJSON_CreateString(player->name);
    cJSON *id = cJSON_CreateNumber(player->id);
    cJSON *hp = cJSON_CreateNumber(player->hp);
    cJSON *dead = cJSON_CreateBool(player->dead);
    cJSON *role = cJSON_CreateNumber(player->role->type);
    cJSON *character = cJSON_CreateNumber(itself ? player->character->type : -1);
    cJSON *weapon = cJSON_CreateBool(player->weapon != NULL);
    cJSON *barrel = cJSON_CreateBool(player->barrel != NULL);
    cJSON *mustang = cJSON_CreateBool(player->mustang != NULL);
    cJSON *scope = cJSON_CreateBool(player->scope != NULL);
    cJSON *jail = cJSON_CreateBool(player->jail != NULL);
    cJSON *dynamite = cJSON_CreateBool(player->dynamite != NULL);
    cJSON *hands_size = cJSON_CreateNumber(player->hands->size);
    cJSON *hands = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "name", name);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "hp", hp);
    cJSON_AddItemToObject(root, "dead", dead);
    cJSON_AddItemToObject(root, "role", role);
    cJSON_AddItemToObject(root, "character", character);
    cJSON_AddItemToObject(root, "weapon", weapon);
    cJSON_AddItemToObject(root, "barrel", barrel);
    cJSON_AddItemToObject(root, "mustang", mustang);
    cJSON_AddItemToObject(root, "scope", scope);
    cJSON_AddItemToObject(root, "jail", jail);
    cJSON_AddItemToObject(root, "dynamite", dynamite);
    cJSON_AddItemToObject(root, "hands_size", hands_size);
    cJSON_AddItemToObject(root, "hands", hands);

    if (itself) {
        for (i32 i = 0; i < player->hands->size; i++) {
            Card  *cur_card = player->hands->get(player->hands, i);
            cJSON *json_card = card_jsonlfy(&cur_card, itself);
            cJSON_AddItemToArray(hands, json_card);
        }
    }

    return root;
}

/*
game_json
{
    "player_size": number,
    "players":[
        player_json,
        player_json,
        .
        .
    ]
    "turn": number
    "finished": boolean
    "deck_size": number
    "discards_size": nubmer
    "discards: [
        card_json,
        card_json,
        .
        .
    ]
}
*/
cJSON *game_jsonify(Game *game, i32 player_id) {
    if (!game) return NULL;
    cJSON *root = cJSON_CreateObject();

    cJSON *player_size = cJSON_CreateNumber(game->players->size);
    cJSON *players = cJSON_CreateArray();
    cJSON *turn = cJSON_CreateNumber(game->turn);
    cJSON *finished = cJSON_CreateBool(game->finished);
    cJSON *deck_size = cJSON_CreateNumber(game->deck->size);
    cJSON *discard_size = cJSON_CreateNumber(game->discard->size);
    cJSON *discards = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "player_size", player_size);
    cJSON_AddItemToObject(root, "players", players);
    cJSON_AddItemToObject(root, "turn", turn);
    cJSON_AddItemToObject(root, "finished", finished);
    cJSON_AddItemToObject(root, "deck_size", deck_size);
    cJSON_AddItemToObject(root, "discard_size", discard_size);
    cJSON_AddItemToObject(root, "discards", discards);

    for (i32 i = 0; i < game->players->size; i++) {
        Player *cur_player = game->players->get(game->players, i);
        cJSON  *json_player = player_jsonify(cur_player, (player_id == i) || DEBUG_DISPLAY);
        cJSON_AddItemToArray(players, json_player);
    }

    for (i32 i = 0; i < game->discard->size; i++) {
        Card  *cur_discard = game->discard->get(game->discard, i);
        cJSON *json_discard = card_jsonlfy(&cur_discard, (player_id == i) || DEBUG_DISPLAY);
        cJSON_AddItemToArray(discards, json_discard);
    }

    return root;
}

#endif  // __JSONIFY_H
