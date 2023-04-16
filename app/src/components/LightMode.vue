<template>
	<div class="panel one-half column">
		<div class="row">
			<h2>Light modes</h2>
		</div>
		<div v-for="(mode, index) in light_mode_names">
			<div class="row">
				<button :class="{ 'button-primary': (light_mode == index) }" @click="onClickLightMode(index)">
					{{ mode }}
				</button>
			</div>
		</div>
	</div>
</template>

<script>
export default {
	name: 'LightMode',
	components: {

	},
	props: ["light_mode"],
	emits: ["light_mode-click"],
	data() {
		return {
			light_mode_names: ["Continuous", "Flash", "Strobe", "Fade", "Smooth", "Dawn", "Sunset", "Start"]
		}
	},
	methods: {
		onClickLightMode(mode) {
			this.$emit('light_mode-click', mode)
		}
	}
}
</script>

<style scoped lang="scss">
.panel {
	text-align: center;
}

button {
	font-size: 1.5em;
    line-height: 1.6;
    font-weight: 600;
    font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif;
    color: #555;

	padding: 0 0 0 0;
	float: left;

	border-radius: 30px;
	height: 50px;
	line-height: 50px;
	margin-bottom: 2rem;

	width: 100%;
	font-size: 1.3rem;
}
</style>