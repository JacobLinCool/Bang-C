#ifndef __UTILS_EVENT_H
#define __UTILS_EVENT_H

#include "base.h"
#include "vector.h"

typedef void (*Subscriber)(void* data);

// Event publish and subscribe system
typedef struct {
    /** Publish an event. */
    void (*publish)(const restrict string event, void* data);
    /** Publish an event. */
    void (*emit)(const restrict string event, void* data);
    /** Subscribe an event. */
    void (*subscribe)(const restrict string event, Subscriber subscriber);
    /** Subscribe an event. */
    void (*on)(const restrict string event, Subscriber subscriber);
    /** Unsubscribe an event. */
    void (*unsubscribe)(const restrict string event, Subscriber subscriber);
    /** Unsubscribe an event. */
    void (*off)(const restrict string event, Subscriber subscriber);
    /** Get the number of subscribers. */
    i32 (*count)(const restrict string event);
} EventUtils;

StructVector(Subscribers, Subscriber, NULL);

typedef struct __Event {
    string       name;
    Subscribers* subscribers;
} __Event;

StructVector(__Events, __Event*, NULL);

__Events* GLOBAL_EVENTS;

void event_init(__Events** events) { *events = create___Events(); }

void __event_publish(const restrict string event, void* data) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    u64 name_len = strlen(event);
    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (strncmp(GLOBAL_EVENTS->data[i]->name, event, name_len) == 0) {
            for (i32 j = 0; j < GLOBAL_EVENTS->data[i]->subscribers->size; j++) {
                GLOBAL_EVENTS->data[i]->subscribers->data[j](data);
            }
        }
    }
}

void __event_subscribe(const restrict string event, Subscriber subscriber) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    u64 name_len = strlen(event);
    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (strncmp(GLOBAL_EVENTS->data[i]->name, event, name_len) == 0) {
            GLOBAL_EVENTS->data[i]->subscribers->push(GLOBAL_EVENTS->data[i]->subscribers,
                                                      subscriber);
            return;
        }
    }

    __Event* new_event = calloc(1, sizeof(__Event));
    new_event->name = event;
    new_event->subscribers = create_Subscribers();
    new_event->subscribers->push(new_event->subscribers, subscriber);
    GLOBAL_EVENTS->push(GLOBAL_EVENTS, new_event);
}

void __event_unsubscribe(const restrict string event, Subscriber subscriber) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    u64 name_len = strlen(event);
    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (strncmp(GLOBAL_EVENTS->data[i]->name, event, name_len) == 0) {
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

i32 __event_count(const restrict string event) {
    if (GLOBAL_EVENTS == NULL) {
        event_init(&GLOBAL_EVENTS);
    }

    u64 name_len = strlen(event);
    for (i32 i = 0; i < GLOBAL_EVENTS->size; i++) {
        if (strncmp(GLOBAL_EVENTS->data[i]->name, event, name_len) == 0) {
            return GLOBAL_EVENTS->data[i]->subscribers->size;
        }
    }

    return 0;
}

EventUtils Event = {
    .publish = __event_publish,
    .emit = __event_publish,
    .subscribe = __event_subscribe,
    .on = __event_subscribe,
    .unsubscribe = __event_unsubscribe,
    .off = __event_unsubscribe,
    .count = __event_count,
};

#endif  // __UTILS_EVENT_H
