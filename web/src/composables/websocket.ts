export function WSClient(verbose = true) {
    const server =
        window.location.hostname === "localhost"
            ? "ws://localhost:8080/"
            : "wss://bang-ws.jacoblin.cool";

    const ws = new WebSocket(server);

    ws.addEventListener("open", () => {
        verbose && console.log(`[WS] Connected to ${server}`);
    });

    ws.addEventListener("close", (event) => {
        verbose && console.log(`[WS] Disconnected from ${server}, reason: ${event.reason}`);
    });

    ws.addEventListener("message", (event) => {
        verbose && console.log(`[WS] Received message: ${event.data}`);
    });

    ws.addEventListener("error", (event) => {
        verbose && console.log(`[WS] Client Error: ${event}`);
    });

    return ws;
}
