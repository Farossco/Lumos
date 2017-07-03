package fr.iclario.bedcontrol;

import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

public class StringRequestPower extends StringRequest
{
	public StringRequestPower(int power)
	{
		super(Request.Method.GET, Resources.espAddress + "/POW=" + power,
				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						MainActivity.setValues(response);
						MainActivity.displayValues();
					}
				},
				new Response.ErrorListener()
				{
					@Override
					public void onErrorResponse(VolleyError error)
					{
						MainActivity.responseTextView.setText("Communication avec l'ESP impossible");
					}
				});
	}
}
