<script setup lang="ts">
import { CardType, Card, CharacterType, RoleType } from "../types";
import { t } from "../i18n";
import { inject, Ref } from "vue";
const props = defineProps<{
    card: Card;
}>();

const suits = ["", "♠", "♥", "♦", "♣"];

const spotlight: Ref<Card | CharacterType | RoleType> | undefined = inject("spotlight");
const spotlight_type: Ref<number> | undefined = inject("spotlight_type");
</script>

<template>
    <div
        :title="t(CardType[props.card.type])"
        class="transition-all duration-100 ease-in-out cursor-pointer overflow-hidden rounded-lg group"
        style="aspect-ratio: 9 / 16"
        @contextmenu="
            (event) => {
                event.preventDefault();
                spotlight = props.card;
                spotlight_type = 0;
            }
        "
    >
        <img
            class="transition-all h-full transform group-hover:scale-105"
            style="aspect-ratio: 9 / 16"
            :src="`/card/${CardType[props.card.type]}.png`"
        />
        <div v-if="props.card.priority % 100" class="absolute font-bold bottom-[5%] left-[12%]">
            {{ props.card.priority % 100 }}{{ suits[Math.floor(props.card.priority / 100)] }}
        </div>
    </div>
</template>
