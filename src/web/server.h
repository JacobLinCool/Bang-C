#ifndef __SERVER_H
#define __SERVER_H

#include <libwebsockets.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"
#include "../third/uds/deque.h"
#include "../third/uds/vector.h"
#include "../utils/all.h"
#include "random_string.h"
#define BUFFER_SIZE (64 * 1024)
#ifdef DEBUG
#define TIME_OUT_SECONDS 600
#else
#define TIME_OUT_SECONDS 60
#endif
// #define cJSON_Delete(json) ({})

int computer_count = 0;

sem_t waiting_for_input;
int   share_num;     // pthread
i32   share_offset;  // pthread

StructDeque(Messages, char *, NULL);

typedef struct Client {
    struct lws *instance;
    bool        named;
    char        name[256];
    Messages   *msg_queue;

    /** bellows will be allocated after game started */
    int player_id;
} Client;
StructVector(Clients, Client *, NULL);

Clients *clients = NULL;

Client *find_client(struct lws *instance) {
    for (int i = 0; i < clients->size; i++) {
        Client *client = clients->data[i];
        if (client->instance == instance) {
            return client;
        }
    }
    return NULL;
}

bool is_host(Client *client) { return clients->size > 0 && client == clients->get(clients, 0); }

void respond(Client *client, const char *type, cJSON *payload, bool detach) {
    if (client == NULL) return;
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "type", cJSON_CreateString(type));
    cJSON_AddItemToObject(res, "payload", payload);

    char *msg = $(cJSON_PrintUnformatted(res));

    if (detach) {
        cJSON_DetachItemFromObject(res, "payload");
    }
    cJSON_Delete(res);

    client->msg_queue->push(client->msg_queue, msg);
    lws_callback_on_writable(client->instance);
}

void respond_error(Client *client, const char *message) {
    if (client == NULL) return;
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddItemToObject(payload, "message", cJSON_CreateString(message));
    respond(client, "error", payload, false);

    Console.red("[%p] Error: %s", client->instance, message);
}

void respond_chat(Client *client, const char *message) {
    if (client == NULL) return;
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddItemToObject(payload, "message", cJSON_CreateString(message));
    respond(client, "chat", payload, false);
}

void respond_all_chat(const char *message) {
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddItemToObject(payload, "message", cJSON_CreateString(message));
    for (int i = 0; i < clients->size; i++) {
        respond(clients->get(clients, i), "chat", payload, true);
    }
    cJSON_Delete(payload);
}

cJSON *create_player_list() {
    cJSON *players = cJSON_CreateArray();
    for (size_t i = 0; i < clients->size; i++) {
        Client *client = clients->get(clients, i);
        cJSON_AddItemToArray(players, cJSON_CreateString(client->name));
    }
    for (size_t i = 0; i < computer_count; i++) {
        cJSON_AddItemToArray(players, cJSON_CreateString($(String.format("Computer %d", i + 1))));
    }

    cJSON *obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "players", players);
    return obj;
}

#endif  // __SERVER_H
