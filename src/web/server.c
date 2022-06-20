
#include "server.h"

#include "../core/game.h"
#include "http.h"
#include "protocol_bang.h"

void       start_server(int port);
void       bang_action(Client *sender, char *action, cJSON *payload);
static int protocol_bang(struct lws *instance, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len);

void start_server(int port) {
    struct lws_protocols protocols[] = {{.name = "http",
                                         .callback = lws_callback_http_dummy,
                                         .per_session_data_size = 0,
                                         .rx_buffer_size = 0},
                                        {.id = 2048,
                                         .name = "bang-gaming",
                                         .callback = protocol_bang,
                                         .per_session_data_size = 0,
                                         .rx_buffer_size = BUFFER_SIZE,
                                         .user = NULL,
                                         .tx_packet_size = 0},
                                        LWS_PROTOCOL_LIST_TERM};

    struct lws_context_creation_info info;
    struct lws_context              *context;

    memset(&info, 0, sizeof(info));
    info.port = port;
    info.protocols = protocols;
    info.vhost_name = "localhost";
    info.mounts = &mount;
    info.error_document_404 = "/index.html";

    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN, NULL);

    context = lws_create_context(&info);

    if (context == NULL) {
        Console.error("Server context init failed");
        return;
    }

    Console.info("Server started");

    while (lws_service(context, 0) >= 0) {
        if (stop_server) break;
    }

    lws_context_destroy(context);

    Console.info("Server stopped");
}

void *server_thread_start() { start_server(8080); }

int main(void) {
    time_t seed = time(NULL);
    Console.info("Random Seed: %ld", seed);
    srand(seed);

    clients = create_Clients();
    pthread_create(&sv, NULL, server_thread_start, NULL);

    while (true) {
        Console.info("You can type \"exit\" to quit after a round of game finished.");
        sem_init(&gm_created, 0, 0);
        sem_init(&waiting_for_input, 0, 0);

        key = rand() % 2048;
        Console.log("encrypt key is %u", key);
        sem_wait(&gm_created);
        pthread_join(gm, NULL);
        Console.log("Game Over");

        game_destroy(game);
        game = NULL;
        game_started = false;

        while (clients->size) {
            Client *client = clients->pop(clients);
            lws_close_reason(client->instance, LWS_CLOSE_STATUS_NORMAL,
                             (unsigned char *)"Game Over", strlen("Game Over"));
            lws_close_free_wsi(client->instance, LWS_CLOSE_STATUS_NORMAL, "Game Over");
            client->msg_queue->free(client->msg_queue);
        }

        Console.info("[$] Freeing %d marked blocks ...", $size());
        $free();
        Console.info("[$] Done.");

        sleep(3);

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval timeout = {0, 0};

        int sel_rv = select(1, &readfds, NULL, NULL, &timeout);
        if (sel_rv > 0) {
            char buffer[1024];
            scanf("%s", buffer);
            if (strcmp(buffer, "exit") == 0) {
                stop_server = true;
                pthread_join(sv, NULL);
                break;
            }
        }
        Console.info("Starting a new round of game.");
    }

    return EXIT_SUCCESS;
}
