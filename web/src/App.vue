<script setup lang="ts">
import { state, game, logs } from "./composables/game";
import Fade from "./components/Fade.vue";
import WaitingRoom from "./components/WaitingRoom.vue";
import { computed } from "vue";

const rev_logs = computed(() =>
    JSON.parse(JSON.stringify(logs))
        .map((l: { type: "chat" | "error"; message: string }, i: number) => ({ ...l, i }))
        .reverse(),
);
</script>

<template>
    <div class="w-full h-full overflow-hidden m-0 p-0 bg-amber-100 flex">
        <div class="w-4/5 h-full">
            <Fade>
                <WaitingRoom v-if="state === 0" />
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
</style>
