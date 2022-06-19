#ifndef __ROOM_H
#define __ROOM_H

#include <libwebsockets.h>
#include <pthread.h>

#include "../third/cimple/all.h"
#include "../third/uds/deque.h"
#include "../third/uds/vector.h"

#ifdef THROW
#undef THROW
#endif
#define THROW(format, args...)                                                                     \
    ({                                                                                             \
        fprintf(stderr, "\x1b[91m[ROOM ERROR]\x1b[0m " format, ##args);                            \
        fprintf(stderr, "\x1b[91m[ROOM ERROR]\x1b[0m at %s:%s (Line %d)\n", __FILE__,              \
                __FUNCTION__, __LINE__);                                                           \
    })

#define STRING_MAX_LEN 256
#define ROOM_MAX_CAPACITY 16

typedef struct Room   Room;
typedef struct Client Client;

StructVector(Rooms, Room *, NULL);
StructVector(Clients, Client *, NULL);

void  room_global_init();
Room *room_create();
void  room_destroy(Room *room);
bool  room_join(Room *room, Client *actor);
bool  room_leave(Room *room, Client *actor);
bool  room_kick(Room *room, Client *actor, Client *target);
bool  room_set_name(Room *room, Client *actor, const char *name);
bool  room_set_key(Room *room, Client *actor, const char *key);
bool  room_is_host(Room *room, Client *actor);
Room *room_find_by_name(const char *name);

Client *client_create();
void    client_destroy(Client *client);
bool    client_set_name(Client *client, const char *name);
bool    client_set_instance(Client *client, const struct lws *instance);

size_t          global_room_counter = 0;
size_t          global_client_counter = 0;
Rooms          *global_rooms = NULL;
Clients        *global_clients = NULL;
pthread_mutex_t global_rooms_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t global_clients_mutex = PTHREAD_MUTEX_INITIALIZER;

////////////////////////////////////////////////////////////////////////////////

struct Room {
    size_t          id;
    char            name[STRING_MAX_LEN];
    char            key[STRING_MAX_LEN];
    Clients        *clients;
    pthread_mutex_t mutex;
};

struct Client {
    size_t          id;
    struct lws     *instance;
    char            name[STRING_MAX_LEN];
    Room           *room;
    pthread_mutex_t mutex;
};

void room_global_init() {
    global_rooms = create_Rooms();
    global_clients = create_Clients();
}

Room *room_create() {
    if (global_rooms == NULL) {
        THROW("global_rooms is NULL");
        return NULL;
    }

    Room *room = malloc(sizeof(Room));
    room->id = global_room_counter++;
    room->clients = create_Clients();
    memset(room->name, 0, STRING_MAX_LEN);
    memset(room->key, 0, STRING_MAX_LEN);
    pthread_mutex_init(&room->mutex, NULL);

    pthread_mutex_lock(&global_rooms_mutex);
    global_rooms->push(global_rooms, room);
    pthread_mutex_unlock(&global_rooms_mutex);

    return room;
}

void room_destroy(Room *room) {
    if (global_rooms == NULL) {
        THROW("global_rooms is NULL");
        return;
    }

    if (room == NULL) {
        THROW("room is NULL");
        return;
    }

    pthread_mutex_lock(&room->mutex);

    pthread_mutex_lock(&global_rooms_mutex);
    for (size_t i = 0; i < global_rooms->size; i++) {
        if (global_rooms->get(global_rooms, i) == room) {
            global_rooms->remove(global_rooms, i);
            break;
        }
    }
    pthread_mutex_unlock(&global_rooms_mutex);

    for (size_t i = 0; i < room->clients->size; i++) {
        Client *client = room->clients->get(room->clients, i);
        pthread_mutex_lock(&client->mutex);
        client->room = NULL;
        pthread_mutex_unlock(&client->mutex);
    }

    room->clients->free(room->clients);

    pthread_mutex_unlock(&room->mutex);
    pthread_mutex_destroy(&room->mutex);
    free(room);
}

bool room_join(Room *room, Client *actor) {
    bool success = false;

    u8 locks = 0;
    if (room == NULL) {
        THROW("room is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&room->mutex);
    locks++;

    if (actor == NULL) {
        THROW("actor is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&actor->mutex);
    locks++;

    if (actor->room != NULL) {
        THROW("actor is already in a room");
        goto unlock;
    }

    if (room->clients->size == ROOM_MAX_CAPACITY) {
        THROW("room is full");
        goto unlock;
    }

    room->clients->push(room->clients, actor);
    actor->room = room;
    success = true;

    goto unlock;

unlock:
    if (locks >= 2) {
        pthread_mutex_unlock(&actor->mutex);
    }
    if (locks >= 1) {
        pthread_mutex_unlock(&room->mutex);
    }
    return success;
}

bool room_leave(Room *room, Client *actor) {
    bool success = false;

    u8 locks = 0;
    if (room == NULL) {
        THROW("room is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&room->mutex);
    locks++;

    if (actor == NULL) {
        THROW("actor is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&actor->mutex);
    locks++;

    if (actor->room != room) {
        THROW("actor is not in this room");
        goto unlock;
    }

    for (size_t i = 0; i < room->clients->size; i++) {
        if (room->clients->get(room->clients, i) == actor) {
            room->clients->remove(room->clients, i);
            break;
        }
    }
    actor->room = NULL;
    success = true;

    goto unlock;

unlock:
    if (locks >= 2) {
        pthread_mutex_unlock(&actor->mutex);
    }
    if (locks >= 1) {
        pthread_mutex_unlock(&room->mutex);
    }
    return success;
}

bool room_kick(Room *room, Client *actor, Client *target) {
    bool success = false;

    u8 locks = 0;
    if (room == NULL) {
        THROW("room is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&room->mutex);
    locks++;

    if (actor == NULL) {
        THROW("actor is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&actor->mutex);
    locks++;

    if (target == NULL) {
        THROW("target is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&target->mutex);
    locks++;

    if (actor->room != room) {
        THROW("actor is not in this room");
        goto unlock;
    }

    if (target->room != room) {
        THROW("target is not in this room");
        goto unlock;
    }

    if (!room_is_host(room, actor)) {
        THROW("actor is not the host");
        goto unlock;
    }

    for (size_t i = 0; i < room->clients->size; i++) {
        if (room->clients->get(room->clients, i) == target) {
            room->clients->remove(room->clients, i);
            break;
        }
    }
    target->room = NULL;
    success = true;

    goto unlock;

unlock:
    if (locks >= 3) {
        pthread_mutex_unlock(&target->mutex);
    }
    if (locks >= 2) {
        pthread_mutex_unlock(&actor->mutex);
    }
    if (locks >= 1) {
        pthread_mutex_unlock(&room->mutex);
    }
    return success;
}

bool room_set_name(Room *room, Client *actor, const char *name) {
    bool success = false;

    u8 locks = 0;
    if (room == NULL) {
        THROW("room is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&room->mutex);
    locks++;

    if (actor == NULL) {
        THROW("actor is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&actor->mutex);
    locks++;

    if (actor->room != room) {
        THROW("actor is not in this room");
        goto unlock;
    }

    if (!room_is_host(room, actor)) {
        THROW("actor is not the host");
        goto unlock;
    }

    strncpy(room->name, name, STRING_MAX_LEN - 1);
    success = true;

    goto unlock;

unlock:
    if (locks >= 2) {
        pthread_mutex_unlock(&actor->mutex);
    }
    if (locks >= 1) {
        pthread_mutex_unlock(&room->mutex);
    }
    return success;
}

bool room_set_key(Room *room, Client *actor, const char *key) {
    bool success = false;

    u8 locks = 0;
    if (room == NULL) {
        THROW("room is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&room->mutex);
    locks++;

    if (actor == NULL) {
        THROW("actor is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&actor->mutex);
    locks++;

    if (actor->room != room) {
        THROW("actor is not in this room");
        goto unlock;
    }

    if (!room_is_host(room, actor)) {
        THROW("actor is not the host");
        goto unlock;
    }

    strncpy(room->key, key, STRING_MAX_LEN - 1);
    success = true;

    goto unlock;

unlock:
    if (locks >= 2) {
        pthread_mutex_unlock(&actor->mutex);
    }
    if (locks >= 1) {
        pthread_mutex_unlock(&room->mutex);
    }
    return success;
}

bool room_is_host(Room *room, Client *actor) {
    bool is_host = false;

    u8 locks = 0;
    if (room == NULL) {
        THROW("room is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&room->mutex);
    locks++;

    if (actor == NULL) {
        THROW("actor is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&actor->mutex);
    locks++;

    if (actor->room != room) {
        THROW("actor is not in this room");
        goto unlock;
    }

    is_host = room->clients->get(room->clients, 0) == actor;

    goto unlock;

unlock:
    if (locks >= 2) {
        pthread_mutex_unlock(&actor->mutex);
    }
    if (locks >= 1) {
        pthread_mutex_unlock(&room->mutex);
    }
    return is_host;
}

Room *room_find_by_name(const char *name) {
    Room *room = NULL;

    if (name == NULL) {
        THROW("name is NULL");
        return room;
    }

    for (size_t i = 0; i < global_rooms->size; i++) {
        Room *curr = global_rooms->get(global_rooms, i);
        pthread_mutex_lock(&curr->mutex);
        if (strcmp(curr->name, name) == 0) {
            room = curr;
        }
        pthread_mutex_unlock(&curr->mutex);
    }

    return room;
}

Client *client_create() {
    if (global_clients == NULL) {
        THROW("global_clients is NULL");
        return NULL;
    }

    Client *client = malloc(sizeof(Client));
    client->id = global_client_counter++;
    client->instance = NULL;
    memset(client->name, 0, STRING_MAX_LEN);
    client->room = NULL;
    pthread_mutex_init(&client->mutex, NULL);

    pthread_mutex_lock(&global_clients_mutex);
    global_clients->push(global_clients, client);
    pthread_mutex_unlock(&global_clients_mutex);

    return client;
}

void client_destroy(Client *client) {
    if (global_clients == NULL) {
        THROW("global_clients is NULL");
        return;
    }

    if (client == NULL) {
        THROW("client is NULL");
        return;
    }

    pthread_mutex_lock(&client->mutex);

    pthread_mutex_lock(&global_clients_mutex);
    for (size_t i = 0; i < global_clients->size; i++) {
        if (global_clients->get(global_clients, i) == client) {
            global_clients->remove(global_clients, i);
            break;
        }
    }
    pthread_mutex_unlock(&global_clients_mutex);

    pthread_mutex_unlock(&client->mutex);
    pthread_mutex_destroy(&client->mutex);
    free(client);
}

bool client_set_name(Client *client, const char *name) {
    bool success = false;

    u8 locks = 0;
    if (client == NULL) {
        THROW("client is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&client->mutex);
    locks++;

    strncpy(client->name, name, STRING_MAX_LEN - 1);
    success = true;

    goto unlock;

unlock:
    if (locks >= 1) {
        pthread_mutex_unlock(&client->mutex);
    }
    return success;
}

bool client_set_instance(Client *client, const struct lws *instance) {
    bool success = false;

    u8 locks = 0;
    if (client == NULL) {
        THROW("client is NULL");
        goto unlock;
    }
    pthread_mutex_lock(&client->mutex);
    locks++;

    if (client->instance != NULL) {
        THROW("client already has an instance");
        goto unlock;
    }

    client->instance = (struct lws *)instance;
    success = true;

    goto unlock;

unlock:
    if (locks >= 1) {
        pthread_mutex_unlock(&client->mutex);
    }
    return success;
}

#undef THROW

#endif  // __ROOM_H
