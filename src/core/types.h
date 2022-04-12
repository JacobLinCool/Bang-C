#ifndef __CORE_TYPES_H
#define __CORE_TYPES_H

#include "../utils/all.h"

typedef struct Game Game;

typedef struct Role {
    string name;
} Role;

typedef struct Character {
    string name;
} Character;

typedef struct Weapon {
    string name;
} Weapon;

typedef struct Card {
    string name;
    void (*use)(Game* game, i32 player_id);
} Card;

typedef struct Player {
    string     name;
    i8         bullet;
    Role*      role;
    Character* character;
    Weapon*    weapon;
    void (*play)(Game* game, i32 player_id);
} Player;

StructVector(Roles, Role*, NULL);
StructVector(Characters, Character*, NULL);
StructVector(Weapons, Weapon*, NULL);
StructVector(Players, Player*, NULL);

typedef void (*Agent)(Game* game, i32 player_id);

typedef struct Game {
    string   name;     /** Game name, related to event system. */
    Players* players;  /** Game players, including computer. */
    u64      turn;     /** 1 round may have N turns. */
    bool     finished; /** Game is finished. */
    /** Add a player to game. */
    void (*join)(Game* game, const string name, Agent agent);
    /** Start the game, this will init something, such as shuffle cards. */
    void (*start)(Game* game);
    /** Call next player to play its turn. */
    void (*next)(Game* game);
} Game;

typedef enum GameEventType {
    GAME_START,
    PLAYER_CHANGED,
    CARD_GOT,
    CARD_USED,
    GAME_DONE,
} GameEventType;

typedef struct GameEvent {
    GameEventType type;
    Game*         game;
} GameEvent;

#endif  // __CORE_TYPES_H
