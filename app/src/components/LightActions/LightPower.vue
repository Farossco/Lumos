<template>
	<v-card class="ma-3 rounded-shaped">
		<v-toolbar flat dense color="primary">
			<v-toolbar-title class="text-h5">
				Power
			</v-toolbar-title>
		</v-toolbar>
		<v-card-text>
			<v-slider v-model="sliderValue" color="primary" :min="0" :max="255" :step="1" @mouseup="onClickLightPower">
				<template v-slot:append>
					<v-text-field v-model="sliderValue" type="number" :min="0" :max="100" style="width: 80px" density="compact" hide-details
						variant="outlined"></v-text-field>
				</template>
			</v-slider>
		</v-card-text>
	</v-card>
</template>

<script>
export default {
	name: 'LightPower',
	components: {
	},
	props: ["light_power"],
	emits: ["light_power-click"],
	data() {
		return {
			sliderValue: this.light_power
		}
	},
	watch: {
		light_power(newValue) {
			this.sliderValue = newValue;
		}
	},
	methods: {
		onClickLightPower() {
			this.$emit('light_power-click', this.sliderValue)
		}
	},
}
</script>

<style scoped lang="scss"></style>