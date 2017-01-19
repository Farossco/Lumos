package fr.iclario.bedcontrol;

import android.widget.TextView;

import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;


public class StringRequestRGB extends StringRequest
{
	public StringRequestRGB(String RGBCode, final TextView textView)
	{
		super(Request.Method.GET, Resources.espAddress + "/RGB=" + RGBCode,
				new Response.Listener<String>()
				{
					@Override
					public void onResponse(String response)
					{
						textView.setText("Response is: " + response);
					}
				},
				new Response.ErrorListener()
				{
					@Override
					public void onErrorResponse(VolleyError error)
					{
						textView.setText("That didn't work!\n\nResponse is: " + error.getMessage());
					}
				});
	}
}
