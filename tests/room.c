#include "../src/web/room.h"

#include <pthread.h>
#define THREAD_COUNT 16
#define EACH_ROOMS 16
#define EACH_CLIENTS 32

#include "../src/web/random_string.h"

void* server() {
    for (int r = 0; r < EACH_ROOMS; r++) {
        Room* room = room_create();
        for (int c = 0; c < EACH_CLIENTS; c++) {
            Client* client = client_create();
            client_set_name(client, $(String.format("%d-%d-%d", getpid(), r, c)));

            room_join(room, client);
            if (c == 0) {
                room_set_name(room, client, $(String.format("%d-%d", getpid(), r)));
            }
        }
    }

    Console.cyan("%d", getpid());
    pthread_exit(NULL);
}

int main() {
    srand(20220619);
    room_global_init();

    pthread_t servers[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&servers[i], NULL, server, NULL);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(servers[i], NULL);
    }
    // $free();

    return EXIT_SUCCESS;
}

// Client* host = client_create();
// Client* guest = client_create();

// for (int i = 0; i < 100; i++) {
//     char* name = random_string(512);
//     char* key = random_string(512);

//     Room* room = room_create();

//     assert(room_join(room, host) == true);
//     assert(room_set_name(room, host, name) == true);
//     assert(room_set_key(room, host, key) == true);

//     assert(room_join(room, guest) == true);
//     assert(room_set_name(room, guest, name) == false);
//     assert(room_set_key(room, guest, key) == false);

//     free(name);
//     free(key);

//     assert(strlen(room->name) == STRING_MAX_LEN - 1);
//     assert(strlen(room->key) == STRING_MAX_LEN - 1);

//     room_destroy(room);
// }