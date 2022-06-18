<script setup lang="ts">
import { state, logs, ws, waiting } from "./composables/game";
import Fade from "./components/Fade.vue";
import WaitingRoom from "./components/WaitingRoom.vue";
import GameBoard from "./components/GameBoard.vue";
import { computed, provide, ref } from "vue";
import { Card as C, CharacterType, RoleType } from "./types";
import Card from "./components/Card.vue";
import Character from "./components/Character.vue";
import Role from "./components/Role.vue";

const rev_logs = computed(() =>
    JSON.parse(JSON.stringify(logs))
        .map((l: { type: "chat" | "error"; message: string }, i: number) => ({ ...l, i }))
        .reverse(),
);

ws.addEventListener("close", (event) => {
    if (state.value >= 0) {
        alert(
            "Connection closed. Reason: " +
                (event.reason || "No reason, it just doesn't like you."),
        );
        window.location.reload();
    }
});

const spotlight = ref<C | CharacterType | RoleType | null>(null);
const spotlight_type = ref(0);

provide("spotlight", spotlight);
provide("spotlight_type", spotlight_type);
</script>

<template>
    <div class="w-full h-full overflow-hidden m-0 p-0 bg-amber-100 flex">
        <div class="w-4/5 h-full">
            <Fade>
                <WaitingRoom v-if="state === 0" />
                <GameBoard v-if="state === 1" />
            </Fade>
        </div>
        <div class="w-1/5 h-full bg-white/50 flex flex-col-reverse overflow-y-auto">
            <TransitionGroup name="logs">
                <div v-for="log of rev_logs" :key="log.i" class="p-2">
                    <span :class="[log.type === 'chat' ? 'text-gray-600' : 'text-red-600']">{{
                        log.message
                    }}</span>
                </div>
            </TransitionGroup>
        </div>
        <Fade enter-active-class="duration-100 ease-out" leave-active-class="duration-100 ease-in">
            <div
                class="w-full h-full overflow-hidden m-0 p-0 bg-black/10 fixed top-0 left-0"
                v-if="state < 0 || waiting"
            ></div>
        </Fade>
        <Fade>
            <div
                v-if="spotlight"
                class="fixed w-full h-full overflow-hidden m-0 p-0 bg-black/50 flex justify-center items-center z-30"
                @click="spotlight = null"
            >
                <Card
                    v-if="spotlight_type === 0 && typeof spotlight !== 'number'"
                    :card="spotlight"
                    class="h-4/5 rounded-2xl"
                />
                <Character
                    v-if="spotlight_type === 1 && typeof spotlight === 'number'"
                    :type="spotlight"
                    class="h-4/5 rounded-2xl"
                />
                <Role
                    v-if="spotlight_type === 2 && typeof spotlight === 'number'"
                    :type="spotlight"
                    class="h-4/5 rounded-2xl"
                />
            </div>
        </Fade>
    </div>
</template>

<style>
html,
body,
#app {
    font-family: Avenir, Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;

    @apply w-full h-full overflow-hidden m-0 p-0;
}

.logs-move, /* apply transition to moving elements */
.logs-enter-active,
.logs-leave-active {
    transition: all 0.5s ease;
}

.logs-enter-from,
.logs-leave-to {
    opacity: 0;
    transform: translateX(30px);
}

/* ensure leaving items are taken out of layout flow so that moving
   animations can be calculated correctly. */
.logs-leave-active {
    position: absolute;
}

* {
    position: relative;
}

div::-webkit-scrollbar {
    display: none;
}
</style>
