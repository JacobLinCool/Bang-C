
#include "server.h"

#include "../core/game.h"

bool  game_started = false;
bool  stop_server = false;
Game *game;
sem_t gm_created;

pthread_t gm, sv;

void *gm_thread_start() {
    sem_post(&gm_created);
    game_loop(game);
}

void start_server(int port, lws_callback_function callback) {
    struct lws_protocols protocols[] = {{.id = 2048,
                                         .name = "cool-protocol",
                                         .callback = callback,
                                         .per_session_data_size = sizeof(Client *),
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

    context = lws_create_context(&info);

    if (context == NULL) {
        Console.error("WebSocket context init failed");
        return;
    }

    Console.info("WebSocket server started");

    while (lws_service(context, 0) >= 0) {
        if (stop_server) {
            break;
        }
    }

    lws_context_destroy(context);

    Console.info("WebSocket server stopped");
}

void handle_action(Client *sender, char *action, cJSON *payload) {
    if (strcmp("join", action) == 0) {
        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s join the game.", sender->name)));

            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

    } else if (strcmp("name", action) == 0) {
        if (sender->named == true) {
            respond_error(sender, $(String.format("Already have a name: %s", sender->name)));
            goto clean;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            goto clean;
        }

        cJSON *name_token = cJSON_GetObjectItem(payload, "name");

        if (name_token == NULL || cJSON_IsString(name_token) == false) {
            respond_error(sender, "Missing name");
            goto clean;
        }

        if (strncmp("Computer", name_token->valuestring, 8) == 0) {
            respond_error(sender, "You can't use that name");
            goto clean;
        }

        for (int i = 0; i < clients->size; i++) {
            Client *other = clients->data[i];
            if (strcmp(other->name, name_token->valuestring) == 0) {
                respond_error(sender, "Name already taken");
                goto clean;
            }
        }

        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(
                clients->get(clients, i),
                $(String.format("%s is now known as %s.", sender->name, name_token->valuestring)));
        }

        strncpy(sender->name, name_token->valuestring, 255);
        sender->named = true;

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

    } else if (strcmp("kick", action) == 0) {
        if (is_host(sender) == false) {
            respond_error(sender, "Only the host can kick players");
            goto clean;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            goto clean;
        }

        cJSON *name_token = cJSON_GetObjectItem(payload, "name");

        if (name_token == NULL || cJSON_IsString(name_token) == false) {
            respond_error(sender, "Missing name");
            goto clean;
        }

        bool    is_computer = false;
        Client *target = NULL;

        char *name = name_token->valuestring;

        if (computer_count > 0 && strncmp("Computer", name, 8) == 0) {
            computer_count--;
            is_computer = true;
        }

        for (size_t i = 0; i < clients->size; i++) {
            Client *c = clients->get(clients, i);
            if (strcmp(c->name, name) == 0) {
                target = c;
                clients->remove(clients, i);
                break;
            }
        }

        if (target == NULL && is_computer == false) {
            respond_error(sender, $(String.format("No player named %s", name)));
            goto clean;
        }

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has been kicked by %s",
                                         is_computer ? name : target->name, sender->name)));

            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

        if (target != NULL) {
            lws_close_reason(target->instance, LWS_CLOSE_STATUS_NORMAL, "Kicked", strlen("Kicked"));
            lws_close_free_wsi(target->instance, LWS_CLOSE_STATUS_NORMAL, "Kicked");
        }

    } else if (strcmp("chat", action) == 0) {
        cJSON *message_token = cJSON_GetObjectItem(payload, "message");

        if (message_token == NULL || cJSON_IsString(message_token) == false) {
            respond_error(sender, "Missing message");
            goto clean;
        }

        char *message = message_token->valuestring;

        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s: %s", sender->name, message)));
        }

    } else if (strcmp("add_computer", action) == 0) {
        if (is_host(sender) == false) {
            respond_chat(sender, "Only the host can add computer player");
            goto clean;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            goto clean;
        }

        if (clients->size + computer_count >= 7) {
            respond_chat(sender, "Room is full");
            goto clean;
        }

        computer_count++;

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has added a computer player", sender->name)));

            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

    } else if (strcmp("start", action) == 0) {
        if (is_host(sender) == false) {
            respond_error(sender, "Only the host can start the game");
            goto clean;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            goto clean;
        }

        size_t players = computer_count + clients->size;

        if (players < 4) {
            respond_error(sender, "Not enough players");
            goto clean;
        }

        game = new_game();
        game_started = true;

        for (size_t i = 0; i < clients->size; i++) {
            if (clients->get(clients, i)->named == false) {
                clients->get(clients, i)->named = true;
            }
        }

        for (size_t i = 0; i < clients->size; i++) {
            game->join(game, clients->get(clients, i)->name, false);
        }

        for (size_t i = 0; i < computer_count; i++) {
            game->join(game, $(String.format("Computer %zu", i + 1)), true);
        }

        game->players->shuffle(game->players);

        game->start(game);

        for (size_t i = 0; i < game->players->size; i++) {
            Player *player = game->players->get(game->players, i);
            for (size_t j = 0; j < clients->size; j++) {
                Client *client = clients->get(clients, j);
                if (strcmp(player->name, client->name) == 0) {
                    client->player_id = player->id;
                }
            }
        }

        Console.log("client size: %d", clients->size);
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has started the game", sender->name)));

            respond(clients->get(clients, i), "start",
                    game_jsonify(game, clients->get(clients, i)->player_id), false);
        }

        pthread_create(&gm, NULL, gm_thread_start, NULL);

    }
    // {"player" : int}
    else if (strcmp("select_player", action) == 0) {
        if (!game_started) {
            respond_error(sender, "Game has not started");
            goto clean;
        }

        cJSON *player = cJSON_GetObjectItem(payload, "player");
        if (player == NULL) {
            respond_error(sender, "Not choose player");
            goto clean;
        }
        if (cJSON_IsNumber(player) == false) {
            respond_error(sender, "Player id should be a number");
            goto clean;
        }
        int number = (int)cJSON_GetNumberValue(player);
        share_num = number;
        sem_post(&waiting_for_input);
    }
    // {"card" : int}
    else if (strcmp("select_card", action) == 0) {
        Console.log("server receive: select_card");
        if (!game_started) {
            respond_error(sender, "Game has not started");
            goto clean;
        }

        cJSON *card = cJSON_GetObjectItem(payload, "card");
        if (card == NULL) {
            respond_error(sender, "No choose card");
            goto clean;
        }
        if (cJSON_IsNumber(card) == false) {
            respond_error(sender, "Card offset should be a number");
            goto clean;
        }
        Console.green("-----------ok---------");
        int number = (int)cJSON_GetNumberValue(card);
        if (number >= 0) {
            share_offset = number ^ key;
        } else {
            share_offset = number;
        }
        Console.green("-----------ok1---------");
        sem_post(&waiting_for_input);
    }
    // {"y/n" : int}
    else if (strcmp("yes_no", action) == 0) {
        if (!game_started) {
            respond_error(sender, "Game has not started");
            goto clean;
        }

        cJSON *yn = cJSON_GetObjectItem(payload, "y/n");
        if (yn == NULL) {
            respond_error(sender, "Not make dicision");
            goto clean;
        }
        if (cJSON_IsNumber(yn) == false) {
            respond_error(sender, "dicision should be a number");
            goto clean;
        }

        int number = (int)cJSON_GetNumberValue(yn);
        share_num = number;
        sem_post(&waiting_for_input);
    }

    goto clean;

