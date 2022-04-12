#include "core/game.h"
#include "core/player.h"

void log_game_start(GameEvent* event) { printf("Game %s start!\n", event->game->name); }
void log_player_changed(GameEvent* event) {
    Game* game = event->game;
    printf("It's %s's turn!\n", game->players->data[game->turn % game->players->size]->name);
}

i32 main() {
    srand(1481);
    Game* game = new_game(String.fmt("game-%d", rand() % 1481));
    Event.on(String.fmt("%s-start", game->name), (Subscriber)log_game_start);
    Event.on(String.fmt("%s-player-changed", game->name), (Subscriber)log_player_changed);

    string player_name = calloc(1024, sizeof(char));
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

    printf(CLR_GREEN "Everything is done.\n" CLR_NONE);
    return EXIT_SUCCESS;
}
