<template>
  <v-container fluid>
    <v-row no-gutters class="d-flex justify-space-between mt-8 mb-6">
      <h1 class="page-title">Settings</h1>
    </v-row>
    <v-row>
      <v-col>
        <v-card>
          <v-card-title class="pa-6 pb-3">
            <p>Settings</p>
          </v-card-title>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
export default {
  name: 'AppSettings',
  components: {
  },
  created() {

  },
  data() {
    return {
    }
  },
  methods: {
  }
}
</script>

<style scoped lang="scss">
.page-title {
  font-size: 2.4rem;
  color: #b9b9b9;
  margin-bottom: 0;
}

h1 {
  font-size: 3rem;
  font-weight: 400;
  margin-bottom: 16px;
  line-height: 1;
}

.v-card-title p {
  color: "#6e6e6e";
  line-height: 1.33;
  margin: 0;
  font-size: 20px;
  font-weight: 400;
}
</style>