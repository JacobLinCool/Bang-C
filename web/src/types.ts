export enum RoleType {
    ROLE_LOW_BOUND,
    Sheriff,
    Deputy,
    Criminal,
    Traitor,
    ROLE_HIGH_BOUND,
}

export enum CharacterType {
    CHARACTER_LOW_BOUND,
    Bart_Cassidy,
    Black_Jack,
    Calamity_Janet,
    El_Gringo,
    Jesse_Jones,
    Jourdonnais,
    Kit_Carlson,
    Lucky_Duke,
    Paul_Regret,
    Pedro_Ramirez,
    Rose_Doolan,
    Sid_Ketchum,
    Slab_the_Killer,
    Suzy_Lafayette,
    Vulture_Sam,
    Willy_the_Kid,
    CHARACTER_HIGH_BOUND,
}

export enum CardType {
    Card_Back,
    Bang,
    Missed,
    Gatling,
    Indians,
    Panic,
    Cat_Balou,
    Stagecoach,
    Wells_Fargo,
    General_Store,
    Beer,
    Saloon,
    Duel,
    Barrel,
    Scope,
    Mustang,
    Jail,
    Dynamite,
    Volcanic,
    Schofield,
    Remington,
    Rev_Carabine,
    Winchester,
    CARD_HIGH_BOUND,
}

export interface Card {
    /** Offset of address  */
    x: number;
    /** Card type, enum (number) */
    type: CardType;
    /** SUIT * 100 + VALUE */
    priority: number;
}

export interface Player {
    name: string;
    id: number;
    hp: number;
    role: RoleType;
    character: CharacterType;
    weapon: Card | null;
    barrel: Card | null;
    mustang: Card | null;
    scope: Card | null;
    jail: Card | null;
    dynamite: Card | null;
    hands: Card[];
}
