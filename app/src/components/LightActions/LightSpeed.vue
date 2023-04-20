<template>
	<v-card class="ma-3 rounded-shaped">
		<v-toolbar flat dense color="primary">
			<v-toolbar-title class="text-h5">
				Speed
			</v-toolbar-title>
		</v-toolbar>
		<v-card-text>
			<v-slider v-model="sliderValue" color="primary" :min="0" :max="100" :step="1" @mouseup="onClickLightSpeed">
				<template v-slot:append>
					<v-text-field v-model="sliderValue" type="number" :min="0" :max="100" style="width: 80px"
						density="compact" hide-details variant="outlined"></v-text-field>
				</template>
			</v-slider>
		</v-card-text>
	</v-card>
</template>

<script>
export default {
	name: 'LightSpeed',
	components: {
	},
	props: ["light_speed"],
	emits: ["light_speed-click"],
	data() {
		return {
			sliderValue: this.light_speed
		}
	},
	watch: {
		light_speed(newValue) {
			this.sliderValue = newValue;
		}
	},
	methods: {
		onClickLightSpeed() {
			this.$emit('light_speed-click', this.sliderValue)
		}
	},
}
</script>

<style scoped lang="scss"></style>