#ifndef __CORE_CONSTANTS_H
#define __CORE_CONSTANTS_H

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

char* card_name[] = {"Hidden",    "Bang",         "Missed",     "Gatling",     "Indians",
                     "Panic",     "Cat Balou",    "Stagecoach", "Wells Fargo", "General Store",
                     "Beer",      "Saloon",       "Duel",       "Barrel",      "Scope",
                     "Mustang",   "Jail",         "Dynamite",   "Volcanic",    "Schofield",
                     "Remington", "Rev Carabine", "Winchester"

};

// number of {"", "Sheriff", "Deputy", "Criminal", "Traitor"} in 4~7 players
int role_in_players[4][5] = {{0, 1, 0, 2, 1}, {0, 1, 1, 2, 1}, {0, 1, 1, 3, 1}, {0, 1, 2, 3, 1}};
int game_start_role_in_players[4][5] = {
    {0, 1, 0, 2, 1}, {0, 1, 1, 2, 1}, {0, 1, 1, 3, 1}, {0, 1, 2, 3, 1}};
u64 card_base;

#endif  // __CORE_CONSTANTS_H
