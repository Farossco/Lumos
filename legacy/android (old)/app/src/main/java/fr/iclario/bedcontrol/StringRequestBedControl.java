package fr.iclario.bedcontrol;

import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;


public class StringRequestBedControl extends StringRequest
{
	public StringRequestBedControl(boolean hasMode, String value, int i, String type)
	{
		super(Request.Method.GET, Resources.espAddress + "/" + type + (hasMode ? i : "") + value,
				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						MainActivity.mainActivity.setVariables(response);
						MainActivity.mainActivity.displayValues(response);
					}
				},
				new Response.ErrorListener()
				{
					@Override
					public void onErrorResponse(VolleyError error)
					{
						MainActivity.mainActivity.responseTextView.setText("Communication avec l'ESP impossible");
					}
				});
	}
}
