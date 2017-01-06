package fr.iclario.bedcontrol;

import android.widget.CompoundButton;
import android.widget.Switch;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;

public class OnSwitchOnOffChangeListener implements Switch.OnCheckedChangeListener
{
	@Override
	public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
	{
		MainActivity.on = isChecked;
		// Instantiate the RequestQueue.
		RequestQueue queue = MainActivity.queue;

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestON(MainActivity.on ? "1" : "0", MainActivity.textView);

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}
}