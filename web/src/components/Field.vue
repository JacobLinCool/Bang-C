<script setup lang="ts">
import Card from "./Card.vue";
import Character from "./Character.vue";
import Role from "./Role.vue";
import ListTransition from "./ListTransition.vue";
import { Player } from "../types";

const props = defineProps<{ player: Partial<Player>; self: boolean }>();

function select_card(x: number) {
    console.log("select card", { x });
}
</script>

<template>
    <div class="p-2 rounded-md" style="aspect-ratio: 16 / 9">
        <div
            :class="['absolute w-2/3 flex p-1 h-2/3 justify-center', self ? 'bottom-0' : 'top-0']"
            :style="{ right: ((props.player.hands?.length || 0) - 3) * 4 + '%' }"
        >
            <ListTransition>
                <Card
                    v-for="card in props.player.hands"
                    :key="card.x"
                    :card="card"
                    :class="[
                        'transform hover:scale-110 hover:z-20 h-full',
                        self ? 'hover:-translate-y-5 z-10' : 'hover:translate-y-4',
                    ]"
                    :style="{ 'margin-right': ((props.player.hands?.length || 0) - 3) * -7 + '%' }"
                    @click="select_card(card.x)"
                />
            </ListTransition>
        </div>
        <div :class="['absolute right-0 w-2/3 h-2/3 flex', self ? 'top-0' : 'bottom-0']">
            <Card v-if="props.player.weapon" :card="props.player.weapon" class="h-full"></Card>
            <Card v-if="props.player.scope" :card="props.player.scope" class="h-full"></Card>
            <Card v-if="props.player.barrel" :card="props.player.barrel" class="h-full"></Card>
            <Card v-if="props.player.mustang" :card="props.player.mustang" class="h-full"></Card>
            <Card v-if="props.player.jail" :card="props.player.jail" class="h-full"></Card>
            <Card v-if="props.player.dynamite" :card="props.player.dynamite" class="h-full"></Card>
        </div>
        <Role class="w-1/5 p-1" :type="props.player.role"></Role>
        <Character :class="['w-1/5 p-1']" :type="props.player.character"></Character>

        <div class="text-xl text-red-500">
            <span v-for="i in props.player.hp">♥</span>
        </div>
    </div>
</template>
