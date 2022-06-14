<script setup lang="ts">
    import { computed, reactive, ref } from "vue";

    type MessageType = "ask" | "chat";

    const ws = new WebSocket("ws://localhost:8080/");

    const name = ref("");
    const message = ref("");
    const messages: { from: string; to: string; message: string }[] = reactive([]);
    const players: string[] = reactive([]);

    const targets = computed(() => ["SERVER", ...players]);
    const target = ref("SERVER");

    ws.onmessage = (event) => {
        const message: { type: MessageType; payload: any } = JSON.parse(event.data);

        if (message.type === "chat") {
            messages.push(message.payload);
        } else if (message.type === "ask") {
            const answer = prompt(message.payload.question);
            name.value = answer || "";
            ws.send(JSON.stringify({ action: "name", name: answer }));
        } else if (message.type === "players") {
            players.splice(0, players.length, ...message.payload.players);
        } else if (message.type === "join") {
            if (!players.includes(message.payload.name)) {
                players.push(message.payload.name);
            }
        }
    };

    function send_message() {
        ws.send(JSON.stringify({ action: "chat", to: target.value, message: message.value }));
        message.value = "";
    }

    function color(n: string) {
        if (n === "SERVER") {
            return "deepskyblue";
        } else if (n === name.value) {
            return "darkorange";
        } else {
            return "darkgreen";
        }
    }
</script>

<template>
    <div id="wrap">
        <h1>Message</h1>
        <div>
            <span v-for="player in players" :key="player" class="mx-1" :style="{ color: color(player) }">{{ player }}</span>
        </div>
        <div>
            <div v-for="message in messages" class="my-4 grid grid-cols-2">
                <div class="text-right pr-2 border-r-1 border-gray">
                    <span :style="{ color: color(message.from) }">{{ message.from }}</span> >
                    <span :style="{ color: color(message.to) }">{{ message.to }}</span>
                </div>
                <div class="text-left pl-2">
                    <span>{{ message.message }}</span>
                </div>
            </div>
        </div>
        <div>
            <select v-model="target">
                <option v-for="target in targets" :key="target" :value="target">{{ target }}</option>
            </select>
            <input v-model="message" />
            <button @click="send_message">Send</button>
        </div>
    </div>
</template>

<style>
    #app {
        font-family: Avenir, Helvetica, Arial, sans-serif;
        -webkit-font-smoothing: antialiased;
        -moz-osx-font-smoothing: grayscale;
        text-align: center;
        color: #2c3e50;
        margin-top: 60px;
    }

    #wrap {
        display: flex;
        flex-direction: column;
        align-items: center;
    }
</style>
