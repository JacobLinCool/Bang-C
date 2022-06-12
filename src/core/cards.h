#ifndef __CORE_CARD_H
#define __CORE_CARD_H

#include "../utils/all.h"
#include "constants.h"
#include "player.h"
#include "types.h"
#include "utils.h"


bool died_player(Game* game, i32 me_id, i32 enemy_id) {
    Player* enemy = game->players->data[enemy_id];
    if (enemy->hp > 0) return SUCCESS;
    // use beer
    // if request() execute the card, then do nothing in if().
    // if (request(game, enemy_id, Beer)) return FAIL;

    // shows his role card
    // View layer Todo: call show(Game* game, i32 player_id)

    // END OF THE GAME detection
    // find Criminal, Traitor.

    if (alive(game, Sheriff) == false) {
        // Sheriff died
        game->finished = true;
    } else if (alive(game, Criminal) == false && alive(game, Traitor) == false) {
        // Criminal and Traitor died
        game->finished = true;
    }

    if (game->finished) return SUCCESS;

    // discard all cards
    Cards* discard_card = game->discard;
    if (me_id != enemy_id && game->players->data[me_id]->character->type == Vulture_Sam)
        discard_card = game->players->data[me_id]->hands;
    transfer(enemy->hands, discard_card);
    discard_card->push(discard_card, enemy->weapon);
    discard_card->push(discard_card, enemy->barrel);
    discard_card->push(discard_card, enemy->mustang);
    discard_card->push(discard_card, enemy->scope);
    discard_card->push(discard_card, enemy->jail);
    discard_card->push(discard_card, enemy->dynamite);

    // Penalties and Rewards
    if (me_id == enemy_id) return SUCCESS;
    Player* me = game->players->data[me_id];
    if (enemy->role->type == Deputy && me->role->type == Sheriff) {
        // Sheriff discards all cards
        discard_card = game->discard;
        transfer(me->hands, discard_card);
        discard_card->push(discard_card, me->weapon);
        discard_card->push(discard_card, me->barrel);
        discard_card->push(discard_card, me->mustang);
        discard_card->push(discard_card, me->scope);
        discard_card->push(discard_card, me->jail);
        discard_card->push(discard_card, me->dynamite);
    } else if (enemy->role->type == Criminal) {
        player_draw_deck(game, me_id, 3);
    }
    return SUCCESS;
}

// If no me_id, me_id = player_id
void attack_player(Game* game, i32 me_id, i32 player_id) {
    if (__get_player_hp(game, player_id) <= 0) return;  // avoid mustang_judge error
    // decrease player's hp
    game->players->data[player_id]->hp--;
    // use character ablity(if valid)
    CharacterType me_type = game->players->data[player_id]->character->type;
    if (me_type == Bart_Cassidy || me_type == El_Gringo || me_type == Sid_Ketchum) {
        if (me_type == El_Gringo) {
            draw_from_player(game, me_id, player_id);
        } else {
            game->players->data[player_id]->character->skill(game, player_id);
        }
    }

    if (game->players->data[player_id]->hp != 0) return;

    // dead
    died_player(game, me_id, player_id);
    return;
}

void bang_no_distance(Game* game, i32 me_id, i32 enemy_id) {
    // Barrel
    i32     missed_total = 0;
    Player* enemy = game->players->data[enemy_id];
    if (enemy->barrel != NULL) {
        if (judge(game, enemy_id, 201, 213)) missed_total++;
    }
    if (enemy->character->type == Jourdonnais) {
        if (judge(game, enemy_id, 201, 213)) missed_total++;
    }
    if (missed_total >= 1 + (game->players->data[me_id]->character->type == Slab_the_Killer))
        return;
    /*
    while(request(game, enemy_id, Missed)){
        missed_total++;
        if(missed_total>=1+(game->players->data[me_id]->character->type == Slab_the_Killer))
            return;
    }
    */
    attack_player(game, me_id, enemy_id);
    return;
}

bool bang(Game* game, i32 me_id) {
    i32 enemy_id = choose_enemy(game, me_id); 
    if(enemy_id == -1) return FAIL;

    // calculate distance between me and enemy
    i32 enemy_distance = distance(game, me_id, enemy_id);
    // my weapon distance
    i32 weapon_distance = 1;
    if (game->players->data[me_id]->weapon != NULL)
        weapon_distance += game->players->data[me_id]->weapon->type - Volcanic;
    // whether my weapon can reach the enemy

    if (weapon_distance < enemy_distance) return FAIL;
    bang_no_distance(game, me_id, enemy_id);
    return SUCCESS;
}

bool missed(Game* game, i32 me_id) { return FAIL; }

bool gatling(Game* game, i32 me_id) {
    for (int i = 0; i < game->players->size; i++) {
        if (get_player_hp(game, me_id) <= 0 || me_id == i) continue;
        bang_no_distance(game, me_id, i);
    }
    return SUCCESS;
}

