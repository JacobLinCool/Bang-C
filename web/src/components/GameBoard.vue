<script setup lang="ts">
import { game, ws, send, name } from "../composables/game";
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
    [{ left: "37.5%", top: "0%" }],
    [
        { left: "15%", top: "0%" },
        { right: "15%", top: "0%" },
    ],
    [
        { left: "0%", top: "30%" },
        { left: "37.5%", top: "0%" },
        { right: "0%", top: "30%" },
    ],
    [
        { left: "0%", top: "40%" },
        { left: "15%", top: "0%" },
        { right: "15%", top: "0%" },
        { right: "0%", top: "40%" },
    ],
    [
        { left: "0%", top: "50%" },
        { left: "0%", top: "10%" },
        { left: "37.5%", top: "0%" },
        { right: "0%", top: "10%" },
        { right: "0%", top: "50%" },
    ],
    [
        { left: "0%", top: "60%" },
        { left: "0%", top: "20%" },
        { left: "25%", top: "0%" },
        { right: "25%", top: "0%" },
        { right: "0%", top: "20%" },
        { right: "0%", top: "60%" },
    ],
];
</script>

<template>
    <div class="w-full h-full" v-if="game.players">
        <Field
            class="absolute bottom-0 w-2/5 left-3/10 bg-blue-300/20"
            :player="game.players[my_idx]"
            :self="true"
        />

        <Field
            v-for="(player, i) in others"
            :class="['absolute bg-blue-300/20 w-1/4 transform scale-90']"
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
    </div>
</template>

<style scoped></style>
