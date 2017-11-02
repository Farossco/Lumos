package fr.iclario.bedcontrol;

import android.widget.SeekBar;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

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

		MainActivity.mainActivity.power[i] = progress;
		// Instantiate the RequestQueue.
		RequestQueue queue;
		queue = Volley.newRequestQueue(MainActivity.mainActivity);

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestBedControl(true, String.valueOf(MainActivity.mainActivity.power[i]), i, "POW");

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}
}