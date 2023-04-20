<template>
	<light-power v-if="displayLightPower()" :light_power="light_power" @light_power-click="onEventLightPower" />
	<light-speed v-if="displayLightSpeed()" :light_speed="light_speed" @light_speed-click="onEventLightSpeed" />
	<light-rgb v-if="displayLightRgb()" :light_rgb="light_rgb" @light_rgb-click="onEventLightRgb" />
</template>

<script>
import LightRgb from "../../components/LightActions/LightRgb.vue"
import LightPower from "../../components/LightActions/LightPower.vue"
import LightSpeed from "../../components/LightActions/LightSpeed.vue"

export default {
	name: 'AppLightActions',
	components: {
		LightRgb,
		LightPower,
		LightSpeed,
	},
	props: ["light_mode", "light_rgb", "light_speed", "light_power"],
	emits: ["light_rgb-click", "light_speed-click", "light_power-click"],
	data() {
		return {
			light_rgb_display_modes: [0, 2, 3, 5, 6],
			light_power_display_modes: [0, 1, 2, 3, 4, 5, 6, 7],
			light_speed_display_modes: [1, 2, 3, 4],
		}
	},
	methods: {
		onEventLightRgb(color) {
			this.$emit('light_rgb-click', color)
		},
		onEventLightPower(power) {
			this.$emit('light_power-click', power)
		},
		onEventLightSpeed(speed) {
			this.$emit('light_speed-click', speed)
		},
		displayLightRgb() {
			return this.light_rgb_display_modes.includes(this.light_mode);
		},
		displayLightPower() {
			return this.light_power_display_modes.includes(this.light_mode);
		},
		displayLightSpeed() {
			return this.light_speed_display_modes.includes(this.light_mode);
		},
	},
}
</script>
