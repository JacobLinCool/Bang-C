<script setup lang="ts">
import { CardType } from "../types";
import { t } from "../i18n";
const props = defineProps({
    type: { type: Number, default: 0 },
    suit: { type: Number, default: 0 },
    number: { type: Number, default: 0 },
});

const suits = ["", "♠", "♥", "♦", "♣"];
</script>

<template>
    <div
        :title="t(CardType[props.type])"
        class="p-1t ransition-all duration-100 ease-in-out transform hover:scale-110 opacity-75 hover:opacity-100"
    >
        <img
            class="w-full"
            style="aspect-ratio: 9 / 16"
            :src="`/card/${CardType[props.type]}.png`"
        />
        <div v-if="props.number" class="absolute bottom-[5%] left-[5%]">{{ props.number }}</div>
        <div v-if="props.suit" class="absolute bottom-[5%] left-[17%]">
            {{ suits[props.suit] }}
        </div>
    </div>
</template>
