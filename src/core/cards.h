#ifndef __CORE_CARD_H
#define __CORE_CARD_H

#include "../utils/all.h"
#include "ai.h"

void died_player(Game* game, i32 me_id, i32 enemy_id) {
    respond_all(game, "status");
    Player* me = game->players->data[me_id];
    Player* enemy = game->players->data[enemy_id];
    if (enemy->hp > 0) return;
    while (1) {
        respond_all(game, "status");
        ai_request_setting(AI_SPECIFY, Beer);
        Card* card = enemy->request(game, enemy_id);
        respond_all(game, "status");
        if (card == NULL) break;
        if (card->type == Beer) {
            game->discard->push(game->discard, card);
            respond_all(game, "status");
            respond_all_chat($(String.format("%s: I use Beer avoid died!", enemy->name)));
            return;
        } else {
            respond_error(find_client_by_id(enemy_id), "You only can use Beer avoid died");
            game->players->data[enemy_id]->hands->push(game->players->data[enemy_id]->hands, card);
            respond_all(game, "status");
        }
    }
    respond_all(game, "status");

    if (enemy->character->type == Sid_Ketchum) {
        DEBUG_PRINT("enemy is Sid_Ketchum\n");
        if (game->players->data[enemy_id]->hands->size >= 2) {
            respond_chat(find_client_by_id(enemy_id),
                         "Your're close to died, you can select two cards to heal youself");
            DEBUG_PRINT("enemy has 2 or more cards\n");
            while (1) {
                ai_request_setting(AI_DISCARD, 0);
                Card* card = game->players->data[enemy_id]->request(game, enemy_id);
                respond_all(game, "status");
                if (card == NULL) {
                    break;
                }
                game->discard->push(game->discard, card);
                respond_all(game, "status");
                break;
            }
            DEBUG_PRINT("has discarded one card\n");
            while (1) {
                ai_request_setting(AI_DISCARD, 0);
                Card* card = game->players->data[enemy_id]->request(game, enemy_id);
                respond_all(game, "status");
                if (card != NULL) {
                    game->discard->push(game->discard, card);
                    respond_all(game, "status");
                    break;
                }
            }
            game->players->data[enemy_id]->hp++;
            respond_all(game, "status");
            respond_all_chat($(String.format(
                "%s: Use Sid Ketchum's skill! I discard two cards to heal myeself!", enemy->name)));
            return;
        }
    }
    enemy->dead = true;
    respond_all(game, "status");
    respond_all_chat($(String.format("%s kill %s", game->players->data[me_id]->name, enemy->name)));
    printf("Died player (%s) role is %s, killed by %s (%s)\n", enemy->name,
           role_name[enemy->role->type], me->name, role_name[me->role->type]);
    respond_all_chat(
        $(String.format("%s died, his role is %s", enemy->name, role_name[enemy->role->type])));

    // END OF THE GAME detection
    // find Criminal, Traitor.

    if (alive(game, Sheriff) == false) {
        // Sheriff died
        game->finished = true;
        respond_all(game, "status");
        DEBUG_PRINT("game finished.\n");
        respond_all_chat("Sheriff died, game end!");
    } else if (alive(game, Criminal) == false && alive(game, Traitor) == false) {
        // Criminal and Traitor died
        game->finished = true;
        respond_all(game, "status");
        respond_all_chat("All Criminal and Traitor died, game end!");
        DEBUG_PRINT("game finished.\n");
    }

    if (game->finished) return;
    DEBUG_PRINT("game not finished.\n");
    // discard all cards
    Cards* discard_card = game->discard;
    if (enemy->character->type != Vulture_Sam) {
        for (int i = 0; i < game->players->size; i++) {
            if (game->players->data[i]->character->type == Vulture_Sam) {
                respond_all_chat($(
                    String.format("%s: Use Vulture Sam's skill! I can get cards from deid people!",
                                  game->players->data[i]->name)));
                discard_card = game->players->data[i]->hands;
                break;
            }
        }
    }
    transfer(enemy->hands, discard_card);
    respond_all(game, "status");
    if (NULL != enemy->weapon) {
        discard_card->push(discard_card, enemy->weapon);
        enemy->weapon = NULL;
    }
    if (NULL != enemy->barrel) {
        discard_card->push(discard_card, enemy->barrel);
        enemy->barrel = NULL;
    }
    if (NULL != enemy->mustang) {
        discard_card->push(discard_card, enemy->mustang);
        enemy->mustang = NULL;
    }
    if (NULL != enemy->scope) {
        discard_card->push(discard_card, enemy->scope);
        enemy->scope = NULL;
    }
    if (NULL != enemy->jail) {
        discard_card->push(discard_card, enemy->jail);
        enemy->jail = NULL;
    }
    if (NULL != enemy->dynamite) {
        discard_card->push(discard_card, enemy->dynamite);
        enemy->dynamite = NULL;
    }
    respond_all(game, "status");

    // Penalties and Rewards
    DEBUG_PRINT("Penalties and Rewards.\n");
    if (me_id == enemy_id) {
        DEBUG_PRINT("I killed myself.\n");
        respond_all_chat($(String.format("%s: I killed myself...", me->name)));
        return;
    }
    if (enemy->role->type == Deputy && me->role->type == Sheriff) {
        // Sheriff discards all cards
        DEBUG_PRINT("Penalty: discard all cards\n");
        respond_all_chat(
            $(String.format("Sherif killed Deputy! He lose his all cards!", me->name)));
        discard_card = game->discard;
        transfer(me->hands, discard_card);
        respond_all(game, "status");
        if (NULL != me->weapon) {
            discard_card->push(discard_card, me->weapon);
            me->weapon = NULL;
        }
        if (NULL != me->barrel) {
            discard_card->push(discard_card, me->barrel);
            me->barrel = NULL;
        }
        if (NULL != me->mustang) {
            discard_card->push(discard_card, me->mustang);
            me->mustang = NULL;
        }
        if (NULL != me->scope) {
            discard_card->push(discard_card, me->scope);
            me->scope = NULL;
        }
        if (NULL != me->jail) {
            discard_card->push(discard_card, me->jail);
            me->jail = NULL;
        }
        if (NULL != me->dynamite) {
            discard_card->push(discard_card, me->dynamite);
            me->dynamite = NULL;
        }
    } else if (enemy->role->type == Criminal) {
        DEBUG_PRINT("Reward: draw 3 cards\n");
        respond_all_chat(
            $(String.format("%s kill Criminal, he get three cards for reward", me->name)));
        player_draw_deck(game, me_id, 3);
    }
    respond_all(game, "status");
    return;
}

