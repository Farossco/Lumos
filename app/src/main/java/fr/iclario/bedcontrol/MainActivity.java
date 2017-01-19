package fr.iclario.bedcontrol;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

public class MainActivity extends AppCompatActivity
{
	public static int power;
	public static int red;
	public static int green;
	public static int blue;
	public static boolean on;

	public static SeekBar seekBarPower;
	public static SeekBar seekBarRed;
	public static SeekBar seekBarGreen;
	public static SeekBar seekBarBlue;

	public static TextView textViewPowerValue;
	public static TextView textViewRedValue;
	public static TextView textViewGreenValue;
	public static TextView textViewBlueValue;

	public static Switch switchOnOff;
	public static TextView responseTextView;

	public static RequestQueue queue;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		initializeVariables();

		// Initialize textViews with '0'.
		textViewPowerValue.setText(seekBarPower.getProgress() + "/" + seekBarPower.getMax());
		textViewRedValue.setText(seekBarRed.getProgress() + "/" + seekBarRed.getMax());
		textViewGreenValue.setText(seekBarGreen.getProgress() + "/" + seekBarGreen.getMax());
		textViewBlueValue.setText(seekBarBlue.getProgress() + "/" + seekBarBlue.getMax());

		seekBarPower.setOnSeekBarChangeListener(new OnSeekBarPowerChangeListener());
		seekBarRed.setOnSeekBarChangeListener(new OnSeekBarRedChangeListener());
		seekBarGreen.setOnSeekBarChangeListener(new OnSeekBarGreenChangeListener());
		seekBarBlue.setOnSeekBarChangeListener(new OnSeekBarBlueChangeListener());

		switchOnOff.setOnCheckedChangeListener(new OnSwitchOnOffChangeListener());
	}

	private void initializeVariables()
	{
		seekBarPower = (SeekBar) findViewById(R.id.seekBarPower);
		seekBarRed = (SeekBar) findViewById(R.id.seekBarRed);
		seekBarGreen = (SeekBar) findViewById(R.id.seekBarGreen);
		seekBarBlue = (SeekBar) findViewById(R.id.seekBarBlue);

		textViewPowerValue = (TextView) findViewById(R.id.textViewPowerValue);
		textViewRedValue = (TextView) findViewById(R.id.textViewRedValue);
		textViewGreenValue = (TextView) findViewById(R.id.textViewGreenValue);
		textViewBlueValue = (TextView) findViewById(R.id.textViewBlueValue);

		switchOnOff = (Switch) findViewById(R.id.switchOnOff);

		responseTextView = (TextView) findViewById(R.id.text);

		queue = Volley.newRequestQueue(this);
	}

	public static String getRgbString()
	{
		String string = (Integer.toHexString(red << 16 | green << 8 | blue)).toUpperCase();

		while (string.length() < 6)
			string = "0" + string;

		return string;
	}

	public static String getPowerString()
	{
		String string = Integer.toString(power);

		while (string.length() < 3)
			string = "0" + string;

		return string;
	}

	public void StringRequestMode(View view)
	{
		String mode = (view.getId() == R.id.buttonModeFlash) ? "1" : (view.getId() == R.id.buttonModeStrobe) ? "2" : (view.getId() == R.id.buttonModeFade) ? "3" : (view.getId() == R.id.buttonModeSmooth) ? "4" : "?";

		RequestQueue queue = MainActivity.queue;

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequest(Request.Method.GET, Resources.espAddress + "/MOD=" + mode,

				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						responseTextView.setText("Response is: " + response);
					}
				},
				new Response.ErrorListener()
				{
					@Override
					public void onErrorResponse(VolleyError error)
					{
						responseTextView.setText("That didn't work!\n\nResponse is: " + error.getMessage());
					}
				});

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}
}