// Display the RGB value
void light ()
{
	rgb2color(); // Convert RGB value to Red, Green and Blue values

	analogWrite (PIN_LED_RED, on ? red : 0);
	analogWrite (PIN_LED_GREEN, on ? green : 0);
	analogWrite (PIN_LED_BLUE, on ? blue : 0);
}

// Convert RGB value to Red, Green and Blue values
void rgb2color ()
{
	// Lower is the power, lower is the color value - It allows you to control LEDs light power
	red   = ((rgb & 0xFF0000) >> 16) * (power / MAX_POWER);
	green = ((rgb & 0x00FF00) >> 8) * (power / MAX_POWER);
	blue  = (rgb & 0x0000FF) * (power / MAX_POWER);
}
