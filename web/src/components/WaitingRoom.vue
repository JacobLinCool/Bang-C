<script setup lang="ts">
import { onBeforeUnmount, onMounted, ref } from "vue";
import { players, ws, send } from "../composables/game";
import ListTransition from "./ListTransition.vue";

const name = ref("");
const name_input = ref<HTMLInputElement | null>(null);

function set_name() {
    if (name.value.length > 0) {
        const checker = (event: MessageEvent) => {
            const data = JSON.parse(event.data);
            if (data.type === "error" && data.payload.message.toLowerCase().includes("name")) {
                if (name_input.value) {
                    name_input.value.disabled = false;
                }
                ws.removeEventListener("message", checker);
            }
        };
        ws.addEventListener("message", checker);

        send("name", { name: name.value });

        if (name_input.value) {
            name_input.value.disabled = true;
        }
    }
}

function add_computer() {
    send("add_computer", {});
}

function start() {
    send("start", {});
}

function kick(name: string) {
    if (confirm(`Kick ${name}?`)) {
        send("kick", { name });
    }
}

let keep_alive: number;

onMounted(() => {
    keep_alive = window.setInterval(() => {
        send("keep_alive", {});
    }, 20_000);
});
onBeforeUnmount(() => {
    window.clearInterval(keep_alive);
});
</script>

<template>
    <div class="w-full h-full flex flex-col justify-center items-center">
        <div class="m-4">
            <input
                ref="name_input"
                type="text"
                v-model="name"
                @keyup.enter="set_name"
                placeholder="Enter your name, then press enter. Name is unchangable."
                class="w-120 p-2 bg-white/50 border-2 border-amber-200 rounded-lg focus:outline-none text-center"
            />
        </div>
        <div class="w-full m-4 flex justify-center items-center">
            <ListTransition>
                <div
                    v-for="player in players"
                    :key="player"
                    @click="kick(player)"
                    class="m-4 p-4 min-h-20 bg-white/30 rounded-lg flex justify-center items-center border-1 border-amber-300 cursor-pointer"
                >
                    <span>{{ player }}</span>
                </div>
            </ListTransition>
        </div>
        <div v-if="players[0] === name && name_input?.disabled">
            <button
                @click="add_computer"
                class="w-48 p-2 m-4 bg-white/40 border-2 border-amber-200 rounded-lg focus:outline-none"
            >
                Add Computer
            </button>
            <button
                @click="start"
                class="w-48 p-2 m-4 bg-white/40 border-2 border-amber-200 rounded-lg focus:outline-none"
            >
                Start Game
            </button>
        </div>
    </div>
</template>