// If no me_id, me_id = player_id
void attack_player(Game* game, i32 me_id, i32 enemy_id) {
    respond_all(game, "status");
    DEBUG_PRINT("%d attack %d\n", me_id, enemy_id);
    if (get_player_hp(game, enemy_id) <= 0) return;  // avoid dynamaite_judge error

    // decrease player's hp
    game->players->data[enemy_id]->hp--;
    respond_all_chat(
        $(String.format("%s attack %s", me_id != -1 ? game->players->data[me_id]->name : "Dynamite",
                        game->players->data[enemy_id]->name)));
    respond_all(game, "status");

    // use character ablity(if valid)
    CharacterType enemy_type = game->players->data[enemy_id]->character->type;
    if (enemy_type == Bart_Cassidy || enemy_type == El_Gringo) {
        if (enemy_type == Bart_Cassidy) {
            respond_all_chat($(
                String.format("%s: Use Bart Cassidy's skill! I can get one card when I'm attacked",
                              game->players->data[enemy_id]->name)));
            Card* card = get_deck_top(game);
            game->players->data[enemy_id]->hands->push(game->players->data[enemy_id]->hands, card);
        }
        if (enemy_type == El_Gringo && me_id != -1) {
            respond_all_chat($(
                String.format("%s: Use El Gringo's skill! I can get one card from attacking people",
                              game->players->data[enemy_id]->name)));
            draw_from_player(game, enemy_id, me_id);
        }
    }
    respond_all(game, "status");

    // determine AI hate value
    ai_hate_change(game, me_id, enemy_id, 1);
    // dead
    // died_player(game, me_id, enemy_id);
    respond_all(game, "status");
    return;
}

