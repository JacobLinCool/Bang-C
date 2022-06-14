#include <unistd.h>

#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"
#include "../third/uds/vector.h"
#include "../third/wsServer/include/ws.h"
#include "../utils/all.h"

StructVector(Connections, ws_cli_conn_t *, NULL);

typedef struct NamedConnection {
    ws_cli_conn_t *client;
    char          *name;
} NamedConnection;

StructVector(NamedConnections, NamedConnection *, NULL);

Connections      *connections = NULL;
NamedConnections *named_connections = NULL;

/**
 * @brief Send a JSON message with a given type and payload. Payload will be freed after sending.
 *
 * @param client
 * @param type
 * @param payload
 * @return int
 */
int send_message(ws_cli_conn_t *client, const char *type, cJSON *payload) {
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "type", cJSON_CreateStringReference(type));
    cJSON_AddItemToObject(obj, "payload", payload);

    char *json = cJSON_PrintUnformatted(obj);

    int length = ws_sendframe_txt(client, json);

    free(json);
    cJSON_Delete(obj);

    return length;
}

/**
 * @brief Broadcast a message to all clients.
 *
 * @param type
 * @param payload
 */
void broadcast(const char *type, cJSON *payload) {
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "type", cJSON_CreateStringReference(type));
    cJSON_AddItemToObject(obj, "payload", payload);

    char *json = cJSON_PrintUnformatted(obj);

    for (int i = 0; i < named_connections->size; i++) {
        ws_sendframe_txt(named_connections->get(named_connections, i)->client, json);
    }

    free(json);
    cJSON_Delete(obj);
}

void onopen(ws_cli_conn_t *client) {
    connections->push(connections, client);

    char *address = ws_getaddress(client);
    Console.yellow("Connection opened, address: %s", address);

    cJSON *obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "question", cJSON_CreateStringReference("What's your name?"));

    send_message(client, "ask", obj);
}

void onclose(ws_cli_conn_t *client) {
    for (size_t i = 0; i < connections->size; i++) {
        if (connections->get(connections, i) == client) {
            connections->remove(connections, i);
            break;
        }
    }

    for (size_t i = 0; i < named_connections->size; i++) {
        if (named_connections->get(named_connections, i)->client == client) {
            NamedConnection *nc = named_connections->remove(named_connections, i);

            cJSON *obj = cJSON_CreateObject();
            cJSON_AddItemToObject(obj, "name", cJSON_CreateStringReference(nc->name));

            broadcast("leave", obj);
            Console.magenta("%s Left!", nc->name);
            free(nc->name);
            break;
        }
    }

    char *address = ws_getaddress(client);
    Console.yellow("Connection closed, address: %s", address);
}

