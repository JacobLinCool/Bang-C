#ifndef __JSONIFY_H
#define __JSONIFY_H

#include "../core/game.h"
#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"

cJSON *card_jsonlfy(Card **card) {
    cJSON *root = cJSON_CreateObject();

    u64    address = (u64)(*card);
    cJSON *pointer = cJSON_CreateNumber(address);
    cJSON *type = cJSON_CreateNumber((*card)->type);
    cJSON *priority = cJSON_CreateNumber((*card)->priority);

    if (!pointer) goto end;
    if (!type) goto end;
    if (!priority) goto end;

    cJSON_AddItemToObject(root, "address", pointer);
    cJSON_AddItemToObject(root, "type", type);
    cJSON_AddItemToObject(root, "priority", priority);

    return root;
end:
    cJSON_Delete(root);
    return NULL;
}

cJSON *player_jsonify(Player *player) {
    cJSON *root = cJSON_CreateObject();

    cJSON *name = cJSON_CreateString(player->name);
    cJSON *id = cJSON_CreateNumber(player->id);
    cJSON *hp = cJSON_CreateNumber(player->hp);
    cJSON *dead = cJSON_CreateBool(player->dead);
    cJSON *role = cJSON_CreateNumber(player->role->type);
    cJSON *character = cJSON_CreateNumber(player->character->type);
    cJSON *weapon = cJSON_CreateBool(player->weapon != NULL);
    cJSON *barrel = cJSON_CreateBool(player->barrel != NULL);
    cJSON *mustang = cJSON_CreateBool(player->mustang != NULL);
    cJSON *scope = cJSON_CreateBool(player->scope != NULL);
    cJSON *jail = cJSON_CreateBool(player->jail != NULL);
    cJSON *dynamite = cJSON_CreateBool(player->dynamite != NULL);
    cJSON *hands = cJSON_CreateArray();

    if (!name) goto end;
    if (!id) goto end;
    if (!hp) goto end;
    if (!dead) goto end;
    if (!role) goto end;
    if (!character) goto end;
    if (!weapon) goto end;
    if (!barrel) goto end;
    if (!mustang) goto end;
    if (!scope) goto end;
    if (!jail) goto end;
    if (!dynamite) goto end;
    if (!hands) goto end;

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
    cJSON_AddItemToObject(root, "hand", hands);

    for (i32 i = 0; i < player->hands->size; i++) {
        cJSON *card = card_jsonlfy(&(player->hands[i]));
    }

    return root;

end:
    cJSON_Delete(root);
    return NULL;
}

cJSON *game_jsonify(Game *game) {
    cJSON *root = cJSON_CreateObject();

    // ...

    return root;
}

cJSON *filter_player_perspective(cJSON *raw) {
    cJSON *root = cJSON_CreateObject();

    // filter out others' information

    return root;
}

#endif  // __JSONIFY_H
