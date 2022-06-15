<script setup lang="ts">
    import { computed, reactive, ref } from "vue";
    import { action } from "./action";

    type MessageType = "ask" | "chat";

    const ws = new WebSocket(window.location.hostname === "localhost" ? "ws://localhost:8080/" : "wss://bang-ws.jacoblin.cool");

    const name = ref("");
    const message = ref("");
    const messages: { from: string; to: string; message: string }[] = reactive([]);
    const players: string[] = reactive([]);

    const targets = computed(() => ["SERVER", ...players]);
    const target = ref("SERVER");

    ws.onmessage = (event) => {
        const message: { type: MessageType; payload: any } = JSON.parse(event.data);

        console.log(message);

        switch (message.type) {
            case "chat":
                // ...
                break;

            default:
                break;
        }
    };

    // @ts-ignore
    window.send = function (action: string, payload: any) {
        ws.send(JSON.stringify({ action, payload }));
    };

    function send_message() {
        ws.send(JSON.stringify({ action: "chat", to: target.value, message: message.value }));
        message.value = "";
    }
</script>

<template>
    <div id="wrap"></div>
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
