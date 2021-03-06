#include <libwebsockets.h>

#include "server.h"

void *gm_thread_start() {
    sem_post(&gm_created);
    game_loop(game);
}

void bang_action(Client *sender, char *action, cJSON *payload) {
    if (strcmp("join", action) == 0) {
        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            if (computer_count + clients->size <= 7 && game_started == false) {
                respond_chat(clients->get(clients, i),
                             $(String.format("%s join the game as a player.", sender->name)));
            } else {
                respond_chat(clients->get(clients, i),
                             $(String.format("%s join the game as a spectator.", sender->name)));
            }

            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

    } else if (strcmp("name", action) == 0) {
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

        if (strncmp("Computer", name_token->valuestring, 8) == 0) {
            respond_error(sender, "You can't use that name");
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
            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

    } else if (strcmp("kick", action) == 0) {
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
            return;
        }

        cJSON *list = create_player_list();
        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s has been kicked by %s",
                                         is_computer ? name : target->name, sender->name)));

            if (clients->size + computer_count >= 7) {
                respond_chat(clients->get(clients, i),
                             $(String.format("%s becomes a player.",
                                             clients->get(clients, 6 - computer_count)->name)));
            }

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
            return;
        }

        char *message = message_token->valuestring;

        for (size_t i = 0; i < clients->size; i++) {
            respond_chat(clients->get(clients, i),
                         $(String.format("%s: %s", sender->name, message)));
        }

    } else if (strcmp("add_computer", action) == 0) {
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

            respond(clients->get(clients, i), "players", list, true);
        }
        cJSON_Delete(list);

    } else if (strcmp("start", action) == 0) {
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

        game = new_game();
        game_started = true;

        size_t real_player_count = clients->size;
        if (real_player_count + computer_count > 7) {
            real_player_count = 7 - computer_count;
        }

        for (size_t i = 0; i < real_player_count; i++) {
            if (clients->get(clients, i)->named == false) {
                clients->get(clients, i)->named = true;
            }
        }

        for (size_t i = 0; i < real_player_count; i++) {
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
            return;
        }

        cJSON *player = cJSON_GetObjectItem(payload, "player");
        if (player == NULL) {
            respond_error(sender, "Not choose player");
            return;
        }
        if (cJSON_IsNumber(player) == false) {
            respond_error(sender, "Player id should be a number");
            return;
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
            return;
        }

        cJSON *card = cJSON_GetObjectItem(payload, "card");
        if (card == NULL) {
            respond_error(sender, "No choose card");
            return;
        }
        if (cJSON_IsNumber(card) == false) {
            respond_error(sender, "Card offset should be a number");
            return;
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
            return;
        }

        cJSON *yn = cJSON_GetObjectItem(payload, "y/n");
        if (yn == NULL) {
            respond_error(sender, "Not make dicision");
            return;
        }
        if (cJSON_IsNumber(yn) == false) {
            respond_error(sender, "dicision should be a number");
            return;
        }

        int number = (int)cJSON_GetNumberValue(yn);
        share_num = number;
        sem_post(&waiting_for_input);
    }
}

static int protocol_bang(struct lws *instance, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    Client *client = find_client(instance);

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            Console.yellow("[%p] Connection established", instance);
            client = $(calloc(1, sizeof(Client)));
            client->instance = instance;
            client->msg_queue = create_Messages();
            client->named = false;
            strcpy(client->name, $(String.format("Player %s", $(random_string(8)))));
            client->player_id = -1;
            clients->push(clients, client);

            bang_action(client, "join", NULL);

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

            bang_action(client, action, payload);
            cJSON_Delete(json);

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
