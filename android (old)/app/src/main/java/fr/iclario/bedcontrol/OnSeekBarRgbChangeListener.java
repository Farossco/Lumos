package fr.iclario.bedcontrol;

import android.widget.SeekBar;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

public class OnSeekBarRgbChangeListener implements SeekBar.OnSeekBarChangeListener
{
	public static final int COLOR_RED = 0;
	public static final int COLOR_GREEN = 1;
	public static final int COLOR_BLUE = 2;

	private int color;

	OnSeekBarRgbChangeListener(int color)
	{
		this.color = color;
	}

	@Override
	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
	{
		MainActivity.mainActivity.textViewRgbValue[color].setText(progress + "/" + MainActivity.mainActivity.seekBarRgb[color].getMax());
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar)
	{
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar)
	{
		int progress = seekBar.getProgress();

		MainActivity.mainActivity.rgb[color] = progress;
		// Instantiate the RequestQueue.
		RequestQueue queue;
		queue = Volley.newRequestQueue(MainActivity.mainActivity);

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestBedControl(true, (Integer.toHexString(MainActivity.mainActivity.rgb[COLOR_RED] << 16 | MainActivity.mainActivity.rgb[COLOR_GREEN] << 8 | MainActivity.mainActivity.rgb[COLOR_BLUE])).toUpperCase(), 0, "RGB");

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
		MainActivity.mainActivity.textViewRgbValue[color].setText(progress + "/" + MainActivity.mainActivity.seekBarRgb[color].getMax());
	}
}
