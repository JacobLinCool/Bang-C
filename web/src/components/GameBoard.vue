<script setup lang="ts">
import { game, send, name, selecting, requesting, choosing, timer } from "../composables/game";
import Field from "./Field.vue";
import { computed } from "vue";
import Card from "./Card.vue";

const my_idx = computed(() =>
    game.players ? game.players.findIndex((p) => p.name === name.value) : -1,
);

const others = computed(() => {
    if (!game.players) {
        return [];
    }

    const players = [];
    for (let i = 1; i < game.players.length; i++) {
        players.push(game.players[(my_idx.value + i) % game.players.length]);
    }

    return players;
});

const discard_top = computed(() =>
    game.discards ? game.discards[game.discards.length - 1] : null,
);

const translates = [
    [{ left: "37.5%", top: "0.25%", transformOrigin: "top left" }],
    [
        { left: "15%", top: "0.25%", transformOrigin: "top left" },
        { right: "15%", top: "0.25%", transformOrigin: "top right" },
    ],
    [
        { left: "0.25%", top: "30%", transformOrigin: "top left" },
        { left: "37.5%", top: "0.25%", transformOrigin: "top" },
        { right: "0.25%", top: "30%", transformOrigin: "top right" },
    ],
    [
        { left: "0.25%", top: "40%", transformOrigin: "top left" },
        { left: "15%", top: "0.25%", transformOrigin: "top" },
        { right: "15%", top: "0.25%", transformOrigin: "top" },
        { right: "0.25%", top: "40%", transformOrigin: "top right" },
    ],
    [
        { left: "0.25%", top: "50%", transformOrigin: "top left" },
        { left: "0.25%", top: "10%", transformOrigin: "top left" },
        { left: "37.5%", top: "0.25%", transformOrigin: "top" },
        { right: "0.25%", top: "10%", transformOrigin: "top right" },
        { right: "0.25%", top: "50%", transformOrigin: "top right" },
    ],
    [
        { left: "0.25%", top: "50%", transformOrigin: "top left" },
        { left: "0.25%", top: "15%", transformOrigin: "top left" },
        { left: "25%", top: "0.25%", transformOrigin: "top left" },
        { right: "25%", top: "0.25%", transformOrigin: "top right" },
        { right: "0.25%", top: "15%", transformOrigin: "top right" },
        { right: "0.25%", top: "50%", transformOrigin: "top right" },
    ],
];

function skip_select_card() {
    if (requesting.value) {
        requesting.value = false;
        send("select_card", { card: -1 });
    }
}
</script>

<template>
    <div class="w-full h-full" v-if="game.players">
        <Field
            class="absolute bottom-0 w-2/5 left-3/10 transform scale-90"
            :player="game.players[my_idx]"
            :self="true"
        />

        <Field
            v-for="(player, i) in others"
            :class="['absolute w-2/5 transform scale-60']"
            :style="translates[game.players.length - 2][i]"
            :player="player"
            :self="false"
        />

        <div
            class="absolute w-20 left-[35%] top-[32%] overflow-hidden rounded-lg"
            style="aspect-ratio: 9 / 16"
        >
            <img class="h-full" style="aspect-ratio: 9 / 16" src="/card/Card_Back.png" />
            <div
                class="absolute top-0 left-0 font-bold w-full h-full flex justify-center items-center text-white text-center text-3xl text-shadow-md"
            >
                {{ game.deck_size || "?" }}
            </div>
        </div>

        <Card
            class="absolute w-20 right-[35%] top-[32%]"
            v-if="discard_top"
            :card="discard_top"
        ></Card>

        <div
            v-if="requesting"
            class="absolute bottom-0 right-0 m-8 w-24 h-10 bg-indigo-600/80 rounded-md flex justify-center items-center cursor-pointer text-white hover:bg-indigo-700 transition-all"
            @click="skip_select_card"
        >
            <span>Skip</span>
        </div>

        <Fade>
            <div
                v-if="selecting || requesting || choosing"
                class="absolute bottom-0 left-0 m-8 w-24 h-10 bg-indigo-600/80 rounded-md flex justify-center items-center cursor-pointer text-white border border-indigo-700"
            >
                <span>{{ timer }}</span>
            </div>
        </Fade>
    </div>
</template>

<style scoped></style>
