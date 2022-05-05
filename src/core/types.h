#ifndef __CORE_TYPES_H
#define __CORE_TYPES_H

#include "../utils/all.h"

typedef struct Game Game;
typedef void (*CharacterSkill)(Game* game, i32 player_id);

typedef enum CharacterType {
    CHARACTER_LOW_BOUND,
    Bart_Cassidy,
    Black_Jack,
    Calamity_Janet,
    El_Gringo,
    Jesse_Jones,
    Jourdonnais,
    Kit_Carlson,
    Lucky_Duke,
    Paul_Regret,
    Pedro_Ramirez,
    Rose_Doolan,
    Sid_Ketchum,
    Slab_the_Killer,
    Suzy_Lafayette,
    Vulture_Sam,
    Willy_the_Kid,
    CHARACTER_HIGH_BOUND,
} CharacterType;

typedef struct Role {
    char* name;
} Role;

StructVector(Roles, Role*, NULL);

typedef struct Character {
    char*          name;
    CharacterSkill skill;
} Character;

StructVector(Characters, Character*, NULL);

typedef struct Weapon {
    char* name;
} Weapon;

StructVector(Weapons, Weapon*, NULL);

typedef struct Card {
    char* name;
    void (*use)(Game* game, i32 player_id);
} Card;

typedef struct Player {
    char*      name;
    i8         bullet;
    Role*      role;
    Character* character;
    Weapons*   weapons;
    void (*play)(Game* game, i32 player_id);
} Player;

StructVector(Players, Player*, NULL);

typedef void (*Agent)(Game* game, i32 player_id);

typedef struct Game {
    char*    name;     /** Game name, related to event system. */
    Players* players;  /** Game players, including computer. */
    u64      turn;     /** 1 round may have N turns. */
    bool     finished; /** Game is finished. */
    /** Add a player to game. */
    void (*join)(Game* game, const char* name, Agent agent);
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
