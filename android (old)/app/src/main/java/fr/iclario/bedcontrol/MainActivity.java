package fr.iclario.bedcontrol;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.design.widget.Snackbar;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    public static MainActivity mainActivity;

    public int power[] = new int[6];
    public int speed[] = new int[6];
    public int mode;
    public int rgb[] = new int[4];
    public boolean on;
    public String status;
    public String message;

    public boolean powerChanged[] = new boolean[6];
    public boolean speedChanged[] = new boolean[6];
    public boolean modeChanged;
    public boolean rgbChanged[] = new boolean[3];
    public boolean onChanged;

    public SeekBar seekBarPower[] = new SeekBar[5];
    public SeekBar seekBarSpeed[] = new SeekBar[6];
    public ProgressBar progressBarWakeUp;
    public SeekBar seekBarRgb[] = new SeekBar[3];

    public TextView textViewRgbValue[] = new TextView[3];

    public Switch switchOnOff;
    public TextView responseTextView;

    public RadioGroup radioGroup;

    static Handler handler = new Handler();

    boolean paused;

    protected void onStart() {
        super.onStart();
        this.paused = false;
        handler.post(runnableCode);
    }

    protected void onStop() {
        super.onStop();
        this.paused = true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        mainActivity = this;

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);



        //Intent intent = new Intent(MainActivity.this, SettingsActivity.class);
        //startActivity(intent);
    }

    private void initializeVariables() {
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

        seekBarRgb[0] = (SeekBar) findViewById(R.id.seekBarRed);
        seekBarRgb[1] = (SeekBar) findViewById(R.id.seekBarGreen);
        seekBarRgb[2] = (SeekBar) findViewById(R.id.seekBarBlue);

        textViewRgbValue[0] = (TextView) findViewById(R.id.textViewRedValue);
        textViewRgbValue[1] = (TextView) findViewById(R.id.textViewGreenValue);
        textViewRgbValue[2] = (TextView) findViewById(R.id.textViewBlueValue);

        radioGroup = (RadioGroup) findViewById(R.id.radioModeGroup);

        switchOnOff = (Switch) findViewById(R.id.switchOnOff);

        responseTextView = (TextView) findViewById(R.id.text);
    }

    public Runnable runnableCode = new Runnable() {
        @Override
        public void run() {
            RequestQueue queue;
            queue = Volley.newRequestQueue(MainActivity.mainActivity);
            StringRequest stringRequest = new StringRequestBedControl(false, "", -1, "INFO");
            queue.add(stringRequest);

            if (!paused)
                handler.postDelayed(runnableCode, 500);

        }
    };

    public void onModeChange(View view) {
        mode = (view.getId() == R.id.radioModeDefault) ? 0 : (view.getId() == R.id.radioModeFlash) ? 1 : (view.getId() == R.id.radioModeStrobe) ? 2 : (view.getId() == R.id.radioModeFade) ? 3 : (view.getId() == R.id.radioModeSmooth) ? 4 : (view.getId() == R.id.radioModeWakeUp) ? 5 : -1;

        // Instantiate the RequestQueue.
        RequestQueue queue;
        queue = Volley.newRequestQueue(MainActivity.mainActivity);

        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequestBedControl(false, String.valueOf(mode), -1, "MOD");

        // Add the request to the RequestQueue.
        queue.add(stringRequest);
    }

    public boolean setVariables(String json) {
        for (int i = 0; i < 6; i++) {
            powerChanged[i] = false;
            speedChanged[i] = false;
        }

        modeChanged = false;
        for (int i = 0; i < 3; i++)
            rgbChanged[i] = false;

        onChanged = false;

        try {
            JSONObject jsonGlobal = new JSONObject(json);
            JSONObject jsonData = jsonGlobal.getJSONObject("Datas");
            JSONArray jsonDataPower = jsonData.getJSONArray("Power");
            JSONArray jsonDataSpeed = jsonData.getJSONArray("Speed");

            status = jsonGlobal.getString("Status");
            message = jsonGlobal.getString("Message");

            for (int i = 0; i < 6; i++) {
                if (power[i] != jsonDataPower.getInt(i)) {
                    power[i] = jsonDataPower.getInt(i);
                    powerChanged[i] = true;
                }

                if (speed[i] != jsonDataSpeed.getInt(i)) {
                    speed[i] = jsonDataSpeed.getInt(i);
                    speedChanged[i] = true;
                }
            }

            if (on != (jsonData.getInt("On") == 1)) {
                on = jsonData.getInt("On") == 1;
                onChanged = true;
            }

            rgb[3] = jsonData.getInt("Rgb");

            if (rgb[0] != rgb[3] / 256 / 256) {
                rgb[0] = rgb[3] / 256 / 256;
                rgbChanged[0] = true;
            }

            if (rgb[1] != (rgb[3] / 256) % 256) {
                rgb[1] = (rgb[3] / 256) % 256;
                rgbChanged[1] = true;
            }

            if (rgb[2] != rgb[3] % 256) {
                rgb[2] = rgb[3] % 256;
                rgbChanged[2] = true;
            }

            if (mode != jsonData.getInt("Mode")) {
                mode = jsonData.getInt("Mode");
                modeChanged = true;
            }

            return jsonGlobal.getString("Status").equals("OK");
        } catch (Exception e) {
            responseTextView.setText(e.getLocalizedMessage());
            return false;
        }
    }

    public void displayValues(String json) {
        String text = "";

        text += "Status: ";
        text += status;
        text += "\nMessage: ";
        text += message;
        text += "\nOn: ";
        text += on;
        text += "\nRGB: ";
        text += Integer.toHexString(rgb[3]).toUpperCase();
        text += " (";
        text += rgb[3];
        text += ")";
        text += "\nRed: ";
        text += rgb[0];
        text += "\nGreen: ";
        text += rgb[1];
        text += "\nBlue: ";
        text += rgb[2];

        text += "\nPower: [";
        for (int i = 0; i < 6; i++) {
            text += power[i];
            if (i != 5)
                text += ", ";
        }

        text += "]\nSpeed: [";
        for (int i = 0; i < 6; i++) {
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

    public void setValues() {
        switchOnOff.setChecked(on);

        if (Build.VERSION.SDK_INT >= 24) {
            for (int i = 0; i < 3; i++)
                if (rgbChanged[i])
                    seekBarRgb[i].setProgress(rgb[i], true);

            for (int i = 0; i < 5; i++)
                if (powerChanged[i])
                    seekBarPower[i].setProgress(power[i], true);

            for (int i = 1; i < 6; i++)
                if (speedChanged[i])
                    seekBarSpeed[i].setProgress(speed[i], true);

            progressBarWakeUp.setProgress(power[5], true);
        } else {
            for (int i = 0; i < 3; i++)
                if (rgbChanged[i])
                    seekBarRgb[i].setProgress(rgb[i]);

            for (int i = 0; i < 5; i++)
                if (powerChanged[i])
                    seekBarPower[i].setProgress(power[i]);

            for (int i = 1; i < 5; i++)
                if (speedChanged[i])
                    seekBarSpeed[i].setProgress(speed[i]);

            progressBarWakeUp.setProgress(power[5]);
        }

        radioGroup.check(mode == 1 ? R.id.radioModeFlash : mode == 2 ? R.id.radioModeStrobe : mode == 3 ? R.id.radioModeFade : mode == 4 ? R.id.radioModeSmooth : mode == 5 ? R.id.radioModeWakeUp : R.id.radioModeDefault);
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_camera) {
            // Handle the camera action
        } else if (id == R.id.nav_gallery) {

        } else if (id == R.id.nav_slideshow) {

        } else if (id == R.id.nav_manage) {

        } else if (id == R.id.nav_share) {

        } else if (id == R.id.nav_send) {

        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }
}
