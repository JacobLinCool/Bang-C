#ifndef __CORE_CONSTANTS_H
#define __CORE_CONSTANTS_H

#include "../third/cimple/all.h"

#define ROLE_COUNT 7
#define CHARACTER_COUNT 16
#define CARD_COUNT 80
#define FAIL 0
#define SUCCESS 1

char* role_name[] = {"", "Sheriff", "Deputy", "Criminal", "Traitor"};
char* character_name[] = {"",
                          "Bart_Cassidy",
                          "Black_Jack",
                          "Calamity_Janet",
                          "El_Gringo",
                          "Jesse_Jones",
                          "Jourdonnais",
                          "Kit_Carlson",
                          "Lucky_Duke",
                          "Paul_Regret",
                          "Pedro_Ramirez",
                          "Rose_Doolan",
                          "Sid_Ketchum",
                          "Slab_the_Killer",
                          "Suzy_Lafayette",
                          "Vulture_Sam",
                          "Willy_the_Kid"};

char* card_name[] = {"Hidden",    "BANG",         "MISSED",     "GATLING",     "INDIANS",
                     "PANIC",     "CAT BALOU",    "STAGECOACH", "WELLS FORGO", "GENERAL STORE",
                     "BEER",      "SALOON",       "DUEL",       "BARREL",      "SCOPE",
                     "MUSTANG",   "JAIL",         "DYNAMITE",   "VOLVANIC",    "SCHOFIELD",
                     "REMINGTON", "REV CARABINE", "WINCHESTER"

};

char* suit[] = {"", "Spade", "Heart", "Diamond", "Club"};

// number of {"", "Sheriff", "Deputy", "Criminal", "Traitor"} in 4~7 players
int role_in_players[4][5] = {{0, 1, 0, 2, 1}, {0, 1, 1, 2, 1}, {0, 1, 1, 3, 1}, {0, 1, 2, 3, 1}};
u64 card_base;
u32 key, key2;

#endif  // __CORE_CONSTANTS_H
