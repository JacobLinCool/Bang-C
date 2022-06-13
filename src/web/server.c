#include <unistd.h>

#include "../third/cimple/all.h"
#include "../third/mkjson/mkjson.h"
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

void onopen(ws_cli_conn_t *client) {
    if (connections != NULL) {
        connections->push(connections, client);
    }

    char *address = ws_getaddress(client);
    Console.yellow("Connection opened, address: %s", address);

    ws_sendframe_txt(client, "What's your name?");
}

void onclose(ws_cli_conn_t *client) {
    if (connections != NULL) {
        for (size_t i = 0; i < connections->size; i++) {
            if (connections->get(connections, i) == client) {
                connections->remove(connections, i);
                break;
            }
        }
    }
    if (named_connections != NULL) {
        for (size_t i = 0; i < named_connections->size; i++) {
            if (named_connections->get(named_connections, i)->client == client) {
                named_connections->remove(named_connections, i);
                break;
            }
        }
    }

    char *address = ws_getaddress(client);
    Console.yellow("Connection closed, address: %s", address);
}

void onmessage(ws_cli_conn_t *client, const char *msg, size_t size, int type) {
    NamedConnection *sender = NULL;
    if (named_connections != NULL) {
        for (size_t i = 0; i < named_connections->size; i++) {
            if (named_connections->get(named_connections, i)->client == client) {
                sender = named_connections->get(named_connections, i);
                break;
            }
        }
    }

    if (sender == NULL) {
        if (msg && memcmp(msg, "name:", 5) == 0) {
            char *name = strdup(msg + 5);
            if (named_connections != NULL) {
                NamedConnection *connection = calloc(1, sizeof(NamedConnection));
                connection->client = client;
                connection->name = name;
                named_connections->push(named_connections, connection);
            }
            ws_sendframe_txt(client, String.format("Hello %s!", name));
        } else {
            ws_sendframe_txt(client, "What's your name?");
        }
    } else {
        Console.cyan("Received [%s] (%zu), from: %s", msg, size, sender->name);

        for (size_t i = 0; i < named_connections->size; i++) {
            NamedConnection *receiver = named_connections->get(named_connections, i);
            if (receiver->client != sender->client) {
                char *json =
                    mkjson(MKJSON_OBJ, 3, MKJSON_STRING, "from", sender->name, MKJSON_STRING, "to",
                           receiver->name, MKJSON_STRING, "message", msg);

                ws_sendframe_txt(receiver->client, json);

                free(json);
            }
        }
    }
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
