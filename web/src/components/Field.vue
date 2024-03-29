<script setup lang="ts">
import { send, game, choosing, requesting } from "../composables/game";
import Card from "./Card.vue";
import Character from "./Character.vue";
import Role from "./Role.vue";
import ListTransition from "./ListTransition.vue";
import { Player, Card as C } from "../types";
import { computed } from "vue";

const props = defineProps<{ player: Partial<Player>; self: boolean }>();
const weapons = computed(
    () =>
        [
            props.player.weapon,
            props.player.scope,
            props.player.barrel,
            props.player.mustang,
            props.player.jail,
            props.player.dynamite,
        ].filter((x) => x && x.type) as C[],
);

function select_player() {
    if (choosing.value) {
        choosing.value = false;
        send("select_player", { player: props.player.id });
    }
}
</script>

<template>
    <div
        :class="[
            'transition-all p-2 rounded-lg',
            (game.turn || 0) % (game.players?.length || 10) === props.player.id
                ? 'bg-orange-300/20 border border-orange-300'
                : 'bg-blue-300/20',
            props.player?.dead ? 'filter brightness-50' : '',
            choosing ? 'border-2 border-orange-500' : '',
        ]"
        style="aspect-ratio: 16 / 9"
        @click="
            (event) => {
                if (choosing) {
                    event.stopPropagation();
                    select_player();
                }
            }
        "
    >
        <div
            :class="['absolute right-0 w-2/3 flex p-1 justify-center', self ? 'bottom-0' : 'top-0']"
        >
            <ListTransition>
                <Card
                    v-for="card in props.player.hands"
                    :key="card.x"
                    :card="card"
                    :class="[
                        'max-h-60 transform hover:scale-110 hover:z-20',
                        self ? 'hover:-translate-y-5 z-10' : 'hover:translate-y-4',
                        requesting && self ? 'border-2 border-indigo-500' : 'filter brightness-85',
                    ]"
                    :style="{
                        'margin-right':
                            ((props.player.hands?.length || 0) - 3 <= 6
                                ? (props.player.hands?.length || 0) - 3
                                : 6) *
                                -5.5 +
                            '%',
                        left: ((props.player.hands?.length || 0) - 3) * -3 + '%',
                    }"
                />
            </ListTransition>
        </div>
        <div
            :class="['absolute right-0 w-2/3 flex p-1 justify-start', self ? 'top-0' : 'bottom-0']"
        >
            <ListTransition>
                <Card
                    v-for="card in weapons"
                    :key="card.x"
                    :card="card"
                    class="hover:z-20 max-h-45"
                    :style="{
                        'margin-right':
                            ((weapons.length || 0) - 3 <= 6 ? (weapons.length || 0) - 3 : 6) *
                                -5.5 +
                            '%',
                        left: ((weapons.length || 0) - 3) * -3 + '%',
                    }"
                />
            </ListTransition>
        </div>
        <Role @click="select_player" :class="['w-1/5 p-1']" :type="props.player.role"></Role>
        <Character
            @click="select_player"
            :class="['w-1/5 p-1']"
            :type="props.player.character"
        ></Character>

        <div class="text-xl text-red-500">
            <span v-if="!props.player?.dead" class="text-black text-sm">
                {{ props.player.name }}
            </span>
            <del v-else class="text-black text-sm"> {{ props.player.name }} </del>
            <br />
            <span v-for="i in props.player.hp">♥</span>
        </div>
    </div>
</template>
