import { reactive, ref } from "vue";
import { Card, Player } from "../types";
import { WSClient } from "./websocket";

export const ws = WSClient();

export const name = ref("");
export const state = ref(-1);
export const waiting = ref(false);
export const game: Partial<{
    turn: number;
    players: Player[];
    deck_size: number;
    discards: Card[];
}> = reactive({});
export const players: string[] = reactive([]);
export const logs: { type: "chat" | "error"; message: string }[] = reactive([]);
export const selecting: Card[] = reactive([]);
export const requesting = ref(false);
export const choosing = ref(false);
export const timer = ref(0);
let timer_id: number;

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
            Object.assign(game, message.payload);
            state.value = 1;
            break;

        case "status":
            Object.assign(game, message.payload.game);
            break;

        case "select_card":
            Object.assign(game, message.payload.game);
            selecting.splice(0, selecting.length, ...message.payload.cards.cards);
            timer_start();
            break;

        case "request_card":
            Object.assign(game, message.payload.game);
            requesting.value = true;
            timer_start();
            break;

        case "choose_enemy":
            Object.assign(game, message.payload.game);
            choosing.value = true;
            timer_start();
            break;

        case "take_card":
            Object.assign(game, message.payload.game);
            if (message.payload.target) {
                const player = message.payload.target;
                const cards = player.hands;
                player.weapon && cards.push(player.weapon);
                player.scope && cards.push(player.scope);
                player.barrel && cards.push(player.barrel);
                player.mustang && cards.push(player.mustang);
                player.jail && cards.push(player.jail);
                player.dynamite && cards.push(player.dynamite);
                selecting.splice(0, selecting.length, ...cards);
            }
            timer_start();
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

function timer_start() {
    timer.value = 60;
    timer_id && clearInterval(timer_id);
    timer_id = window.setInterval(timer_interval, 1000);
}

function timer_interval() {
    timer.value--;
    if (timer.value <= 0) {
        clearInterval(timer_id);
    }
}
