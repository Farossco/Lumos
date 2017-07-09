package fr.iclario.bedcontrol;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ProgressBar;
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

import org.json.JSONArray;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity
{
	public static int power[] = new int[6];
	public static int speed[] = new int[6];
	public static int mode;
	public static int rgb;
	public static int red;
	public static int green;
	public static int blue;
	public static boolean on;
	public static String status;
	public static String message;

	public static boolean powerChanged[] = new boolean[6];
	public static boolean speedChanged[] = new boolean[6];
	public static boolean modeChanged;
	public static boolean redChanged;
	public static boolean greenChanged;
	public static boolean blueChanged;
	public static boolean onChanged;

	public static SeekBar seekBarPower[] = new SeekBar[5];
	public static SeekBar seekBarSpeed[] = new SeekBar[6];
	public static ProgressBar progressBarWakeUp;
	public static SeekBar seekBarRed;
	public static SeekBar seekBarGreen;
	public static SeekBar seekBarBlue;

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
		textViewRedValue.setText(seekBarRed.getProgress() + "/" + seekBarRed.getMax());
		textViewGreenValue.setText(seekBarGreen.getProgress() + "/" + seekBarGreen.getMax());
		textViewBlueValue.setText(seekBarBlue.getProgress() + "/" + seekBarBlue.getMax());

		seekBarRed.setOnSeekBarChangeListener(new OnSeekBarRedChangeListener());
		seekBarGreen.setOnSeekBarChangeListener(new OnSeekBarGreenChangeListener());
		seekBarBlue.setOnSeekBarChangeListener(new OnSeekBarBlueChangeListener());

		for (int i = 0; i < 5; i++)
			seekBarPower[i].setOnSeekBarChangeListener(new OnSeekBarPowerChangeListener(i));

		for (int i = 1; i < 6; i++)
			seekBarSpeed[i].setOnSeekBarChangeListener(new OnSeekBarSpeedChangeListener(i));

		switchOnOff.setOnClickListener(new OnSwitchOnOffClickListener());

		handler.post(runnableCode);
	}

	private void initializeVariables()
	{
		seekBarPower[0] = (SeekBar) findViewById(R.id.seekBarPowerDefault);
		seekBarPower[1] = (SeekBar) findViewById(R.id.seekBarPowerFlash);
		seekBarPower[2] = (SeekBar) findViewById(R.id.seekBarPowerStrobe);
		seekBarPower[3] = (SeekBar) findViewById(R.id.seekBarPowerFade);
		seekBarPower[4] = (SeekBar) findViewById(R.id.seekBarPowerSmooth);

		progressBarWakeUp = (ProgressBar) findViewById(R.id.seekBarWakeUpProgress);

		seekBarSpeed[1] = (SeekBar) findViewById(R.id.seekBarSpeedFlash);
		seekBarSpeed[2] = (SeekBar) findViewById(R.id.seekBarSpeedStrobe);
		seekBarSpeed[3] = (SeekBar) findViewById(R.id.seekBarSpeedFade);
		seekBarSpeed[4] = (SeekBar) findViewById(R.id.seekBarSpeedSmooth);
		seekBarSpeed[5] = (SeekBar) findViewById(R.id.seekBarSpeedWakeUp);

		seekBarRed = (SeekBar) findViewById(R.id.seekBarRed);
		seekBarGreen = (SeekBar) findViewById(R.id.seekBarGreen);
		seekBarBlue = (SeekBar) findViewById(R.id.seekBarBlue);

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
			StringRequest stringRequest = new StringRequestBedControl(false, "", -1, "INFO");
			queue.add(stringRequest);
			handler.postDelayed(runnableCode, 1000);
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
		StringRequest stringRequest = new StringRequest(Request.Method.GET, Resources.espAddress + "/MOD" + mode,

				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						setVariables(response);
						displayValues(response);
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

	public static void setVariables(String json)
	{
		for (int i = 0; i < 6; i++)
		{
			powerChanged[i] = false;
			speedChanged[i] = false;
		}

		modeChanged = false;
		redChanged = false;
		greenChanged = false;
		blueChanged = false;
		onChanged = false;

		try
		{
			JSONObject jsonGlobal = new JSONObject(json);
			JSONObject jsonData = jsonGlobal.getJSONObject("Datas");
			JSONArray jsonDataPower = jsonData.getJSONArray("Power");
			JSONArray jsonDataSpeed = jsonData.getJSONArray("Speed");

			status = jsonGlobal.getString("Status");
			message = jsonGlobal.getString("Message");

			for (int i = 0; i < 6; i++)
			{
				if (power[i] != jsonDataPower.getInt(i))
				{
					power[i] = jsonDataPower.getInt(i);
					powerChanged[i] = true;
				}

				if (speed[i] != jsonDataSpeed.getInt(i))
				{
					speed[i] = jsonDataSpeed.getInt(i);
					speedChanged[i] = true;
				}
			}

			if (on != (jsonData.getInt("On") != 0))
			{
				on = jsonData.getInt("On") != 0;
				onChanged = true;
			}
			if (rgb != jsonData.getInt("Rgb"))
			{

			}
			rgb = jsonData.getInt("Rgb");

			if (red != rgb / 256 / 256)
			{
				red = rgb / 256 / 256;
				redChanged = true;
			}

			if (green != (rgb / 256) % 256)
			{
				green = (rgb / 256) % 256;
				greenChanged = true;
			}

			if (blue != rgb % 256)
			{
				blue = rgb % 256;
				blueChanged = true;
			}

			if (mode != jsonData.getInt("Mode"))
			{
				mode = jsonData.getInt("Mode");
				modeChanged = true;
			}
		}
		catch (Exception e)
		{
			responseTextView.setText(e.getLocalizedMessage());
		}
	}

	public static void displayValues(String json)
	{
		String text = "";

		text += "Status: ";
		text += status;
		text += "\nMessage: ";
		text += message;
		text += "\nOn: ";
		text += on;
		text += "\nRGB: ";
		text += rgb;

		text += "\nPower: [";
		for (int i = 0; i < 6; i++)
		{
			text += power[i];
			if (i != 5)
				text += ", ";
		}

		text += "]\nSpeed: [";
		for (int i = 0; i < 6; i++)
		{
			text += speed[i];
			if (i != 5)
				text += ", ";
		}

		text += "]\nMode: ";
		text += mode;
		text += "\n\n";
		text += json;

		responseTextView.setText(text);

		setValues();
	}

	public static void setValues()
	{
		switchOnOff.setChecked(on);

		if (Build.VERSION.SDK_INT >= 24)
		{
			if (redChanged)
				seekBarRed.setProgress(red, true);
			if (greenChanged)
				seekBarGreen.setProgress(green, true);
			if (blueChanged)
				seekBarBlue.setProgress(blue, true);

			for (int i = 0; i < 5; i++)
			{
				if (powerChanged[i])
					seekBarPower[i].setProgress(power[i], true);
			}
			for (int i = 1; i < 6; i++)
			{
				if (speedChanged[i])
					seekBarSpeed[i].setProgress(speed[i], true);
			}

			progressBarWakeUp.setProgress(power[5], true);
		}
		else
		{
			if (redChanged)
				seekBarRed.setProgress(red);
			if (greenChanged)
				seekBarGreen.setProgress(green);
			if (blueChanged)
				seekBarBlue.setProgress(blue);

			for (int i = 0; i < 5; i++)
			{
				if (powerChanged[i])
					seekBarPower[i].setProgress(power[i]);
			}
			for (int i = 1; i < 5; i++)
			{
				if (speedChanged[i])
					seekBarSpeed[i].setProgress(speed[i]);
			}

			progressBarWakeUp.setProgress(power[5]);
		}
		
		radioGroup.check(mode == 1 ? R.id.radioModeFlash : mode == 2 ? R.id.radioModeStrobe : mode == 3 ? R.id.radioModeFade : mode == 4 ? R.id.radioModeSmooth : mode == 5 ? R.id.radioModeWakeUp : R.id.radioModeDefault);
	}
}