void bang_no_distance(Game* game, i32 me_id, i32 enemy_id) {
    respond_all(game, "status");
    respond_error(find_client_by_id(enemy_id),
                  $(String.format("You're attacked by %s!", game->players->data[me_id]->name)));
    // Barrel
    i32     missed_total = 0;
    Player* enemy = game->players->data[enemy_id];
    if (enemy->barrel != NULL) {
        respond_all_chat($(String.format("%s: The barrel judge work!", enemy->name)));
        if (judge(game, enemy_id, 201, 213, Barrel)) missed_total++;
    }
    if (missed_total >= 1 + (game->players->data[me_id]->character->type == Slab_the_Killer))
        return;
    if (enemy->character->type == Jourdonnais) {
        respond_all_chat($(String.format(
            "%s: Use Jourdonnais's skill! I have another chance to barrel judge!", enemy->name)));
        if (judge(game, enemy_id, 201, 213, Barrel)) missed_total++;
    }
    if (missed_total >= 1 + (game->players->data[me_id]->character->type == Slab_the_Killer))
        return;

    respond_all(game, "status");
    bool  pass = false;
    Card* card[2] = {NULL};
    i32   card_amount = 0;
    respond_chat(find_client_by_id(enemy->id), "You can use missed to avoid attack");

    while (1) {
        respond_error(
            find_client_by_id(enemy->id),
            $(String.format("You still need %d missed to avoid attack",
                            1 + (game->players->data[me_id]->character->type == Slab_the_Killer) -
                                missed_total)));
        ai_request_setting(AI_SPECIFY, Missed);  // ai no use bang
        card[card_amount] = enemy->request(game, enemy_id);
        respond_all(game, "status");
        // DEBUG_PRINT("Give: %s\n",
        //             card[card_amount] == NULL ? "NULL" : card_name[card[card_amount]->type]);
        if (card[card_amount] == NULL) break;
        if (card[card_amount]->type == Missed) {
            respond_all_chat(
                $(String.format("%s use missed to avoid one attack point", enemy->name)));
            // card_amount++;
            missed_total++;
        }
        if (card[card_amount]->type == Bang && enemy->character->type == Calamity_Janet) {
            respond_all_chat($(String.format(
                "%s: Use Calamity Janet's skill! My Bang can be used as Missed!", enemy->name)));
            // card_amount++;
            missed_total++;
        }
        if (missed_total >= 1 + (game->players->data[me_id]->character->type == Slab_the_Killer)) {
            pass = true;
            break;
        }
        // return wrong card
        if (enemy->character->type == Calamity_Janet) {
            if (card[card_amount]->type != Bang && card[card_amount]->type != Missed) {
                respond_error(find_client_by_id(enemy_id), "You can't use this card");
                enemy->hands->push(enemy->hands, card[card_amount]);
            }
        } else if (card[card_amount]->type != Missed) {
            respond_error(find_client_by_id(enemy_id), "You can't use this card");
            enemy->hands->push(enemy->hands, card[card_amount]);
        }
        respond_all(game, "status");
    }
    respond_all(game, "status");

    if (pass) {
        if (card[0]) game->discard->push(game->discard, card[0]);
        if (card[1]) game->discard->push(game->discard, card[1]);
        respond_all(game, "status");
        return;
    }
    respond_all_chat(
        $(String.format("%s avoid %s attack ", enemy->name, game->players->data[me_id]->name)));
    attack_player(game, me_id, enemy_id);
    respond_all(game, "status");
    return;
}