// Todo: request
bool indians(Game* game, i32 me_id) {
    for (int i = 0; i < game->players->size; i++) {
        if (get_player_hp(game, me_id) <= 0 || me_id == i) continue;
        // if(request(game, i, Bang))continue;
        attack_player(game, me_id, i);
    }
    return SUCCESS;
}

bool panic(Game* game, i32 me_id) {
    i32 enemy_id = choose_enemy(game, me_id);
    if( enemy_id == -1 ) return FAIL;
    
    // calculate distance between me and enemy
    if (distance(game, me_id, enemy_id) > 1) return FAIL;

    draw_from_player(game, me_id, enemy_id);
    return SUCCESS;
}

bool stagecoach(Game* game, i32 me_id) {
    player_draw_deck(game, me_id, 2);
    return SUCCESS;
}

bool wells_fargo(Game* game, i32 me_id) {
    player_draw_deck(game, me_id, 3);
    return SUCCESS;
}

bool beer(Game* game, i32 me_id) {
    if (game->players->size <= 2) return FAIL;
    if (game->players->data[me_id]->hp == game->players->data[me_id]->character->health +
                                              (game->players->data[me_id]->role->type == Sheriff))
        return FAIL;
    game->players->data[me_id]->hp++;
    return SUCCESS;
}

bool saloon(Game* game, i32 me_id) {
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp <= 0) continue;
        recover(game, i);
    }
    return SUCCESS;
}

bool duel(Game* game, i32 me_id) {
    i32 enemy_id;  //= choose_enemy(); Layer View Todo // enemy died should be in this function

    // duel
    while (true) {
        /*
        if(!request(game, enemy_id, Bang)){
            attack_player(game, me_id, enemy_id);
            break;
        }
        if(!request(game, me_id, Bang)){
            attack_player(game, -1, me_id);
            break;
        }
        */
    }
    return SUCCESS;
}

bool barrel(Game* game, i32 me_id) {
    if (game->players->data[me_id]->barrel != NULL) return FAIL;
    return SUCCESS;
}

bool scope(Game* game, i32 me_id) {
    if (game->players->data[me_id]->scope != NULL) return FAIL;
    return SUCCESS;
}

bool mustang(Game* game, i32 me_id) {
    if (game->players->data[me_id]->mustang != NULL) return FAIL;
    return SUCCESS;
}

bool dynamite_judge(Game* game, i32 me_id) {
    Player* me = game->players->data[me_id];
    if (judge(game, me_id, 102, 109)) {
        game->discard->push(game->discard, me->dynamite);
        me->dynamite = NULL;
        attack_player(game, -1, me_id);
        attack_player(game, -1, me_id);
        attack_player(game, -1, me_id);
    } else {
        i32 left_player_id = (me_id + 1) % game->players->size;
        while (game->players->data[left_player_id]->hp <= 0) {
            left_player_id = (left_player_id + 1) % game->players->size;
        }
        game->players->data[left_player_id]->mustang = me->mustang;
        me->mustang = NULL;
    }
    return SUCCESS;
}

bool jail(Game* game, i32 me_id) {
    i32 enemy_id;  //= choose_enemy(); View Todo
    if (game->players->data[enemy_id]->hp <= 0) return FAIL;
    if (game->players->data[enemy_id]->role->type == Sheriff) return FAIL;
    return SUCCESS;
}

bool jail_judge(Game* game, i32 me_id) {
    game->discard->push(game->discard, game->players->data[me_id]->jail);
    game->players->data[me_id]->jail = NULL;
    if (judge(game, me_id, 201, 213)) return SUCCESS;  // SUCCESS escapes from jail
    return FAIL;                                       // FAIL escapes from jail
}

bool dynamite(Game* game, i32 me_id) {
    if (game->players->data[me_id]->dynamite != NULL) return FAIL;
    return SUCCESS;
}

bool volcanic(Game* game, i32 me_id) {
    if (game->players->data[me_id]->weapon != NULL) return FAIL;
    return SUCCESS;
}

bool schofield(Game* game, i32 me_id) {
    if (game->players->data[me_id]->weapon != NULL) return FAIL;
    return SUCCESS;
}

bool remington(Game* game, i32 me_id) {
    if (game->players->data[me_id]->weapon != NULL) return FAIL;
    return SUCCESS;
}

bool rev_carabine(Game* game, i32 me_id) {
    if (game->players->data[me_id]->weapon != NULL) return FAIL;
    return SUCCESS;
}

bool winchester(Game* game, i32 me_id) {
    if (game->players->data[me_id]->weapon != NULL) return FAIL;
    return SUCCESS;
}

