import { ref } from "vue";
import en from "./en";
import zh from "./zh";

type Lang = "en" | "zh";

const langs: Record<Lang, Record<string, string>> = { en, zh };

export const lang = ref<Lang>("en");

export function t(key: string) {
    return langs[lang.value][key];
}
