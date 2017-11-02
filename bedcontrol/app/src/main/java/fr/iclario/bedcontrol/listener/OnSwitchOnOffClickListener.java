package fr.iclario.bedcontrol.listener;

import android.view.View;
import android.widget.Switch;

import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import fr.iclario.bedcontrol.activity.ColorPickerActivity;
import fr.iclario.bedcontrol.utils.StringRequestBedControl;

public class OnSwitchOnOffClickListener implements View.OnClickListener {
    @Override
    public void onClick(View v) {
        ColorPickerActivity.colorPickerActivity.on = ((Switch) v).isChecked();

        // Instantiate the RequestQueue.
        RequestQueue queue;
        queue = Volley.newRequestQueue(ColorPickerActivity.colorPickerActivity);

        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequestBedControl(false, String.valueOf(ColorPickerActivity.colorPickerActivity.on ? 1 : 0), -1, "ONF");

        // Add the request to the RequestQueue.
        queue.add(stringRequest);
    }
}
