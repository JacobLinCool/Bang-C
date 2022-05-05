#ifndef __CORE_CHARACTERS_H
#define __CORE_CHARACTERS_H

#include "../utils/all.h"
#include "types.h"

Characters* get_characters() {
    Characters* characters = create_Characters();

    characters->push(characters, NULL);
}

#endif  // __CORE_CHARACTERS_H