void dynamite_judge(Game* game, i32 me_id) {
    respond_all(game, "status");
    Player* me = game->players->data[me_id];
    bool    dynamite_judge_result = judge(game, me_id, 102, 109, Dynamite);
    respond_all_chat($(String.format("The dynamite_judge result is ... %s",
                                     dynamite_judge_result ? "EXPLOSION!!!" : "SAFTY!!!")));
    if (dynamite_judge_result) {
        game->discard->push(game->discard, me->dynamite);
        me->dynamite = NULL;
        attack_player(game, me_id, me_id);
        attack_player(game, me_id, me_id);
        attack_player(game, me_id, me_id);
    } else {
        i32 left_player_id = (me_id + 1) % game->players->size;
        while (game->players->data[left_player_id]->hp <= 0) {
            left_player_id = (left_player_id + 1) % game->players->size;
        }
        game->players->data[left_player_id]->dynamite = me->dynamite;
        me->dynamite = NULL;
    }
    respond_all(game, "status");
}

bool jail_judge(Game* game, i32 me_id) {
    respond_all(game, "status");
    game->discard->push(game->discard, game->players->data[me_id]->jail);
    game->players->data[me_id]->jail = NULL;
    bool jail_judge_result = judge(game, me_id, 201, 213, Jail);
    respond_all_chat($(String.format("The jail_judge result is ... %s",
                                     jail_judge_result ? "SUCCESS!!!" : "FAIL!!!")));
    respond_all_chat($(String.format("%s skip this round", game->players->data[me_id]->name)));
    respond_all(game, "status");
    return jail_judge_result ? SUCCESS : FAIL;
}

bool bang(Game* game, i32 me_id) {
    respond_all(game, "status");
    i32 enemy_id = game->players->data[me_id]->choose_enemy(game, me_id);
    if (enemy_id < 0) return FAIL;

    // calculate distance between me and enemy
    i32 enemy_distance = distance(game, me_id, enemy_id);
    // my weapon distance
    i32 weapon_distance = 1;
    if (game->players->data[me_id]->weapon != NULL)
        weapon_distance += game->players->data[me_id]->weapon->type - Volcanic;
    // whether my weapon can reach the enemy

    if (weapon_distance < enemy_distance) {
        DEBUG_PRINT("Your Weapon Distance: %d\n", weapon_distance);
        DEBUG_PRINT("Enemy Distance: %d\n", enemy_distance);
        respond_error(find_client_by_id(me_id), "This player is too far");
        return FAIL;
    }

    respond_all_chat(
        $(String.format("%s: I use Bang to attack %s!", game->players->data[me_id]->name,
                        game->players->data[enemy_id]->name)));
    respond_all(game, "show bang");
    bang_no_distance(game, me_id, enemy_id);
    respond_all(game, "status");
    return SUCCESS;
}

bool missed(Game* game, i32 me_id) {
    return game->players->data[me_id]->character->type == Calamity_Janet;
}

bool gatling(Game* game, i32 me_id) {
    respond_all(game, "status");
    respond_all_chat($(String.format("%s: I use Gatling!", game->players->data[me_id]->name)));
    respond_all(game, "show gatling");
    for (int i = 0; i < game->players->size; i++) {
        if (get_player_hp(game, i) <= 0 || me_id == i) continue;
        respond_error(find_client_by_id(i), "You need to use a Missed to avoid Gatling card");
        bang_no_distance(game, me_id, i);
    }
    respond_all(game, "status");
    return SUCCESS;
}

