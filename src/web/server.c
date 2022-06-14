#include <libwebsockets.h>
#include <unistd.h>

#include "../third/cJSON/cJSON.h"
#include "../third/cimple/all.h"
#include "../third/uds/deque.h"
#include "../third/uds/vector.h"
#include "../utils/all.h"
#define BUFFER_SIZE (64 * 1024)

typedef struct Message {
    char  *payload;
    size_t len;
} Message;
StructDeque(Messages, Message *, NULL);

typedef struct Client {
    struct lws *instance;
    char        name[256];
    Messages   *msg_queue;
} Client;
StructVector(Clients, Client *, NULL);

Clients *clients = NULL;

static void free_message(Message *msg) {
    free(msg->payload);
    msg->payload = NULL;
    msg->len = 0;
}

Client *find_client(struct lws *instance) {
    for (int i = 0; i < clients->size; i++) {
        Client *client = clients->data[i];
        if (client->instance == instance) {
            return client;
        }
    }
    return NULL;
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
        // nothing!
    }

    lws_context_destroy(context);

    Console.info("WebSocket server stopped");
}

void handle_action(Client *client, char *action, cJSON *payload) {
    if (strcmp("stop", action) == 0) {
        exit(0);
    }

    Message *msg = malloc(sizeof(Message));
    msg->payload = cJSON_PrintUnformatted(payload);
    msg->len = strlen(msg->payload);
    client->msg_queue->push(client->msg_queue, msg);

    lws_callback_on_writable(client->instance);
}

static int event_handler(struct lws *instance, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    Client *client = find_client(instance);

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            if (clients->size >= 7) {
                Console.red("[%p] Room is full, Connection refused", instance);
                break;
            }

            Console.yellow("[%p] Connection established", instance);
            client = calloc(1, sizeof(Client));
            client->instance = instance;
            client->msg_queue = create_Messages();
            clients->push(clients, client);

            break;
        }

        case LWS_CALLBACK_RECEIVE: {
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
            lws_write(instance, msg->payload, msg->len, LWS_WRITE_TEXT);

            // free_message(msg);
            // free(msg);

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
                Client *client = clients->get(clients, i);
                if (client->instance == instance) {
                    clients->remove(clients, i);
                }
            }

            while (client->msg_queue->size > 0) {
                Message *msg = client->msg_queue->shift(client->msg_queue);
                free_message(msg);
            }

            client->msg_queue->free(client->msg_queue);
            free(client);

            break;
        }
    }

    return 0;
}

int main(void) {
    clients = create_Clients();

    start_server(8080, event_handler);

    return EXIT_SUCCESS;
}
