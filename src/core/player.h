#ifndef __CORE_PLAYER_H
#define __CORE_PLAYER_H
#define FAIL 1
#include "../utils/all.h"
#include "types.h"

void real_player(Game* game, i32 player_id) {
    printf("We didn't implement anything yet, so just enter anything: ");
    char* input = $(calloc(1024, sizeof(char)));
    scanf("%*[\n]");
    scanf("%[^\n]%*c", input);
}

void computer_player(Game* game, i32 player_id) {
    Console.gray("My name is %s.", game->players->data[player_id]->name);
    Console.gray("I am a computer, I will do nothing.");
}

/*
If no me_id, enter -1
Dangerous! use carefully, it let a player die, regardless he has hp or not.
Contain beer request.
*/
bool died_player(Game* game, i32 me_id, i32 enemy_id) {
    Player* enemy = game->players->data[enemy_id];
    // use beer
    // if request() execute the card, then do nothing in if().
    if (request(game, enemy_id, Beer)) return 0;

    // shows his role card
    // View layer Todo: call show(Game* game, i32 player_id)

    // END OF THE GAME detection
    if (end_of_game(game)) return 0;

    // discard all cards
    Cards* add_card = game->discard;
    if (me_id != -1 && game->players->data[me_id]->character->type == Vulture_Sam)
        add_card = game->players->data[me_id]->hands;
    add_card->concat(add_card, enemy->hands);
    add_card->push(add_card, enemy->weapon);
    add_card->push(add_card, enemy->barrel);
    add_card->push(add_card, enemy->horse);
    add_card->push(add_card, enemy->scope);
    add_card->push(add_card, enemy->jail);
    add_card->push(add_card, enemy->bomb);

    // Penalties and Rewards
    if (me_id == -1) return 0;
    Player* me = game->players->data[me_id];
    if (enemy->role->type == Deputy && me->role->type == Sheriff) {
        // Sheriff discards all cards
        add_card = game->discard;
        add_card->concat(add_card, me->hands);
        add_card->push(add_card, me->weapon);
        add_card->push(add_card, me->barrel);
        add_card->push(add_card, me->horse);
        add_card->push(add_card, me->scope);
        add_card->push(add_card, me->jail);
        add_card->push(add_card, me->bomb);
    } else if (enemy->role->type == Criminal) {
        draw_from_deck(game, me_id, 3);
    }
    return 0;
}

// If no me_id, enter -1
bool attack_player(Game* game, i32 me_id, i32 player_id) {
    // player died?
    if (game->players->data[player_id]->hp <= 0) return FAIL;
    // decrease player's hp
    game->players->data[player_id]->hp--;
    // use character ablity(if valid)
    CharacterType me_type = game->players->data[player_id]->character->type;
    if (me_type == Bart_Cassidy || me_type == El_Gringo || me_type == Sid_Ketchum) {
        game->players->data[player_id]->character->skill(game, player_id);
    }

    if (game->players->data[player_id]->hp != 0) return 0;

    // dead
    return died_player(game, me_id, player_id);
}

// Why not just draw from the first card? It's for FUN!
bool draw_from_player(Game* game, i32 me_id, i32 enemy_id) {
    // player died?
    if (game->players->data[enemy_id]->hp <= 0) return FAIL;

    i8 total_card = game->players->data[enemy_id]->hands->size;
    i8 me_choose;  //=func(game,me_id,total_card) View layer Todo: call function
    game->players->data[me_id]->hands->push(
        game->players->data[me_id]->hands,
        game->players->data[enemy_id]->hands->data[me_choose - 1]);
    game->players->data[enemy_id]->hands->remove(game->players->data[enemy_id]->hands,
                                                 me_choose - 1);
    return 0;
}
#endif  // __CORE_PLAYER_H