Card decks[] = {{.type = Bang, .priority = 101, .use = bang},  // Done, Todo: request
                {.type = Bang, .priority = 201, .use = bang},
                {.type = Bang, .priority = 212, .use = bang},
                {.type = Bang, .priority = 213, .use = bang},
                {.type = Bang, .priority = 301, .use = bang},
                {.type = Bang, .priority = 302, .use = bang},
                {.type = Bang, .priority = 303, .use = bang},
                {.type = Bang, .priority = 304, .use = bang},
                {.type = Bang, .priority = 305, .use = bang},
                {.type = Bang, .priority = 306, .use = bang},
                {.type = Bang, .priority = 307, .use = bang},
                {.type = Bang, .priority = 308, .use = bang},
                {.type = Bang, .priority = 309, .use = bang},
                {.type = Bang, .priority = 310, .use = bang},
                {.type = Bang, .priority = 311, .use = bang},
                {.type = Bang, .priority = 312, .use = bang},
                {.type = Bang, .priority = 313, .use = bang},
                {.type = Bang, .priority = 402, .use = bang},
                {.type = Bang, .priority = 403, .use = bang},
                {.type = Bang, .priority = 404, .use = bang},
                {.type = Bang, .priority = 405, .use = bang},
                {.type = Bang, .priority = 406, .use = bang},
                {.type = Bang, .priority = 407, .use = bang},
                {.type = Bang, .priority = 408, .use = bang},
                {.type = Bang, .priority = 409, .use = bang},
                {.type = Missed, .priority = 102, .use = missed},
                {.type = Missed, .priority = 103, .use = missed},
                {.type = Missed, .priority = 104, .use = missed},
                {.type = Missed, .priority = 105, .use = missed},
                {.type = Missed, .priority = 106, .use = missed},
                {.type = Missed, .priority = 107, .use = missed},
                {.type = Missed, .priority = 108, .use = missed},
                {.type = Missed, .priority = 401, .use = missed},
                {.type = Missed, .priority = 410, .use = missed},
                {.type = Missed, .priority = 411, .use = missed},
                {.type = Missed, .priority = 412, .use = missed},
                {.type = Missed, .priority = 413, .use = missed},
                {.type = Gatling, .priority = 210, .use = gatling},
                {.type = Indians, .priority = 301, .use = NULL},
                {.type = Indians, .priority = 313, .use = NULL},
                {.type = Panic, .priority = 201, .use = NULL},  //= choose_enemy(); View Todo
                {.type = Panic, .priority = 211, .use = NULL},
                {.type = Panic, .priority = 212, .use = NULL},
                {.type = Panic, .priority = 308, .use = NULL},
                {.type = Cat_Balou, .priority = 213, .use = NULL},
                {.type = Cat_Balou, .priority = 309, .use = NULL},
                {.type = Cat_Balou, .priority = 310, .use = NULL},
                {.type = Cat_Balou, .priority = 311, .use = NULL},
                {.type = Stagecoach, .priority = 109, .use = stagecoach},
                {.type = Stagecoach, .priority = 109, .use = stagecoach},
                {.type = Wells_Fargo, .priority = 203, .use = wells_fargo},
                {.type = General_Store, .priority = 112, .use = NULL},
                {.type = General_Store, .priority = 409, .use = NULL},
                {.type = Beer, .priority = 206, .use = beer},
                {.type = Beer, .priority = 207, .use = beer},
                {.type = Beer, .priority = 208, .use = beer},
                {.type = Beer, .priority = 209, .use = beer},
                {.type = Beer, .priority = 210, .use = beer},
                {.type = Beer, .priority = 211, .use = beer},
                {.type = Saloon, .priority = 205, .use = saloon},
                {.type = Duel, .priority = 111, .use = NULL},  // Done, Todo: request,choose_enemy
                {.type = Duel, .priority = 312, .use = NULL},
                {.type = Duel, .priority = 408, .use = NULL},
                {.type = Barrel, .priority = 112, .use = barrel},
                {.type = Barrel, .priority = 113, .use = barrel},
                {.type = Scope, .priority = 101, .use = scope},
                {.type = Mustang, .priority = 208, .use = mustang},
                {.type = Mustang, .priority = 209, .use = mustang},
                {.type = Jail, .priority = 110, .use = jail},
                {.type = Jail, .priority = 111, .use = jail},
                {.type = Jail, .priority = 204, .use = jail},
                {.type = Dynamite, .priority = 202, .use = dynamite},
                {.type = Volcanic, .priority = 110, .use = volcanic},
                {.type = Volcanic, .priority = 410, .use = volcanic},
                {.type = Schofield, .priority = 113, .use = schofield},
                {.type = Schofield, .priority = 411, .use = schofield},
                {.type = Schofield, .priority = 412, .use = schofield},
                {.type = Remington, .priority = 413, .use = remington},
                {.type = Rev_Carabine, .priority = 401, .use = rev_carabine},
                {.type = Winchester, .priority = 108, .use = winchester}};

#endif  // __CORE_CARD_H