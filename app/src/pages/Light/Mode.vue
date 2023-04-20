<template>
	<v-card class="ma-3 rounded-shaped">
		<v-toolbar flat dense color="primary">
			<v-toolbar-title class="text-h5">
				Light modes
			</v-toolbar-title>
		</v-toolbar>
		<v-card-text>
			<v-col v-for="(mode, index) in light_mode_names" :key="index" cols="12" class="pb-2 text-center">
				<v-btn class="rounded-pill" width="85%" height="50px"
					:color="(light_mode == index) ? 'secondary' : 'transparent'" @click="onClickLightMode(index)">
					{{ mode }}
				</v-btn>
			</v-col>
		</v-card-text>
	</v-card>
</template>

<script>
export default {
	name: 'AppLightMode',
	components: {

	},
	props: ["light_mode"],
	emits: ["light_mode-click"],
	data() {
		return {
			light_mode_names: ["Continuous", "Flash", "Strobe", "Fade", "Smooth", "Dawn", "Sunset", "Start"],
		}
	},
	methods: {
		onClickLightMode(mode) {
			this.$emit('light_mode-click', mode)
		}
	}
}
</script>

<style scoped lang="scss"></style>