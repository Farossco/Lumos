package fr.iclario.bedcontrol;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.Volley;

public class MainActivity extends AppCompatActivity
{
	public static int RED;
	public static int GREEN;
	public static int BLUE;
	public static boolean on;

	public static SeekBar seekBarRed;
	public static SeekBar seekBarGreen;
	public static SeekBar seekBarBlue;

	public static TextView textViewRedValue;
	public static TextView textViewGreenValue;
	public static TextView textViewBlueValue;

	public static Switch switchOnOff;
	public static TextView textView;

	public static RequestQueue queue;

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

		switchOnOff.setOnCheckedChangeListener(new OnSwitchOnOffChangeListener());
	}


	private void initializeVariables()
	{
		seekBarRed = (SeekBar) findViewById(R.id.seekBarRed);
		seekBarGreen = (SeekBar) findViewById(R.id.seekBarGreen);
		seekBarBlue = (SeekBar) findViewById(R.id.seekBarBlue);

		textViewRedValue = (TextView) findViewById(R.id.textViewRedValue);
		textViewGreenValue = (TextView) findViewById(R.id.textViewGreenValue);
		textViewBlueValue = (TextView) findViewById(R.id.textViewBlueValue);

		switchOnOff = (Switch) findViewById(R.id.switchOnOff);

		textView = (TextView) findViewById(R.id.text);

		queue = Volley.newRequestQueue(this);
	}

	public static String getRGB()
	{
		String string = (Integer.toHexString(RED << 16 | GREEN << 8 | BLUE)).toUpperCase();

		while (string.length() < 6)
			string = "0" + string;

		return string;
	}
}
