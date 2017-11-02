package fr.iclario.bedcontrol.utils;

import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

import fr.iclario.bedcontrol.activity.ColorPickerActivity;


public class StringRequestBedControl extends StringRequest
{
	public StringRequestBedControl(boolean hasMode, String value, int i, String type)
	{
		super(Method.GET, Resources.espAddress + "/" + type + (hasMode ? i : "") + value,
				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						ColorPickerActivity.colorPickerActivity.setVariables(response);
						ColorPickerActivity.colorPickerActivity.displayValues(response);
					}
				},
				new Response.ErrorListener()
				{
					@Override
					public void onErrorResponse(VolleyError error)
					{
						ColorPickerActivity.colorPickerActivity.responseTextView.setText("Communication avec l'ESP impossible");
					}
				});
	}
}
