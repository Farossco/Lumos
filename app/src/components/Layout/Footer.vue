<template>
  <v-footer class="bg-secondary text-center d-flex flex-column" absolute position="bottom">
    <div class="d-flex w-100 align-center px-4">
      <v-chip variant="text">
        Lumos web interface v0.1.0
      </v-chip>
      <v-spacer></v-spacer>
      <v-btn v-for="icon in icons" :key="icon" :href="icon.link" :icon="icon.icon" variant="plain">
      </v-btn>
    </div>
  </v-footer>
</template>

<script>
export default {
  name: 'AppFooter',

  data: () => ({
    icons: [
      { icon: 'mdi-github', link: 'https://github.com/Farossco/Lumos' },
    ]
  }),
}
</script>

<style lang="scss">
.v-footer {
  bottom: 0;
  left: 0;
}
</style>

