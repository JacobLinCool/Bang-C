#ifndef __CORE_TYPES_H
#define __CORE_TYPES_H

#include "../utils/all.h"

typedef struct Game Game;
typedef void (*CharacterSkill)(Game* game, i32 player_id);

typedef enum RoleType {
    ROLE__LOW_BOUND,
    Sheriff,   //警長
    Deputy,    //副警長
    Criminal,  //歹徒
    Traitor,   //叛徒
    ROLE__HIGH_BOUND
} RoleType;

typedef enum CharacterType {
    CHARACTER_LOW_BOUND,
    Bart_Cassidy,  //巴特
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
    CardType type;     /** card id */
    u16      priority; /** suit * 100 + number 100spade 200heart 300diamond 400club*/

    void (*use)(Game* game, i32 player_id);
} Card;

StructVector(Cards, Card*, NULL);

typedef struct Player {
    char*      name;      /** player name, customizable */
    i8         bullet;    /** bullet count, equal to hp and max number of cards in hand */
    i8         hp;        /** player hp, default is character hp + role additional hp*/
    Role*      role;      /** player role, 4 types of roles */
    Character* character; /** player character */
    Card*      weapon;    /** weapon, NULL if no weapon */
    Card*      barrel;    /** barrel, NULL if no barrel */
    Card*      horse;     /** horse, NULL if no horse */
    Card*      scope;     /** scope, NULL if no scope */
    Card*      jail;      /** jail, NULL if no jail */
    Card*      bomb;      /** bomb, NULL if no bomb */
    Cards*     hands;     /** cards in hand */

    void (*play)(Game* game, i32 player_id);
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
    void (*join)(Game* game, const char* name, Agent agent);
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

#endif  // __CORE_TYPES_H
