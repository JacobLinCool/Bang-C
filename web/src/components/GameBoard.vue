<script setup lang="ts">
import { game, ws, send, name } from "../composables/game";
import Field from "./Field.vue";
import { computed } from "vue";

const my_idx = computed(() =>
    game.players ? game.players.findIndex((p) => p.name === name.value) : -1,
);
</script>

<template>
    <div class="w-full h-full" v-if="game.players">
        <Field
            class="absolute bottom-0 w-1/2 left-1/4 bg-blue-300/20"
            :player="game.players[my_idx]"
            :self="true"
        />

        <Field
            v-if="game.players.length >= 2"
            class="absolute bg-blue-300/20 w-1/3"
            :player="game.players[(my_idx + 1) % game.players.length]"
            :self="false"
        />

        <Field
            v-if="game.players.length >= 3"
            class="absolute bg-blue-300/20 w-1/3"
            :player="game.players[(my_idx + 2) % game.players.length]"
            :self="false"
        />

        <Field
            v-if="game.players.length >= 4"
            class="absolute bg-blue-300/20 w-1/3"
            :player="game.players[(my_idx + 3) % game.players.length]"
            :self="false"
        />

        <Field
            v-if="game.players.length >= 5"
            class="absolute bg-blue-300/20 w-1/3"
            :player="game.players[(my_idx + 4) % game.players.length]"
            :self="false"
        />

        <Field
            v-if="game.players.length >= 6"
            class="absolute bg-blue-300/20 w-1/3"
            :player="game.players[(my_idx + 5) % game.players.length]"
            :self="false"
        />

        <Field
            v-if="game.players.length >= 7"
            class="absolute bg-blue-300/20 w-1/3"
            :player="game.players[(my_idx + 6) % game.players.length]"
            :self="false"
        />
    </div>
</template>

<style scoped></style>
