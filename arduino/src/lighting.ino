// Display the RGB value
void light ()
{
	rgb2color(); // Convert RGB value to Red, Green and Blue values

	analogWrite (PIN_LED_RED, on ? red[mode] : 0);
	analogWrite (PIN_LED_GREEN, on ? green[mode] : 0);
	analogWrite (PIN_LED_BLUE, on ? blue[mode] : 0);
}

// Convert RGB value to Red, Green and Blue values
void rgb2color ()
{
	// Lower is the power, lower is the color value - It allows you to control LEDs light power
	red[mode]   = ((rgb[mode] & 0xFF0000) >> 16) * (power[mode] / maxSpeed[MODE_DEFAULT]); // maxSpeed[MODE_DEFAULT] = Max Power
	green[mode] = ((rgb[mode] & 0x00FF00) >> 8) * (power[mode] / maxSpeed[MODE_DEFAULT]);
	blue[mode]  = (rgb[mode] & 0x0000FF) * (power[mode] / maxSpeed[MODE_DEFAULT]);
}