bool indians(Game* game, i32 me_id) {
    respond_all(game, "status");
    respond_all_chat($(String.format("%s: I use Indian!", game->players->data[me_id]->name)));
    respond_all(game, "show indian");

    for (int i = 0; i < game->players->size; i++) {
        if (get_player_hp(game, i) <= 0 || me_id == i) continue;
        while (1) {
            respond_error(find_client_by_id(i), "You need to use a Bang to avoid Indians card");
            ai_request_setting(AI_SPECIFY, Bang);
            Card* card = game->players->data[i]->request(game, i);
            respond_all(game, "status");
            if (card == NULL) {
                attack_player(game, me_id, i);
                respond_all(game, "status");
                break;
            }
            if (card->type == Bang) {
                game->discard->push(game->discard, card);
                respond_all(game, "status");
                break;
            } else if (card->type == Missed &&
                       game->players->data[i]->character->type == Calamity_Janet) {
                game->discard->push(game->discard, card);
                respond_all(game, "status");
                break;
            } else {
                game->players->data[i]->hands->push(game->players->data[i]->hands, card);
                respond_all(game, "status");
            }
        }
    }
    respond_all(game, "status");
    return SUCCESS;
}

bool panic(Game* game, i32 me_id) {
    respond_all(game, "status");
    i32 enemy_id = game->players->data[me_id]->choose_enemy(game, me_id);
    if (enemy_id < 0) return FAIL;

    // calculate distance between me and enemy
    if (distance(game, me_id, enemy_id) > 1) return FAIL;

    respond_all_chat($(String.format("%s: I use Panic to %s!", game->players->data[me_id]->name,
                                     game->players->data[enemy_id]->name)));
    respond_error(find_client_by_id(enemy_id),
                  $(String.format("You're used Panic by %s", game->players->data[me_id]->name)));
    respond_all(game, "show panic");

    draw_from_player(game, me_id, enemy_id);
    respond_all(game, "status");
    return SUCCESS;
}

bool cat_balou(Game* game, i32 me_id) {
    respond_all(game, "status");
    Player* player = game->players->data[me_id];
    i32     enemy_id = player->choose_enemy(game, player->id);

    if (enemy_id < 0 || game->players->get(game->players, enemy_id)->hands->size == 0) return FAIL;

    respond_all_chat($(String.format("%s: I use Cat_balou to %s!", game->players->data[me_id]->name,
                                     game->players->data[enemy_id]->name)));
    respond_error(find_client_by_id(enemy_id), $(String.format("You're used Cat_balou by %s",
                                                               game->players->data[me_id]->name)));
    respond_all(game, "show cat_balou");

    discard_from_enemy(game, me_id, enemy_id);
    respond_all(game, "status");
    return SUCCESS;
}

bool stagecoach(Game* game, i32 me_id) {
    respond_all(game, "status");

    respond_all_chat($(String.format("%s: I use Stagecoach!", game->players->data[me_id]->name)));
    respond_all(game, "show stagecoach");

    player_draw_deck(game, me_id, 2);
    respond_all(game, "status");
    return SUCCESS;
}

bool wells_fargo(Game* game, i32 me_id) {
    respond_all(game, "status");
    respond_all_chat($(String.format("%s: I use Wells_fargo!", game->players->data[me_id]->name)));
    respond_all(game, "show wells_fargo");

    player_draw_deck(game, me_id, 3);
    respond_all(game, "status");
    return SUCCESS;
}

bool beer(Game* game, i32 me_id) {
    respond_all(game, "status");
    if (game->players->size <= 2) return FAIL;
    if (game->players->data[me_id]->hp == game->players->data[me_id]->character->health +
                                              (game->players->data[me_id]->role->type == Sheriff))
        return FAIL;
    game->players->data[me_id]->hp++;

    respond_all(game, "status");

    respond_all_chat($(String.format("%s: I use Beer!", game->players->data[me_id]->name)));
    respond_all(game, "show Beer");

    respond_all(game, "status");
    return SUCCESS;
}

