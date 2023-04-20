<template>
	<div class="d-flex justify-center align-center">
		<v-btn height="150px" width="85%" size="x-large" :color="light_state ? 'secondary' : 'transparent'"
			class="rounded-pill" elevation="4" @click="onClickLumos">
			{{ light_state ? "LUMOS" : "LUMOS" }}
		</v-btn>
	</div>
</template>

<script>
export default {
	name: 'AppLightState',
	components: {
	},
	props: ["light_state"],
	emits: ["lumos-click"],
	data() {
		return {
			text: "LUMOS",
			color: null
		}
	},
	methods: {
		onClickLumos() {
			this.$emit('lumos-click', !this.light_state)
		},
		onInput(hue) {
			this.color.hue = hue;
		},
	}
}
</script>
  
<style scoped lang="scss">
button {
	font-weight: 600;
	font-family: "Raleway", "HelveticaNeue", "Helvetica Neue", Helvetica, Arial, sans-serif;
	color: #555;

	margin-top: 2rem;
	margin-bottom: 4rem;

	font-size: 5rem;
}
</style>