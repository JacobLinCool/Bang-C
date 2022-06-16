#include "server.h"

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
        // nothing!
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

            respond(clients->get(clients, i), "players", list);
        }
        cJSON_Delete(list);
    }
    if (strcmp("name", action) == 0) {
        if (sender->named == true) {
            respond_error(sender, $(String.format("Already have a name: %s", sender->name)));
            return;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            return;
        }

        cJSON *name_token = cJSON_GetObjectItem(payload, "name");

        if (name_token == NULL || cJSON_IsString(name_token) == false) {
            respond_error(sender, "Missing name");
            return;
        }

        for (int i = 0; i < clients->size; i++) {
            Client *other = clients->data[i];
            if (strcmp(other->name, name_token->valuestring) == 0) {
                respond_error(sender, "Name already taken");
                return;
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
            respond(clients->get(clients, i), "players", list);
        }
        cJSON_Delete(list);

        cJSON_Delete(payload);
        return;
    }

    if (strcmp("kick", action) == 0) {
        if (is_host(sender) == false) {
            respond_error(sender, "Only the host can kick players");
            return;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            return;
        }

        cJSON *name_token = cJSON_GetObjectItem(payload, "name");

        if (name_token == NULL || cJSON_IsString(name_token) == false) {
            respond_error(sender, "Missing name");
            return;
        }

        char *name = name_token->valuestring;

        Client *target = NULL;

        for (size_t i = 0; i < clients->size; i++) {
            Client *c = clients->get(clients, i);
            if (strcmp(c->name, name) == 0) {
                target = c;
                clients->remove(clients, i);
                break;
            }
        }

        if (target == NULL) {
            respond_error(sender, $(String.format("No player named %s", name)));
            return;
        }

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has been kicked by %s", target->name, sender->name)));

            respond(clients->get(clients, i), "players", list);
        }
        cJSON_Delete(list);

        lws_close_reason(target->instance, LWS_CLOSE_STATUS_NORMAL, "Kicked", strlen("Kicked"));
        lws_close_free_wsi(target->instance, LWS_CLOSE_STATUS_NORMAL, "Kicked");

        cJSON_Delete(payload);
        return;
    }

    if (strcmp("chat", action) == 0) {
        cJSON *message_token = cJSON_GetObjectItem(payload, "message");

        if (message_token == NULL || cJSON_IsString(message_token) == false) {
            respond_error(sender, "Missing message");
            return;
        }

        char *message = message_token->valuestring;

        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s: %s", sender->name, message)));
        }

        cJSON_Delete(payload);
        return;
    }

    if (strcmp("add_computer", action) == 0) {
        if (is_host(sender) == false) {
            respond_chat(sender, "Only the host can add computer player");
            return;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            return;
        }

        if (clients->size + computer_count >= 7) {
            respond_chat(sender, "Room is full");
            return;
        }

        computer_count++;

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has added a computer player", sender->name)));

            respond(clients->get(clients, i), "players", list);
        }
        cJSON_Delete(list);

        cJSON_Delete(payload);
        return;
    }

    if (strcmp("start", action) == 0) {
        if (is_host(sender) == false) {
            respond_error(sender, "Only the host can start the game");
            return;
        }

        if (game_started) {
            respond_chat(sender, "Game already started");
            return;
        }

        size_t players = computer_count + clients->size;

        if (players < 4) {
            respond_error(sender, "Not enough players");
            return;
        }

        game_started = true;
        game = new_game();

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

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has started the game", sender->name)));

            respond(clients->get(clients, i), "start", list);
        }
        cJSON_Delete(list);

        cJSON_Delete(payload);
        return;
    }
}

static int event_handler(struct lws *instance, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    Client *client = find_client(instance);

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            lws_set_timer_usecs(instance, TIME_OUT_SECONDS * LWS_USEC_PER_SEC);
            if (clients->size >= 7 - computer_count) {
                Console.red("[%p] Room is full, Connection refused", instance);
                lws_close_free_wsi(instance, LWS_CLOSE_STATUS_NORMAL, "Room is full");
                break;
            }

            Console.yellow("[%p] Connection established", instance);
            client = calloc(1, sizeof(Client));
            client->instance = instance;
            client->msg_queue = create_Messages();
            client->named = false;
            strcpy(client->name, $(String.format("Player %s", random_string(8))));
            clients->push(clients, client);

            handle_action(client, "join", NULL);

            break;
        }

        case LWS_CALLBACK_RECEIVE: {
            lws_set_timer_usecs(instance, TIME_OUT_SECONDS * LWS_USEC_PER_SEC);

            if (client == NULL) {
                Console.red("[%p] Client is NULL", instance);
                break;
            }

            char  *data = strndup((char *)in, len);
            cJSON *json = cJSON_Parse(data);
            if (json == NULL) {
                Console.red("[%p] Invalid JSON: %s", instance, data);
                break;
            }

            cJSON *action_token = cJSON_GetObjectItem(json, "action");
            char  *action = NULL;

            if (cJSON_IsString(action_token) == false || (action_token->valuestring == NULL)) {
                cJSON_Delete(json);
                break;
            }

            action = action_token->valuestring;

            cJSON *payload_token = cJSON_GetObjectItem(json, "payload");
            cJSON *payload = NULL;

            if (cJSON_IsObject(payload_token) == false) {
                cJSON_Delete(json);
                break;
            }

            payload = payload_token;

            Console.cyan("[%p] Received action: %s", instance, action);
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

            Message *msg = client->msg_queue->shift(client->msg_queue);

            if (msg == NULL) {
                Console.red("[%p] Message is NULL", instance);
                break;
            }

            Console.blue("[%p] Sending %s", instance, msg->payload);
            lws_write(instance, (unsigned char *)msg->payload, msg->len, LWS_WRITE_TEXT);

            // free_message(msg);
            free(msg);

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
                Message *msg = client->msg_queue->shift(client->msg_queue);
                free_message(msg);
            }

            cJSON *list = create_player_list();
            for (size_t i = 0; i < clients->size; i++) {
                respond_chat(clients->get(clients, i),
                             $(String.format("%s leave the game.", client->name)));

                respond(clients->get(clients, i), "players", list);
            }
            cJSON_Delete(list);

            client->msg_queue->free(client->msg_queue);
            free(client);

            break;
        }

        case LWS_CALLBACK_TIMER: {
            lws_close_reason(instance, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"Timeout",
                             strlen("Timeout"));
            lws_close_free_wsi(instance, LWS_CLOSE_STATUS_NORMAL, "Timeout");

            Console.yellow("[%p] Connection timed out", instance);

            break;
        }

        default:
            break;
    }

    return 0;
}

int main(void) {
    srand(time(NULL));
    clients = create_Clients();

    start_server(8080, event_handler);

    return EXIT_SUCCESS;
}
