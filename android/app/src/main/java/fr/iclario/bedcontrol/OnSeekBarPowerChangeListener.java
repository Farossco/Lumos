package fr.iclario.bedcontrol;

import android.widget.SeekBar;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;

public class OnSeekBarPowerChangeListener implements SeekBar.OnSeekBarChangeListener
{
	int i;

	OnSeekBarPowerChangeListener(int i)
	{
		this.i = i;
	}

	@Override
	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
	{
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar)
	{
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar)
	{
		int progress = seekBar.getProgress();

		MainActivity.power[i] = progress;
		// Instantiate the RequestQueue.
		RequestQueue queue = MainActivity.queue;

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestBedControl(true, String.valueOf(MainActivity.power[i]), i, "POW");

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}
}