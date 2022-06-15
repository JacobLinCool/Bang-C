export interface Card {
    pointer: number;
    id: number;
    priority: number;
}

export interface Player {
    name: string;
    id: number;
    bullet: number;
    hp: number;
    dead: boolean;
    role: number;
    character: number;
    weapon: Card;
    barrel: Card;
    mustang: Card;
    scope: Card;
    jail: Card;
    dynamite: Card;
    hands: Card[];
}