bool saloon(Game* game, i32 me_id) {
    respond_all(game, "status");

    respond_all_chat($(String.format("%s: I use Saloon!", game->players->data[me_id]->name)));
    respond_all(game, "show saloon");

    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp <= 0) continue;
        recover(game, i);
        ai_saloon(game, i, me_id);
    }
    respond_all(game, "status");
    return SUCCESS;
}

bool duel(Game* game, i32 me_id) {
    respond_all(game, "status");
    i32 enemy_id = game->players->data[me_id]->choose_enemy(game, me_id);
    if (enemy_id < 0) return FAIL;

    respond_all_chat($(String.format("%s: I use Duel to %s!", game->players->data[me_id]->name,
                                     game->players->data[enemy_id]->name)));
    respond_error(find_client_by_id(enemy_id),
                  $(String.format("You're used Duel by %s", game->players->data[me_id]->name)));
    respond_all(game, "show duel");

    // duel
    bool duel_finish = false;
    while (1) {
        respond_all(game, "status");
        while (1) {
            respond_error(find_client_by_id(enemy_id),
                          "You need to use Bang to avoid attack from duel!");
            ai_request_setting(AI_SPECIFY, Bang);
            Card* card = game->players->data[enemy_id]->request(game, enemy_id);
            respond_all(game, "status");
            if (card == NULL) {
                respond_all_chat(
                    $(String.format("%s loses the duel", game->players->data[enemy_id]->name)));
                attack_player(game, me_id, enemy_id);
                duel_finish = true;
                break;
            }
            if (card->type == Bang) {
                respond_all_chat($(
                    String.format("%s use a bang for duel", game->players->data[enemy_id]->name)));
                game->discard->push(game->discard, card);
                break;
            } else if (card->type == Missed &&
                       game->players->data[enemy_id]->character->type == Calamity_Janet) {
                respond_all_chat($(
                    String.format("%s: Use Calamity Janet's skill! My Bang can be used as Missed!",
                                  game->players->data[enemy_id]->name)));
                game->discard->push(game->discard, card);
                break;
            } else {
                respond_error(find_client_by_id(enemy_id), "You can't use this card");
                game->players->data[enemy_id]->hands->push(game->players->data[enemy_id]->hands,
                                                           card);
            }
            respond_all(game, "status");
        }
        respond_all(game, "status");
        if (duel_finish) break;
        while (1) {
            respond_all(game, "status");
            respond_error(find_client_by_id(enemy_id),
                          "You need to use Bang to avoid attack from duel!");
            ai_request_setting(AI_SPECIFY, Bang);
            Card* card = game->players->data[me_id]->request(game, me_id);
            respond_all(game, "status");
            if (card == NULL) {
                respond_all_chat(
                    $(String.format("%s loses the duel", game->players->data[me_id]->name)));
                attack_player(game, enemy_id, me_id);
                duel_finish = true;
                break;
            }
            if (card->type == Bang) {
                respond_all_chat(
                    $(String.format("%s use a bang for duel", game->players->data[me_id]->name)));
                game->discard->push(game->discard, card);
                break;
            } else if (card->type == Missed &&
                       game->players->data[me_id]->character->type == Calamity_Janet) {
                respond_all_chat($(
                    String.format("%s: Use Calamity Janet's skill! My Bang can be used as Missed!",
                                  game->players->data[me_id]->name)));
                game->discard->push(game->discard, card);
                break;
            } else {
                respond_error(find_client_by_id(me_id), "You can't use this card");
                game->players->data[me_id]->hands->push(game->players->data[me_id]->hands, card);
            }
            respond_all(game, "status");
        }
        if (duel_finish) break;
    }
    respond_all(game, "status");
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

bool jail(Game* game, i32 me_id) { return SUCCESS; }

bool dynamite(Game* game, i32 me_id) {
    if (game->players->data[me_id]->dynamite != NULL) return FAIL;
    return SUCCESS;
}

bool volcanic(Game* game, i32 me_id) {
    Card* cur_card = game->players->data[me_id]->weapon;
    if (cur_card == NULL) return SUCCESS;
    game->discard->push(game->discard, cur_card);
    respond_all(game, "status");
    return SUCCESS;
}

bool schofield(Game* game, i32 me_id) {
    Card* cur_card = game->players->data[me_id]->weapon;
    if (cur_card == NULL) return SUCCESS;
    game->discard->push(game->discard, cur_card);
    respond_all(game, "status");
    return SUCCESS;
}

bool remington(Game* game, i32 me_id) {
    Card* cur_card = game->players->data[me_id]->weapon;
    if (cur_card == NULL) return SUCCESS;
    game->discard->push(game->discard, cur_card);
    respond_all(game, "status");
    return SUCCESS;
}

bool rev_carabine(Game* game, i32 me_id) {
    Card* cur_card = game->players->data[me_id]->weapon;
    if (cur_card == NULL) return SUCCESS;
    game->discard->push(game->discard, cur_card);
    respond_all(game, "status");
    return SUCCESS;
}

bool winchester(Game* game, i32 me_id) {
    Card* cur_card = game->players->data[me_id]->weapon;
    if (cur_card == NULL) return SUCCESS;
    game->discard->push(game->discard, cur_card);
    respond_all(game, "status");
    return SUCCESS;
}

bool general_store(Game* game, i32 me_id) {
    Cards* set = create_Cards();
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->get(game->players, i)->hp <= 0) continue;
        set->push(set, get_deck_top(game));
    }
    respond_all(game, "status");

    for (int i = 0; i < game->players->size; i++) {
        int id = (me_id + i) % game->players->size;
        Player* player = game->players->get(game->players, id);
        ai_request_setting(AI_FORCE_PLAY, 0);
        respond_all(game, "status");
        if (player->hp > 0) {
            while (player->select(game, id, set) == false)
                ;
        }
        respond_all(game, "status");
    }
    set->free(set);
    return SUCCESS;
}

