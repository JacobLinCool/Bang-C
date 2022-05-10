#ifndef __UTILS_EVENT_H
#define __UTILS_EVENT_H

#include "../third/cimple/all.h"
#include "../third/uds/vector.h"

typedef void (*Subscriber)(void* data);
StructVector(Subscribers, Subscriber, NULL);
typedef struct __Event {
    i64          code;
    Subscribers* subscribers;
} __Event;

StructVector(__Events, __Event*, NULL);

__Events* GLOBAL_EVENTS;

void event_init(__Events** events) { *events = create___Events(); }

void __event_publish(const i64 event, void* data) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (GLOBAL_EVENTS->data[i]->code == event) {
            for (i32 j = 0; j < GLOBAL_EVENTS->data[i]->subscribers->size; j++) {
                GLOBAL_EVENTS->data[i]->subscribers->data[j](data);
            }
        }
    }
}

void __event_subscribe(const i64 event, Subscriber subscriber) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (GLOBAL_EVENTS->data[i]->code == event) {
            GLOBAL_EVENTS->data[i]->subscribers->push(GLOBAL_EVENTS->data[i]->subscribers,
                                                      subscriber);
            return;
        }
    }

    __Event* new_event = calloc(1, sizeof(__Event));
    new_event->code = event;
    new_event->subscribers = create_Subscribers();
    new_event->subscribers->push(new_event->subscribers, subscriber);
    GLOBAL_EVENTS->push(GLOBAL_EVENTS, new_event);
}

void __event_unsubscribe(const i64 event, Subscriber subscriber) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (GLOBAL_EVENTS->data[i]->code == event) {
            for (i32 j = 0; j < GLOBAL_EVENTS->data[i]->subscribers->size; j++) {
                if (GLOBAL_EVENTS->data[i]->subscribers->data[j] == subscriber) {
                    GLOBAL_EVENTS->data[i]->subscribers->remove(GLOBAL_EVENTS->data[i]->subscribers,
                                                                j);
                    if (GLOBAL_EVENTS->data[i]->subscribers->size == 0) {
                        GLOBAL_EVENTS->remove(GLOBAL_EVENTS, i);
                    }
                }
            }
        }
    }
}

i32 __event_count(const i64 event) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (GLOBAL_EVENTS->data[i]->code == event) {
            return GLOBAL_EVENTS->data[i]->subscribers->size;
        }
    }

    return 0;
}

/** Event publish and subscribe system */
struct {
    /** Publish an event. */
    void (*publish)(const i64 event, void* data);
    /** Publish an event. */
    void (*emit)(const i64 event, void* data);
    /** Subscribe an event. */
    void (*subscribe)(const i64 event, Subscriber subscriber);
    /** Subscribe an event. */
    void (*on)(const i64 event, Subscriber subscriber);
    /** Unsubscribe an event. */
    void (*unsubscribe)(const i64 event, Subscriber subscriber);
    /** Unsubscribe an event. */
    void (*off)(const i64 event, Subscriber subscriber);
    /** Get the number of subscribers. */
    i32 (*count)(const i64 event);
} Event = {
    .publish = __event_publish,
    .emit = __event_publish,
    .subscribe = __event_subscribe,
    .on = __event_subscribe,
    .unsubscribe = __event_unsubscribe,
    .off = __event_unsubscribe,
    .count = __event_count,
};

#endif  // __UTILS_EVENT_H
