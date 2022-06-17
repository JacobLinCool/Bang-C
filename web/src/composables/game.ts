import { reactive, ref } from "vue";
import { WSClient } from "./websocket";

export const ws = WSClient();

export const state = ref(-1);
export const waiting = ref(false);
export const game: any = reactive({});
export const players: string[] = reactive([]);
export const logs: { type: "chat" | "error"; message: string }[] = reactive([]);

ws.addEventListener("open", () => {
    state.value = 0;
});

ws.addEventListener("message", (event) => {
    const message: { type: string; payload: any } = JSON.parse(event.data);

    console.log(message);
    waiting.value = false;

    switch (message.type) {
        case "chat":
            logs.push({ type: "chat", message: message.payload.message });
            break;

        case "error":
            logs.push({ type: "error", message: message.payload.message });
            break;

        case "players":
            players.splice(0, players.length, ...message.payload.players);
            break;

        case "start":
            game.value = message.payload.game;
            state.value = 1;
            break;

        default:
            break;
    }
});

export function send(action: string, payload: any) {
    if (action !== "keep_alive") {
        waiting.value = true;
    }
    ws.send(JSON.stringify({ action, payload }));
}

// @ts-ignore For testing purposes
window.send = send;
// @ts-ignore For testing purposes
window.game = game;