bool is_weapon(Card* card) {
    if (card->type == Volcanic || card->type == Schofield || card->type == Remington ||
        card->type == Rev_Carabine || card->type == Remington || card->type == Barrel ||
        card->type == Winchester || card->type == Mustang || card->type == Scope ||
        card->type == Dynamite || card->type == Jail) {
        return true;
    }
    return false;
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
                {.type = Indians, .priority = 301, .use = indians},
                {.type = Indians, .priority = 313, .use = indians},
                {.type = Panic, .priority = 201, .use = panic},
                {.type = Panic, .priority = 211, .use = panic},
                {.type = Panic, .priority = 212, .use = panic},
                {.type = Panic, .priority = 308, .use = panic},
                {.type = Cat_Balou, .priority = 213, .use = cat_balou},
                {.type = Cat_Balou, .priority = 309, .use = cat_balou},
                {.type = Cat_Balou, .priority = 310, .use = cat_balou},
                {.type = Cat_Balou, .priority = 311, .use = cat_balou},
                {.type = Stagecoach, .priority = 109, .use = stagecoach},
                {.type = Stagecoach, .priority = 109, .use = stagecoach},
                {.type = Wells_Fargo, .priority = 203, .use = wells_fargo},
                {.type = General_Store, .priority = 112, .use = general_store},
                {.type = General_Store, .priority = 409, .use = general_store},
                {.type = Beer, .priority = 206, .use = beer},
                {.type = Beer, .priority = 207, .use = beer},
                {.type = Beer, .priority = 208, .use = beer},
                {.type = Beer, .priority = 209, .use = beer},
                {.type = Beer, .priority = 210, .use = beer},
                {.type = Beer, .priority = 211, .use = beer},
                {.type = Saloon, .priority = 205, .use = saloon},
                {.type = Duel, .priority = 111, .use = duel}, 
                {.type = Duel, .priority = 312, .use = duel},
                {.type = Duel, .priority = 408, .use = duel},
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
