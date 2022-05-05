#include "core/game.h"
#include "core/player.h"
#include "utils/all.h"

void log_game_start(GameEvent* event) { Console.cyan("Game %s start!", event->game->name); }
void log_player_changed(GameEvent* event) {
    Game* game = event->game;
    Console.cyan("It's %s's turn!", game->players->data[game->turn % game->players->size]->name);
}

i32 main() {
    setup_catcher();
    srand(1481);
    Game* game = new_game($(String.format("game-%d", rand() % 1481)));
    Event.on($(String.format("%s-start", game->name)), (Subscriber)log_game_start);
    Event.on($(String.format("%s-player-changed", game->name)), (Subscriber)log_player_changed);

    char* player_name = $(calloc(1024, sizeof(char)));
    printf("Enter player name: ");
    scanf("%[^\n]%*c", player_name);
    game->join(game, player_name, real_player);

    game->join(game, "Computer A", computer_player);
    game->join(game, "Computer B", computer_player);
    game->join(game, "Computer C", computer_player);

    VectorShuffle(game->players);

    game->start(game);
    while (game->finished == false) {
        game->next(game);
    }
    $free();

    Console.green("Everything is done.");
    return EXIT_SUCCESS;
}
