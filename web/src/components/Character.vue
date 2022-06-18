<script setup lang="ts">
import { CharacterType } from "../types";
import { t } from "../i18n";
const props = defineProps({
    type: { type: Number, default: 0 },
});
</script>

<template>
    <div
        :title="t(CharacterType[props.type])"
        class="transition-all hover:z-10 transform hover:scale-180 hover:-translate-y-10"
    >
        <img
            class="w-full"
            style="aspect-ratio: 9 / 16"
            :src="`/character/${CharacterType[props.type]}.png`"
        />
    </div>
</template>
