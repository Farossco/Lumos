<template>
    <v-app class="pa-0">
        <app-header />
        <app-sidebar />
        <v-main class="content">
            <router-view />
            <app-footer absolute position="bottom" />
        </v-main>
    </v-app>
</template>

<script>
import AppHeader from '@/components/Layout/Header.vue';
import AppSidebar from '@/components/Layout/Sidebar.vue';
import AppFooter from "@/components/Layout/Footer.vue";

export default {
    name: 'AppLayout',
    components: { AppFooter, AppHeader, AppSidebar },
};
</script>

<style lang="scss" >

</style>
