<template>
	<div class="panel one-half column">
		<div class="row">
			<h2>Speed</h2>
			<div>
				<input type="range" v-model="sliderValue" min="0" max="100" @mouseup="onClickLightSpeed" />
				<div>{{ sliderValue }}</div>
			</div>
		</div>
	</div>
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

<style scoped lang="scss">
.panel {
	text-align: center;
}

input {
	margin-left: 4%;

	&:first-child {
		margin-left: 0;
	}

	padding: 0 0 0 0;
	float: left;

	border-radius: 30px;
	height: 50px;
	line-height: 50px;
	margin-bottom: 2rem;

	width: 100%;
	font-size: 1.3rem;


	&:hover,
	&:focus {
		box-shadow: 1px 1px 5px grey;
	}

	&.selected {
		box-shadow: 5px 5px 8px grey;
	}

	&.one {
		width: 100%;
	}

	&.two {
		width: 48%;
	}

	&.three {
		width: 30.6666666667%;
	}

	&.four {
		width: 22%;
	}

	&.five {
		width: 16.8%;
	}

	&.six {
		width: 13.3333333333%;
	}

	&.seven {
		width: 10.8571428571%;
	}

	&.eight {
		width: 9%;
	}

	&.nine {
		width: 7.55555555556%;
	}

	&.ten {
		width: 6.4%;
	}
}
</style>