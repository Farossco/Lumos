<template>
	<div class="panel one-half column">
		<div class="row">
			<h2>Color</h2>
		</div>
		<div v-for="colorLine in light_colors">
			<div class="row">
				<button v-for="color in rgbRowToMinimal(colorLine)" v-bind:class="rowToStyle(rgbRowToMinimal(colorLine))"
					:class="{ 'selected': (light_rgb == color) }" v-bind:style="{ backgroundColor: decimalToColor(color) }"
					@click="onClickLightRgb(color)">
				</button>
			</div>
		</div>
	</div>
</template>

<script>
export default {
	name: 'LightRgb',
	components: {
	},
	props: ["light_rgb"],
	emits: ["light_rgb-click"],
	data() {
		return {
			light_colors:
				[[16777215, 16777215, 16777215, 16777215, 16777215, 16777215],
				[16711680, 16733440, 16755200, 16776960, 11206400, 5635840],
				[65280, 65365, 65450, 65535, 43775, 22015],
				[255, 5570815, 11141375, 16711935, 16711850, 16711765]]
		}
	},
	methods: {
		onClickLightRgb(color) {
			this.$emit('light_rgb-click', color)
		},
		decimalToColor(decimalValue) {
			const hexValue = decimalValue.toString(16);
			return '#' + hexValue.padStart(6, '0');
		},
		rgbRowToMinimal(row) {
			let actualRow = [];
			let previousItem = -1;

			for (let item of row) {
				if (item != previousItem) {
					actualRow.push(item);
					previousItem = item;
				}
			}
			return actualRow;
		},
		rowToStyle(row) {
			const numbers = ["zero", "one", "two", "three", "four", "five", "six"]
			let length = row.length

			return numbers[length];
		}
	},

}
</script>

<style scoped lang="scss">
.panel {
	text-align: center;
}

button {
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