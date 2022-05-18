#ifndef __CORE_CHARACTERS_H
#define __CORE_CHARACTERS_H

#include "../utils/all.h"
#include "types.h"

Characters* get_characters() {
    Characters* characters = create_Characters();

    characters->push(characters, NULL);
}

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

#endif  // __CORE_CHARACTERS_H
