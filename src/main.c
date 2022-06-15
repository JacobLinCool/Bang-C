#include "core/game.h"
#include "core/player.h"
#include "utils/all.h"

// void log_game_start(GameEvent* event) { Console.cyan("Game %s start!", event->game->name); }
// void log_player_changed(GameEvent* event) {
//     Game* game = event->game;
//     Console.cyan("It's %s's turn!", game->players->data[game->turn % game->players->size]->name);
// }

void main_win(Game* game);

i32 main() {
    // setup_catcher();
    // srand(1481);210909
    // printf("ok\n");
    // time_t seed = time(NULL);
    time_t seed = 1655282672;
    printf("seed: %ld\n", seed);
    fflush(stdout);
    srand(seed);
    Game* game = new_game();

    char* player_name = $(calloc(1024, sizeof(char)));
    // printf("Enter player name: ");
    // scanf("%[^\n]%*c", player_name);
    //  game->join(game, player_name, false);

    game->join(game, "Computer A", true);
    game->join(game, "Computer B", true);
    game->join(game, "Computer C", true);
    game->join(game, "Computer D", true);
    game->join(game, "Computer E", true);

    VectorShuffle(game->players);

    game->start(game);
    while (game->finished == false) {
        game->next(game);
        for (int i = 0; i < game->players->size; i++) {
            if (!game->players->data[i]->dead && game->players->data[i]->hp <= 0)
                died_player(game, i, i);
        }
    }
    DEBUG_PRINT("Game End!\n");

    // Todo: who is win, GUI(show all roles.)
    main_win(game);
    $free();

    Console.green("Everything is done.");
    DEBUG_PRINT("seed: %ld\n", seed);
    return EXIT_SUCCESS;
}

void main_win(Game* game) {
    i32 live_player[5] = {0};
    for (int i = 0; i < game->players->size; i++) {
        if (game->players->data[i]->hp > 0) {
            live_player[game->players->data[i]->role->type]++;
        }
    }
    if (live_player[Sheriff] == 0) {
        if (live_player[Criminal] == 0) {
            Console.green("Traitor win!");
        } else {
            Console.green("Criminal win!");
        }
    } else {
        Console.green("Sheriff and Deputy win!");
    }
}
