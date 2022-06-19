# Bang! C

Bang! the board game C implementation, including game manager, computer player, backend websocket server, and frontend web client.

[Docker image is avaliable on Docker Hub](https://hub.docker.com/repository/docker/jacoblincool/bang-dev)

## Architecture

We need to implement a 4-layer architecture.

### Layer 1: Utils

The first layer is the utils layer.

We have implemented some generic utilities, such as connection waiting room.

### Layer 2: Game Logic (Core)

The second layer is the game logic layer.

It contains all the game logic and expose a simplified API to the third layer (network layer).

### Layer 3: Network Interface (Transportation)

The third layer is the network interface layer.

We use websocket portocol to transfer data over TCP.

All the data were using JSON to deliver from / to the client.

### Layer 4: User Interface (View)

The forth layer is the user interface layer.

We designed a website as graphical interface (GUI).

## Multiple Threads

The main thread will spawn a server thread, and in each round of game, the server thread will spawn a game manager thread, which will be ended when a game is done.

- Main
  - Server
    - Game Manager 1
    - Game Manager 2
    - Game Manager 3
    - ...

And the main thread will stop the server when a round of game finished if user type `exit` in terminal during the cycle.

## Development

Install cmake (to build `libwebsocket`) and valgrind (to debug).

```sh
sudo apt update && sudo apt install -y cmake valgrind
```

Then, you need two terminal window to run the game server and the client.

### Backend Game Server

1. Install third party libraries `sudo make setup`
2. Build server `make server`
3. Run server `./server`

```sh
# Install third party libraries
sudo make setup

# Build server
make server
# or "make server-debug" to enable debug mode

# Run server
./server
# or "make valgrind" to run with valgrind
```

### Frontend Game Client

Change directory to `web`

1. Install dev tools: Node.js 16+, pnpm
2. Install dependencies: `pnpm install`
3. Run dev server `pnpm dev`

```sh
# Install PNPM
curl -fsSL https://get.pnpm.io/install.sh | sh -

# Install Node LTS
pnpm env use --global lts

# Change directory to web
cd web

# Install dependencies
pnpm install

# Run dev server
pnpm dev
```

After you run the both server and client, you should be able to play the game on `http://localhost:8080`.

## References

- [Bang! (card game) - Wikipedia](https://en.wikipedia.org/wiki/Bang!_(card_game))

## Source

### Cards

- [Card List - Bang!](https://bang.dvgiochi.com/cardslist.php?id=1#q_result)

We Love GitHub!
