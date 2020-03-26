function sendData(type, value, comp) {
	var xhttp = new XMLHttpRequest();

	xhttp.open("GET", "command?type=" + type + "&value=" + value + "&comp=" + comp, true);
	xhttp.send();

	xhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			//const nl = /\n/gi;
			//const space = / /gi;
			var data = JSON.parse(this.responseText);

			if (data.Light["On"]) {
				document.getElementById("buttonOnOff").innerHTML = "ON";
				document.getElementById("buttonOnOff").classList.add("button-primary");

			}
			else {
				document.getElementById("buttonOnOff").innerHTML = "OFF";
				document.getElementById("buttonOnOff").classList.remove("button-primary");
			}
			document.getElementById("buttonOnOff").setAttribute("onClick", "sendData('LON', " + (data.Light["On"] ? 0 : 1) + ", 0)");

			//document.getElementById("LEDState").innerHTML = data.Light["On"] == true ? "On" : "Off";
			//document.getElementById("BrutJson").innerHTML = JSON.stringify(data, null, '     ').replace(nl, "<br>").replace(space, "&nbsp;");
		}
	};
}

function test() {
	document.getElementById("buttonOnOff").innerHTML = "ON";
	document.getElementById("buttonOnOff").classList.toggle("button-primary");
}

sendData('INFO', 0, 0);

// Call a function repetatively with 2 Second interval
setInterval(function () { sendData('INFO', 0, 0); }, 1000);