import { reactive, ref } from "vue";
import { WSClient } from "./websocket";

export const ws = WSClient();

export function send(action: string, payload: any) {
    ws.send(JSON.stringify({ action, payload }));
}

export const state = ref(0);
export const game: any = reactive({});
export const players: string[] = reactive([]);
export const logs: { type: "chat" | "error"; message: string }[] = reactive([]);

ws.addEventListener("message", (event) => {
    const message: { type: string; payload: any } = JSON.parse(event.data);

    console.log(message);

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

        default:
            break;
    }
});

// @ts-ignore For testing purposes
window.send = send;
