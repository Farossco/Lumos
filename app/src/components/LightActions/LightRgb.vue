<template>
	<v-card class="ma-3 rounded-shaped">
		<v-toolbar flat dense color="primary">
			<v-toolbar-title class="text-h5">
				Color
			</v-toolbar-title>
		</v-toolbar>
		<v-card-text>
			<v-col>
				<v-row v-for="(color_line, i) in light_colors" :key="i">
					<v-col v-for="(color, i) in rgbRowToMinimal(color_line)" :key="i" cols="72" class="pb-2 text-center">
						<v-btn class="rounded-pill" width="100%" height="50px" :elevation="color == light_rgb ? 8 : null"
							:color="decimalToColor(color)" @click="onClickLightRgb(color)">
						</v-btn>
					</v-col>
				</v-row>
			</v-col>
		</v-card-text>
	</v-card>
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
				[[16777215, 16777215, 16777215,],
				[16711680, 16733440, 16755200], [16776960, 11206400, 5635840],
				[65280, 65365, 65450], [65535, 43775, 22015],
				[255, 5570815, 11141375], [16711935, 16711850, 16711765]]
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
		decimalToColorTab(decimalTab) {
			let colorTab = [];

			for (let row in decimalTab) {
				for (let color in row) {
					colorTab.push(this.decimalToColor(color));
				}
			}

			return colorTab;
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

<style scoped lang="scss"></style>