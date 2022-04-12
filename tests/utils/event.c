#include "../../src/utils/event.h"

#include <assert.h>

typedef struct PayloadA {
    char* name;
    int   balance;
} PayloadA;

PayloadA payload_a = {
    .name = "Payload A",
    .balance = 100,
};

void handle_EventA_1(PayloadA* data) {
    assert(data != &payload_a);
    assert(strcmp(data->name, payload_a.name) == 0);
}
void handle_EventA_2(PayloadA* data) {
    assert(data != &payload_a);
    assert(data->balance == 100);
}

int main() {
    Event.on("event A", (Subscriber)handle_EventA_1);
    Event.on("event A", (Subscriber)handle_EventA_2);

    PayloadA* payload_a_copy = malloc(sizeof(PayloadA));
    memcpy(payload_a_copy, &payload_a, sizeof(PayloadA));

    Event.emit("event A", payload_a_copy);

    assert(Event.count("event A") == 2);

    Event.off("event A", (Subscriber)handle_EventA_1);

    assert(Event.count("event A") == 1);

    Event.off("event A", (Subscriber)handle_EventA_2);

    Event.emit("event A", payload_a_copy);

    assert(Event.count("event A") == 0);

    return EXIT_SUCCESS;
}
