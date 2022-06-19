# Bang! C

Bang! the board game C implementation.

## Architecture

We need to implement a 3 layer architecture.

### Layer 1: Utils

The first layer is the utils layer.

It should be more generic than the other layers, which means that it should be able to migrate to other projects.

### Layer 2: Game Logic (Core)

The second layer is the game logic layer.

It should contains all the game logic and expose a simplified API to the third layer.

I think it should be a event driven architecture.

Game Init -> Game Start -> Game Loop -> Game End

### Layer 3: User Interface (View)

The third layer is the user interface layer.

It may be a simple terminal interface (TUI), or a graphical interface (GUI).

Also, the config file parser should be implemented here.

## Development

### Backend Game Server

1. Install third party libraries `sudo make setup`
2. Build server `make server`
3. Run server `./server`

### Frontend Game Client

Change directory to `web`

1. Install dev tools: Node.js 16+, pnpm
2. Install dependencies: `pnpm install`
3. Run dev server `pnpm dev`

## References

- [Bang! (card game) - Wikipedia](https://en.wikipedia.org/wiki/Bang!_(card_game))

## Source

### Cards

- [Card List - Bang!](https://bang.dvgiochi.com/cardslist.php?id=1#q_result)

We Love GitHub!
