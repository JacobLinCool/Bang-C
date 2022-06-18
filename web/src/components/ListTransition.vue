<template>
    <TransitionGroup
        enter-from-class="opacity-0"
        enter-to-class="opacity-100"
        leave-from-class="opacity-100"
        leave-to-class="opacity-0"
        move-class="transition-all duration-350 ease"
        enter-active-class="transition-all duration-350 ease"
        leave-active-class="transition-all duration-350 ease absolute"
    >
        <slot />
    </TransitionGroup>
</template>
