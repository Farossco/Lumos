package fr.iclario.bedcontrol;

import android.widget.SeekBar;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;

public class OnSeekBarGreenChangeListener implements SeekBar.OnSeekBarChangeListener
{
	@Override
	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
	{
		MainActivity.green = progress;
		// Instantiate the RequestQueue.
		RequestQueue queue = MainActivity.queue;

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestRGB(MainActivity.getRgbString(), MainActivity.responseTextView);

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
		MainActivity.textViewGreenValue.setText(progress + "/" + MainActivity.seekBarGreen.getMax());
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar)
	{
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar)
	{
	}
}
