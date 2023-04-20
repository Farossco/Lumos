<template>
  <v-app-bar class="main-header" color='primary'>
    <v-btn :icon="getDrawerState() ? 'mdi-menu' : 'mdi-arrow-left'" @click="toggleDrawer" />
    <v-toolbar-title>Lumos</v-toolbar-title>
    <v-spacer />
    <v-btn icon="mdi-dots-vertical"/>
  </v-app-bar>
</template>

<script>
import { mapStores } from 'pinia';
import { useDrawerStore } from "@/store/drawer.js"

export default {
  name: 'AppHeader',
  components: {

  },
  data: () => ({
    searchCollapse: true,
    notifications: [
      { text: 'Check out this awesome ticket', icon: 'mdi-tag', color: 'warning' },
      { text: 'What is the best way to get ...', icon: 'mdi-thumb-up', color: 'success' },
      { text: 'This is just a simple notification', icon: 'mdi-flag', color: 'error' },
      { text: '12 new orders has arrived today', icon: 'mdi-cart', color: 'primary' },
    ],
    messages: [
      { text: 'JH', name: 'Jane Hew', message: 'Hey! How is it going?', time: '09:32', color: 'warning' },
      { text: 'LB', name: 'Lloyd Brown', message: 'Check out my new Dashboard', time: '10:02', color: 'success' },
      { text: 'MW', name: 'Mark Winstein', message: 'I want rearrange the appointment', time: '12:16', color: 'error' },
      { text: 'LD', name: 'Liana Dutti', message: 'Good news from sale department', time: '14:56', color: 'primary' },
    ],
    account: [
      { text: 'Profile', icon: 'mdi-account', color: 'textColor' },
      { text: 'Tasks', icon: 'mdi-thumb-up', color: 'textColor' },
      { text: 'Messages', icon: 'mdi-flag', color: 'textColor' }
    ],
    notificationsBadge: true,
    messageBadge: true
  }),
  computed: {
    ...mapStores(useDrawerStore)
  },
  methods: {
    toggleDrawer() {
      this.drawerStore.isOpen = !this.drawerStore.isOpen;
    },
    getDrawerState() {
      return this.drawerStore.isOpen;
    },
    logOut: function () {
      window.localStorage.setItem('authenticated', false);
      this.$router.push('/login');
    }
  }
};
</script>

<style lang="scss"></style>
