package fr.iclario.bedcontrol;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONObject;

public class MainActivity extends AppCompatActivity
{
	public static int power;
	public static int mode;
	public static int rgb;
	public static int red;
	public static int green;
	public static int blue;
	public static boolean on;
	public static String status;
	public static String message;

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

	public static RadioGroup radioGroup;

	public static RequestQueue queue;

	Handler handler = new Handler();

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

		handler.post(runnableCode);
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

		radioGroup = (RadioGroup) findViewById(R.id.radioModeGroup);

		switchOnOff = (Switch) findViewById(R.id.switchOnOff);

		responseTextView = (TextView) findViewById(R.id.text);

		queue = Volley.newRequestQueue(this);
	}

	private Runnable runnableCode = new Runnable()
	{
		@Override
		public void run()
		{
			StringRequest stringRequest = new StringRequestAll();
			queue.add(stringRequest);
			handler.postDelayed(runnableCode, 10000);
		}
	};

	public static String getRgbString()
	{
		String string = (Integer.toHexString(red << 16 | green << 8 | blue)).toUpperCase();

		while (string.length() < 6)
			string = "0" + string;

		return string;
	}


	public void onModeChange(View view)
	{
		mode = (view.getId() == R.id.radioModeDefault) ? 0 : (view.getId() == R.id.radioModeFlash) ? 1 : (view.getId() == R.id.radioModeStrobe) ? 2 : (view.getId() == R.id.radioModeFade) ? 3 : (view.getId() == R.id.radioModeSmooth) ? 4 : (view.getId() == R.id.radioModeWakeUp) ? 5 : -1;

		RequestQueue queue = MainActivity.queue;

		// Request a string response from the provided URL.
		StringRequest stringRequest = new StringRequest(Request.Method.GET, Resources.espAddress + "/MOD=" + mode,

				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						setValues(response);
						displayValues();
					}
				},
				new Response.ErrorListener()
				{
					@Override
					public void onErrorResponse(VolleyError error)
					{
						responseTextView.setText("Communication avec l'ESP impossible");
					}
				});

		// Add the request to the RequestQueue.
		queue.add(stringRequest);
	}

	public static void setValues(String json)
	{
		try
		{
			JSONObject jsonGlobal = new JSONObject(json);
			JSONObject jsonData = jsonGlobal.getJSONObject("Datas");

			status = jsonGlobal.getString("Status");
			message = jsonGlobal.getString("Message");
			on = jsonData.getInt("On") != 0;
			rgb = jsonData.getInt("RGB");
			red = rgb / 256 / 256;
			green = (rgb / 256) % 256;
			blue = rgb % 256;
			power = jsonData.getInt("Power");
			mode = jsonData.getInt("Mode");
		}
		catch (org.json.JSONException e)
		{
			responseTextView.setText(e.getLocalizedMessage());
		}
	}

	public static void displayValues()
	{
		responseTextView.setText("" +
				"Status: " + status +
				"\nMessage: " + message +
				"\nOn: " + on +
				"\nRGB: " + rgb +
				"\nPower: " + power +
				"\nMode: " + mode);

		setValues();
	}

	public static void setValues()
	{
		switchOnOff.setChecked(on);
		if (Build.VERSION.SDK_INT >= 24)
		{
			seekBarPower.setProgress(power, true);
			seekBarRed.setProgress(red, true);
			seekBarGreen.setProgress(green, true);
			seekBarBlue.setProgress(blue, true);
		}
		else
		{
			seekBarPower.setProgress(power);
			seekBarRed.setProgress(red);
			seekBarGreen.setProgress(green);
			seekBarBlue.setProgress(blue);
		}
		radioGroup.check(mode == 1 ? R.id.radioModeFlash : mode == 2 ? R.id.radioModeStrobe : mode == 3 ? R.id.radioModeFade : mode == 4 ? R.id.radioModeSmooth : mode == 5 ? R.id.radioModeWakeUp : R.id.radioModeDefault);
	}
}