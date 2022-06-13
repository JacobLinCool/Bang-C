#ifndef __CORE_TYPES_H
#define __CORE_TYPES_H

#include "../third/cimple/all.h"
#include "../utils/all.h"
#include "cards.h"

typedef struct Game Game;
typedef void (*CharacterSkill)(Game* game, i32 player_id);

typedef enum RoleType {
    ROLE_LOW_BOUND,
    Sheriff,   // 警長
    Deputy,    // 副警長
    Criminal,  // 歹徒
    Traitor,   // 叛徒
    ROLE_HIGH_BOUND
} RoleType;

typedef enum CharacterType {
    CHARACTER_LOW_BOUND,
    Bart_Cassidy,  // In Progress(Takala)
    Black_Jack,
    Calamity_Janet,
    El_Gringo,  // Done
    Jesse_Jones,
    Jourdonnais,  // Done
    Kit_Carlson,
    Lucky_Duke,   // Done
    Paul_Regret,  // Done
    Pedro_Ramirez,
    Rose_Doolan,      // Done
    Sid_Ketchum,      // Done, Todo: request
    Slab_the_Killer,  // Done
    Suzy_Lafayette,
    Vulture_Sam,  // Done
    Willy_the_Kid,
    CHARACTER_HIGH_BOUND,
} CharacterType;

typedef enum CardType {
    CARD_LOW_BOUND,
    Bang,
    Missed,
    Gatling,
    Indians,
    Panic,
    Cat_Balou,
    Stagecoach,
    Wells_Fargo,
    General_Store,
    Beer,
    Saloon,
    Duel,
    Barrel,
    Scope,
    Mustang,
    Jail,
    Dynamite,
    Volcanic,
    Schofield,
    Remington,
    Rev_Carabine,
    Winchester,
    CARD_HIGH_BOUND,
} CardType;

typedef struct Role {
    RoleType type;
} Role;

StructVector(Roles, Role*, NULL);

typedef struct Character {
    CharacterType  type;
    CharacterSkill skill;
    u8             health;
} Character;

StructVector(Characters, Character*, NULL);

typedef struct Card {
    CardType type; /** card id */
    /** priority = suit * 100 + number; 100: spade 200: heart 300: diamond 400: club */
    u16 priority;

    bool (*use)(Game* game, i32 player_id);
} Card;

StructVector(Cards, Card*, NULL);

typedef struct Player {
    char*      name;      /** player name, customizable */
    i8         id;        /** player id */
    i8         bullet;    /** bullet count, equal to hp and max number of cards in hand */
    i8         hp;        /** player hp, default is character hp + role additional hp*/
    Role*      role;      /** player role, 4 types of roles */
    Character* character; /** player character */
    Card*      weapon;    /** weapon, NULL if no weapon */
    Card*      barrel;    /** barrel, NULL if no barrel */
    Card*      mustang;   /** horse, NULL if no mustang */
    Card*      scope;     /** scope, NULL if no scope */
    Card*      jail;      /** jail, NULL if no jail */
    Card*      dynamite;  /** bomb, NULL if no dynamite */
    Cards*     hands;     /** cards in hand */

    i32 (*choose_enemy)(Game* game, i32 me_id);

    void (*play)(Game* game, i32 player_id);
    /**
     * @brief Select one card from a set of cards.
     * @return true if selected, false if failed.
     */
    bool (*select)(Game* game, i32 player_id, Cards* cards);
    /**
     * @brief Request a card from player's hand.
     * @return the card if success, NULL if failed.
     */
    Card* (*request)(Game* game, i32 player_id);
    /**
     * @brief Take a card from player's hand or equipment.
     * @return the card if success, NULL if failed.
     */
    Card* (*take)(Game* game, i32 player_id, i32 target_id);
} Player;

StructVector(Players, Player*, NULL);

typedef void (*Agent)(Game* game, i32 player_id);

typedef struct Game {
    Players*    players;    /** game players, including computer */
    u64         turn;       /** 1 round may have N turns */
    bool        finished;   /** game is finished */
    Cards*      deck;       /** cards in deck */
    Cards*      discard;    /** cards in discard */
    Roles*      roles;      /** roles to be drawn */
    Characters* characters; /** characters to be drawn */

    /** add a player to game */
    void (*join)(Game* game, const char* name, bool is_computer);
    /** start the game, this will init something, such as shuffle cards */
    void (*start)(Game* game);
    /** call next player to play its turn */
    void (*next)(Game* game);
} Game;

enum GameEventType {
    EVT_GAME_LOAD = 101,
    EVT_GAME_PLAYER_JOIN,
    EVT_GAME_INIT,
    EVT_GAME_PLAYER_CHANGED,
    EVT_GAME_DONE,
};

enum PlayerEventType {
    EVT_DRAW = 201,
    EVT_ATTACKED,
};

typedef struct EventPayload {
    Game* game;
    i32   trigger_id;
    i32   target_id;

} EventPayload;

Role roles[] = {
    {.type = Sheriff},
    {.type = Deputy},
    {.type = Criminal},
    {.type = Traitor},
};

Character chars[] = {
    {.type = Bart_Cassidy, .health = 4, .skill = NULL},
    {.type = Black_Jack, .health = 4, .skill = NULL},
    {.type = Calamity_Janet, .health = 4, .skill = NULL},
    {.type = El_Gringo, .health = 3, .skill = NULL},
    {.type = Jesse_Jones, .health = 4, .skill = NULL},
    {.type = Jourdonnais, .health = 4, .skill = NULL},
    {.type = Kit_Carlson, .health = 4, .skill = NULL},
    {.type = Lucky_Duke, .health = 4, .skill = NULL},
    {.type = Paul_Regret, .health = 3, .skill = NULL},
    {.type = Pedro_Ramirez, .health = 4, .skill = NULL},
    {.type = Rose_Doolan, .health = 4, .skill = NULL},
    {.type = Sid_Ketchum, .health = 4, .skill = NULL},
    {.type = Slab_the_Killer, .health = 4, .skill = NULL},
    {.type = Suzy_Lafayette, .health = 4, .skill = NULL},
    {.type = Vulture_Sam, .health = 4, .skill = NULL},
    {.type = Willy_the_Kid, .health = 4, .skill = NULL},
};

#endif  // __CORE_TYPES_H
