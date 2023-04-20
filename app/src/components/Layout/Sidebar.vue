<template>
  <v-navigation-drawer v-model="drawer" :rail="test" permanent @click="test = false">
    <v-list>
      <template v-for="(item, i) in items" :key="i">
        <v-row v-if="item.heading" align="center">
          <v-col cols="6" class="py-5">
            <span style="padding-left: 32px" class="text-body-1 subheader"
              :class="(item.heading && test) ? 'show ' : 'hide'">
              {{ item.heading }}
            </span>
          </v-col>
        </v-row>

        <v-divider v-else-if="item.divider" dark class="my-4"></v-divider>

        <v-list-group v-else-if="item.children" v-model="item.model" append-icon="" color="primary">
          <template v-slot:activator="{ props }">
            <v-list-item v-bind="props" :prepend-icon="item.icon" :title="item.title"></v-list-item>
          </template>

          <v-list-item v-for="(child, i) in item.children" :key="i" :to="child.link" :title="child.title"
            :prepend-icon="child.icon" :value="child.title">
          </v-list-item>
        </v-list-group>

        <v-list-item v-else :to="item.link === '#' ? null : item.link" link :prepend-icon="item.icon" :value="item.title"
          :title="item.title">
        </v-list-item>
      </template>
    </v-list>

  </v-navigation-drawer>
</template>

<script>
import { mapStores } from 'pinia';
import { useDrawerStore } from "@/store/drawer.js"

export default {
  name: 'AppSideBar',
  components: {
  },
  props: {
    source: String,
  },
  data() {
    return {
      items: [
        { title: 'Home', icon: 'mdi-home', link: '/home' },
        { divider: true },
        { title: 'Settings', icon: 'mdi-cog', link: '/settings' },
      ],
      sidebarWidth: 240,
      sidebarMinWidth: 55,
      drawer: true
    }
  },
  computed: {
    ...mapStores(useDrawerStore),
    test: {
      get() {
        return this.drawerStore.isOpen;
      },
      set(value) {
        this.drawerStore.isOpen = value;
      }
    },
  },
  methods: {
    toggleDrawer() {
      this.drawerStore.isOpen = !this.drawerStore.isOpen;
    },
    getDrawerState() {
      return this.drawerStore.isOpen;
    }
  }

}
</script>

<style lang="scss"/>
