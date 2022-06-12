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

## References

- [Bang! (card game) - Wikipedia](https://en.wikipedia.org/wiki/Bang!_(card_game))

We Love GitHub!
