<template>
	<div class="row">
		<button :class="{ 'button-primary': light_state }" @click="onClickLumos">
			{{ light_state ? "NOX" : "LUMOS" }}
		</button>
	</div>
</template>

<script>
export default {
	name: 'LightState',
	components: {

	},
	props: ["light_state"],
	emits: ["lumos-click"],
	data() {
		return {
			text: "LUMOS"
		}
	},
	methods: {
		onClickLumos() {
			this.$emit('lumos-click', !this.light_state)
		}
	}
}
</script>
  
<style scoped lang="scss">

div {
	
	
	display: flex;
	justify-content: center;
	align-items: center;
	height: 20vh;
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

	margin-top: 5rem;
	margin-bottom: 4rem;

	width: 100%;
	height: 100px;
	line-height: 100px;
	font-size: 5rem;
}
</style>