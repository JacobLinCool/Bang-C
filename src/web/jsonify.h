#ifndef __JSONIFY_H
#define __JSONIFY_H

#include "../core/game.h"
#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"

cJSON* player_jsonify(Player* player) {
    cJSON* root = cJSON_CreateObject();

    // ...

    return root;
}

cJSON* game_jsonify(Game* game) {
    cJSON* root = cJSON_CreateObject();

    // ...

    return root;
}

cJSON* filter_player_perspective(cJSON* raw) {
    cJSON* root = cJSON_CreateObject();

    // filter out others' information

    return root;
}

#endif  // __JSONIFY_H
