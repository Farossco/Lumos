package fr.iclario.bedcontrol.listener;

import android.widget.SeekBar;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import fr.iclario.bedcontrol.activity.ColorPickerActivity;
import fr.iclario.bedcontrol.utils.StringRequestBedControl;

public class OnSeekBarSpeedChangeListener implements SeekBar.OnSeekBarChangeListener
{
	int i;

	public OnSeekBarSpeedChangeListener(int i)
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

		ColorPickerActivity.colorPickerActivity.speed[i] = progress;

		// Instantiate the RequestQueue.
		RequestQueue queue;
		queue = Volley.newRequestQueue(ColorPickerActivity.colorPickerActivity);

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestBedControl(true, String.valueOf(ColorPickerActivity.colorPickerActivity.speed[i]), i, "SPE");

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}
}
