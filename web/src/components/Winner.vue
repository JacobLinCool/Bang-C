<script setup lang="ts">
import { computed } from "vue";
import { name, game, winner } from "../composables/game";
import { RoleType } from "../types";

const title = computed(() => {
    if (winner.value === RoleType.Sheriff) {
        return "Sheriff's party wins!";
    } else if (winner.value === RoleType.Criminal) {
        return "Criminal's party wins!";
    } else if (winner.value === RoleType.Traitor) {
        return "Traitor wins!";
    } else {
        return "";
    }
});

const winners = computed(() => {
    if (game.players) {
        if (winner.value === RoleType.Sheriff) {
            return game.players.filter(
                (p) => p.role === RoleType.Sheriff || p.role === RoleType.Deputy,
            );
        } else {
            return game.players.filter((p) => p.role === winner.value);
        }
    } else {
        return [];
    }
});
</script>

<template>
    <div class="w-full h-full flex flex-col justify-center items-center">
        <div class="w-full m-4 flex justify-center items-center text-4xl font-bold">
            {{ title }}
        </div>
        <div class="w-full m-4 flex justify-center items-center text-2xl font-bold">
            🎉🎉 {{ winners.map((p) => p.name).join(", ") }} 🎉🎉
        </div>
    </div>
</template>
