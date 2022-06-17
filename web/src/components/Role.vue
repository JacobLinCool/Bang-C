<script setup lang="ts">
import { RoleType } from "../types";
import { t } from "../i18n";
const props = defineProps({
    type: { type: Number, default: 0 },
});
</script>

<template>
    <div :title="t(RoleType[props.type])" class="hover:z-10">
        <img
            class="w-full"
            style="aspect-ratio: 9 / 16"
            :src="props.type ? `/role/${RoleType[props.type]}.png` : `/card/Card_Back.png`"
        />
    </div>
</template>
