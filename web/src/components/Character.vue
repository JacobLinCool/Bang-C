<script setup lang="ts">
import { Card, CharacterType, RoleType } from "../types";
import { t } from "../i18n";
import { Ref, inject } from "vue";
const props = defineProps({
    type: { type: Number, default: 0 },
});

const spotlight: Ref<Card | CharacterType | RoleType> | undefined = inject("spotlight");
const spotlight_type: Ref<number> | undefined = inject("spotlight_type");
</script>

<template>
    <div
        :title="t(CharacterType[props.type])"
        class="transition-all hover:z-20 overflow-hidden rounded-lg group"
        style="aspect-ratio: 9 / 16"
        @contextmenu="
            (event) => {
                event.preventDefault();
                spotlight = props.type;
                spotlight_type = 1;
            }
        "
    >
        <img
            class="transition-all w-full transform group-hover:scale-105"
            style="aspect-ratio: 9 / 16"
            :src="`/character/${CharacterType[props.type]}.png`"
        />
    </div>
</template>
