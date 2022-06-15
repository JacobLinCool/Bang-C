export type ActionType = "name" | "kick" | "add_computer" | "start_game" | "use_card" | "respond";

export type ActionPayload = {
    name: { name: string };
    kick: { name: string };
    add_computer: {};
    start_game: {};
    use_card: { card: number };
    respond: { card: number };
};

export function action<T extends ActionType>(type: T, payload: ActionPayload[T]): string {
    return JSON.stringify({ action: type, payload: payload });
}

export type EventType = "start" | "draw" | "respond";
