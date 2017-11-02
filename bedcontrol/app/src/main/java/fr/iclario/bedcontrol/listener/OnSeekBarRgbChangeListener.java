package fr.iclario.bedcontrol.listener;

import android.widget.SeekBar;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import fr.iclario.bedcontrol.activity.ColorPickerActivity;
import fr.iclario.bedcontrol.utils.StringRequestBedControl;

public class OnSeekBarRgbChangeListener implements SeekBar.OnSeekBarChangeListener
{
	public static final int COLOR_RED = 0;
	public static final int COLOR_GREEN = 1;
	public static final int COLOR_BLUE = 2;

	private int color;

	public OnSeekBarRgbChangeListener(int color)
	{
		this.color = color;
	}

	@Override
	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
	{
		ColorPickerActivity.colorPickerActivity.textViewRgbValue[color].setText(progress + "/" + ColorPickerActivity.colorPickerActivity.seekBarRgb[color].getMax());
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar)
	{
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar)
	{
		int progress = seekBar.getProgress();

		ColorPickerActivity.colorPickerActivity.rgb[color] = progress;
		// Instantiate the RequestQueue.
		RequestQueue queue;
		queue = Volley.newRequestQueue(ColorPickerActivity.colorPickerActivity);

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestBedControl(true, (Integer.toHexString(ColorPickerActivity.colorPickerActivity.rgb[COLOR_RED] << 16 | ColorPickerActivity.colorPickerActivity.rgb[COLOR_GREEN] << 8 | ColorPickerActivity.colorPickerActivity.rgb[COLOR_BLUE])).toUpperCase(), 0, "RGB");

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
		ColorPickerActivity.colorPickerActivity.textViewRgbValue[color].setText(progress + "/" + ColorPickerActivity.colorPickerActivity.seekBarRgb[color].getMax());
	}
}
