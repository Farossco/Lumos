<template>
  <v-container fluid>
    <v-row dense>
      <v-col>
        <app-light-state :light_state="light_state" @lumos-click="setLightState" />
      </v-col>
    </v-row>
    <v-row dense>
      <v-col cols="12" md="6">
        <app-light-mode :light_mode="light_mode" @light_mode-click="setLightMode" />
      </v-col>
      <v-col cols="12" md="6">
          <app-light-actions :light_rgb="light_rgb" :light_mode="light_mode" :light_speed="light_speed"
            :light_power="light_power" @light_rgb-click="setLightRgb" @light_speed-click="setLightSpeed"
            @light_power-click="setLightPower" />
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import axios from 'axios'
import AppLightState from "@/pages/Light/State.vue"
import AppLightMode from "@/pages/Light/Mode.vue"
import AppLightActions from "@/pages/Light/Actions.vue"

export default {
  name: 'AppLight',
  components: {
    AppLightState,
    AppLightMode,
    AppLightActions
  },
  created() {
    this.sendCommand("RQD", 0, 0)
  },
  data() {
    return {
      light_state: false,
      light_mode: 0,
      light_rgb: 16777215,
      light_speed: 0,
      light_power: 0
    }
  },
  methods: {
    async sendCommand(type, complement, value) {
      const url_base = ''
      const url = url_base + '/command?type=' + type + '&comp=' + Number(complement) + '&value=' + Number(value);
      try {
        const response = await axios.get(url);
        if (response.data.Status == 'OK') {
          this.light_state = response.data.Light.On;
          this.light_mode = response.data.Light.Mode;
          this.light_rgb = response.data.Light.Rgb[this.light_mode];
          this.light_speed = response.data.Light.Speed[this.light_mode];
          this.light_power = response.data.Light.Power[this.light_mode];
        } else {
          console.error(response.data);
        }
      } catch (error) {
        console.error(error);
      }
    },
    setLightState(state) {
      // this.light_state = state;
      this.sendCommand("LON", 0, state)
    },
    setLightMode(mode) {
      //this.light_mode = mode;
      this.sendCommand("LMO", 0, mode)
    },
    setLightRgb(rgb) {
      //this.light_rgb = rgb;
      this.sendCommand("RGB", this.light_mode, rgb)
    },
    setLightSpeed(speed) {
      //this.light_speed = speed;
      this.sendCommand("SPE", this.light_mode, speed)
    },
    setLightPower(power) {
      //this.light_speed = speed;
      this.sendCommand("POW", this.light_mode, power)
    }
  }
}
</script>

<style scoped lang="scss"></style>