clean:
    cJSON_Delete(payload);
}

static int event_handler(struct lws *instance, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    Client *client = find_client(instance);

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            if (clients->size >= 7 - computer_count) {
                Console.red("[%p] Room is full, Connection refused", instance);
                lws_close_free_wsi(instance, LWS_CLOSE_STATUS_NORMAL, "Room is full");
                break;
            }

            Console.yellow("[%p] Connection established", instance);
            client = $(calloc(1, sizeof(Client)));
            client->instance = instance;
            client->msg_queue = create_Messages();
            client->named = false;
            strcpy(client->name, $(String.format("Player %s", $(random_string(8)))));
            client->player_id = -1;
            clients->push(clients, client);

            handle_action(client, "join", NULL);

            break;
        }

        case LWS_CALLBACK_RECEIVE: {
            if (client == NULL) {
                Console.red("[%p] Client is NULL", instance);
                break;
            }

            char  *data = $(strndup((char *)in, len));
            cJSON *json = cJSON_Parse(data);
            if (json == NULL) {
                Console.red("[%p] Invalid JSON: %s", instance, data);
                break;
            }

            cJSON *action_token = cJSON_GetObjectItem(json, "action");
            char  *action = NULL;

            if (cJSON_IsString(action_token) == false || (action_token->valuestring == NULL)) {
                // cJSON_Delete(json);
                break;
            }

            action = action_token->valuestring;

            cJSON *payload_token = cJSON_GetObjectItem(json, "payload");
            cJSON *payload = NULL;

            if (cJSON_IsObject(payload_token) == false) {
                // cJSON_Delete(json);
                break;
            }

            payload = payload_token;

            Console.cyan("[%p] Received action: %s", instance, action);

            cJSON_DetachItemFromObject(json, "payload");
            cJSON_Delete(json);

            handle_action(client, action, payload);

            break;
        }

        case LWS_CALLBACK_SERVER_WRITEABLE: {
            if (client == NULL) {
                Console.red("[%p] Client is NULL", instance);
                break;
            }

            if (client->msg_queue->size == 0) {
                Console.blue("[%p] No messages to send", instance);
                break;
            }

            char *msg = client->msg_queue->shift(client->msg_queue);

            if (msg == NULL) {
                Console.red("[%p] Message is NULL", instance);
                break;
            }

            // Console.blue("[%p] Sending %s", instance, msg);
            size_t         len = strlen(msg);
            unsigned char *payload = $(calloc(len + 1 + LWS_PRE + 4, sizeof(char)));
            memcpy(payload + LWS_PRE, msg, len + 1);
            lws_write(instance, payload + LWS_PRE, len, LWS_WRITE_TEXT);
            Console.green("[%p] Message sent (%zu), queueing: %zu", instance, len,
                          client->msg_queue->size);

            if (client->msg_queue->size > 0) {
                lws_callback_on_writable(instance);
            }

            break;
        }

        case LWS_CALLBACK_CLOSED: {
            if (client == NULL) {
                Console.red("[%p] Client is NULL", instance);
                break;
            }

            Console.yellow("[%p] Connection closed", instance);
            for (size_t i = 0; i < clients->size; i++) {
                if (clients->get(clients, i)->instance == instance) {
                    clients->remove(clients, i);
                }
            }

            while (client->msg_queue->size > 0) {
                char *msg = client->msg_queue->shift(client->msg_queue);
            }

            cJSON *list = create_player_list();
            for (size_t i = 0; i < clients->size; i++) {
                respond_chat(clients->get(clients, i),
                             $(String.format("%s leave the game.", client->name)));

                respond(clients->get(clients, i), "players", list, true);
            }
            cJSON_Delete(list);

            if (game != NULL && client->player_id != -1) {
                Console.info("[%p] %s leave the game, computer is now taking over", instance,
                             client->name);
                Player *player = game->players->get(game->players, client->player_id);
                player->play = computer_player;
                player->choose_enemy = computer_choose_enemy;
                player->select = computer_player_select;
                player->request = computer_player_request;
                player->take = computer_player_take;
                player->ramirez = computer_player_ramirez;

                if (player->id == waiting_for_player) {
                    waiting_for_player = -1;
                    share_num = -2;
                    share_offset = -2;
                    sem_post(&waiting_for_input);
                }
            }

            client->msg_queue->free(client->msg_queue);

            break;
        }

        case LWS_CALLBACK_TIMER: {
            if (client && game && waiting_for_player != -1 &&
                waiting_for_player == client->player_id) {
                lws_close_reason(instance, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"Timeout",
                                 strlen("Timeout"));
                lws_close_free_wsi(instance, LWS_CLOSE_STATUS_NORMAL, "Timeout");

                Console.yellow("[%p] Connection timed out", instance);
            }

            break;
        }

        default:
            break;
    }

    return 0;
}

void *server_thread_start() { start_server(8080, event_handler); }

int main(void) {
    time_t seed = time(NULL);  //= 1655497098;  // time(NULL);1655587684
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

        for (size_t i = 0; i < game->players->size; i++) {
            Player *player = game->players->get(game->players, i);
            player->hands->free(player->hands);
        }
        game->players->free(game->players);
        game->characters->free(game->characters);
        game->roles->free(game->roles);
        game->deck->free(game->deck);
        game->discard->free(game->discard);
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
