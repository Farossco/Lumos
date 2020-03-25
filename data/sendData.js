function sendData(type, value, comp, scheduled) {
	var xhttp = new XMLHttpRequest();

	xhttp.open("GET", "command?type=" + type + "&value=" + value + "&comp=" + comp, true);
	xhttp.send();

	xhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			const nl = /\n/gi;
			const space = / /gi;
			var data = JSON.parse(this.responseText);


			document.getElementById("LEDState").innerHTML = data.Light["On"] == true ? "On" : "Off";
			document.getElementById("BrutJson").innerHTML = JSON.stringify(data, null, '     ').replace(nl, "<br>").replace(space, "&nbsp;");
		}
	};
}

// Call a function repetatively with 2 Second interval
setInterval(function () { sendData('INFO', 0, 0, 1); }, 1000);