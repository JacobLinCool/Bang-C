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
        <div class="absolute top-0 right-0 w-3/5 flex">
            <Card v-if="props.player.weapon"></Card>
            <Card v-if="props.player.scope"></Card>
            <Card v-if="props.player.barrel"></Card>
            <Card v-if="props.player.mustang"></Card>
            <Card v-if="props.player.jail"></Card>
            <Card v-if="props.player.dynamite"></Card>
        </div>
        <div class="absolute bottom-0 right-0 w-5/7 flex p-1">
            <ListTransition>
                <Card
                    v-for="card in props.player.hands"
                    :key="card.x"
                    :type="card.type"
                    :suit="Math.floor(card.priority / 100)"
                    :number="card.priority % 100"
                    class="-mx-6"
                    @click="select_card(card.x)"
                />
            </ListTransition>
        </div>
        <Role class="w-1/5 p-1" :type="props.player.role"></Role>
        <Character class="w-1/5 p-1" :type="props.player.character"></Character>
    </div>
</template>