void onmessage(ws_cli_conn_t *client, const unsigned char *msg, uint64_t size, int type) {
    NamedConnection *sender = NULL;

    for (size_t i = 0; i < named_connections->size; i++) {
        if (named_connections->get(named_connections, i)->client == client) {
            sender = named_connections->get(named_connections, i);
            break;
        }
    }

    if (msg == NULL) {
        Console.red("Invalid msg");
        return;
    }

    cJSON *json = cJSON_Parse((const char *)msg);

    if (json == NULL) {
        Console.red("Invalid JSON");
        return;
    }

    cJSON *action_token = cJSON_GetObjectItem(json, "action");
    char  *action = NULL;
    if (cJSON_IsString(action_token) && (action_token->valuestring != NULL)) {
        action = action_token->valuestring;
    } else {
        cJSON_Delete(json);
        return;
    }

    // Console.blue("%s: %s", sender ? sender->name : "Unnamed", action);

    if (sender == NULL) {
        if (strcmp(action, "name") == 0) {
            cJSON *name_token = cJSON_GetObjectItem(json, "name");
            char  *name = NULL;

            if (cJSON_IsString(name_token) && name_token->valuestring != NULL) {
                name = $(strdup(name_token->valuestring));
            } else {
                cJSON_Delete(json);
                return;
            }

            if (memcmp(name, "SERVER", 6) == 0) {
                cJSON *obj = cJSON_CreateObject();
                cJSON_AddItemToObject(
                    obj, "question",
                    cJSON_CreateStringReference(
                        "You can't take \"SERVER\" as your name. Use another one."));
                send_message(client, "ask", obj);

                free(name);
                return;
            }

            for (size_t i = 0; i < named_connections->size; i++) {
                if (strcmp(named_connections->get(named_connections, i)->name, name) == 0) {
                    cJSON *obj = cJSON_CreateObject();
                    cJSON_AddItemToObject(obj, "question",
                                          cJSON_CreateStringReference(
                                              "This name is already taken. Use another one."));
                    send_message(client, "ask", obj);

                    free(name);
                    return;
                }
            }

            NamedConnection *nc = calloc(1, sizeof(NamedConnection));
            nc->client = client;
            nc->name = name;
            named_connections->push(named_connections, nc);

            cJSON *obj = cJSON_CreateObject();
            cJSON_AddItemToObject(obj, "from", cJSON_CreateStringReference("SERVER"));
            cJSON_AddItemToObject(obj, "to", cJSON_CreateStringReference(name));
            cJSON_AddItemToObject(obj, "message",
                                  cJSON_CreateStringReference($(String.format("Hello %s!", name))));

            send_message(client, "chat", obj);

            obj = cJSON_CreateObject();
            cJSON *players = cJSON_CreateArray();
            for (size_t i = 0; i < named_connections->size; i++) {
                cJSON_AddItemToArray(
                    players,
                    cJSON_CreateString(named_connections->get(named_connections, i)->name));
            }
            cJSON_AddItemToObject(obj, "players", players);
            send_message(client, "players", obj);

            obj = cJSON_CreateObject();
            cJSON_AddItemToObject(obj, "name", cJSON_CreateStringReference(name));

            broadcast("join", obj);
            Console.magenta("%s Joined!", name);
        } else {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddItemToObject(obj, "question",
                                  cJSON_CreateStringReference("What's your name?"));

            send_message(client, "ask", obj);
        }
    } else if (strcmp(action, "chat") == 0) {
        cJSON *message_token = cJSON_GetObjectItem(json, "message");
        char  *message = NULL;

        if (cJSON_IsString(message_token) && (message_token->valuestring != NULL)) {
            message = message_token->valuestring;
        } else {
            cJSON_Delete(json);
            return;
        }

        Console.cyan("Received [%s], from: %s", message, sender->name);

        cJSON *to_token = cJSON_GetObjectItem(json, "to");
        char  *to = NULL;

        if (cJSON_IsString(to_token) && (to_token->valuestring != NULL)) {
            to = to_token->valuestring;
        } else {
            cJSON_Delete(json);
            return;
        }

        if (strcmp(to, "SERVER") == 0) {
            for (size_t i = 0; i < named_connections->size; i++) {
                NamedConnection *receiver = named_connections->get(named_connections, i);
                cJSON           *obj = cJSON_CreateObject();
                cJSON_AddItemToObject(obj, "from", cJSON_CreateString("SERVER"));
                cJSON_AddItemToObject(obj, "to", cJSON_CreateString(receiver->name));
                cJSON_AddItemToObject(obj, "message", cJSON_CreateString(message));

                send_message(receiver->client, "chat", obj);
            }
        } else {
            for (size_t i = 0; i < named_connections->size; i++) {
                NamedConnection *receiver = named_connections->get(named_connections, i);
                if (strcmp(receiver->name, to) == 0) {
                    cJSON *obj = cJSON_CreateObject();
                    cJSON_AddItemToObject(obj, "from", cJSON_CreateString(sender->name));
                    cJSON_AddItemToObject(obj, "to", cJSON_CreateString(receiver->name));
                    cJSON_AddItemToObject(obj, "message", cJSON_CreateString(message));

                    send_message(receiver->client, "chat", obj);

                    if (receiver->client != sender->client) {
                        obj = cJSON_CreateObject();
                        cJSON_AddItemToObject(obj, "from", cJSON_CreateString(sender->name));
                        cJSON_AddItemToObject(obj, "to", cJSON_CreateString(receiver->name));
                        cJSON_AddItemToObject(obj, "message", cJSON_CreateString(message));

                        send_message(sender->client, "chat", obj);
                    }

                    break;
                }
            }
        }
    }

    cJSON_Delete(json);
}

int main(void) {
    connections = create_Connections();
    named_connections = create_NamedConnections();

    struct ws_events evt = {
        .onopen = onopen,
        .onclose = onclose,
        .onmessage = onmessage,
    };

    ws_socket(&evt, 8080, 0, 1000);

    return EXIT_SUCCESS;
}
