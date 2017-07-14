package fr.iclario.bedcontrol;

import android.view.View;
import android.widget.Switch;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

class OnSwitchOnOffClickListener implements View.OnClickListener
{
	@Override
	public void onClick(View v)
	{
		MainActivity.mainActivity.on = ((Switch) v).isChecked();

		// Instantiate the RequestQueue.
		RequestQueue queue;
		queue = Volley.newRequestQueue(MainActivity.mainActivity);

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequestBedControl(false, String.valueOf(MainActivity.mainActivity.on ? 1 : 0), -1, "ONF");

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}
}
