var sendDataTimeout;
var modeNames;
var colors;
var data;
var res;


function sendData(type = 'INFO', value = 0, comp = 0) {
	var xhttp = new XMLHttpRequest();

	xhttp.open("GET", "command?type=" + type + "&value=" + value + "&comp=" + comp, true);
	xhttp.send();

	xhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {

			data = JSON.parse(this.responseText);

			if (data.Light["On"])
				document.getElementById("buttonOnOff").classList.add("button-primary");
			else
				document.getElementById("buttonOnOff").classList.remove("button-primary");

			document.getElementById("buttonOnOff").setAttribute(`onClick`, `sendData('LON', ${data.Light["On"] ? 0 : 1})`);

			document.getElementById("lightModes").innerHTML = `${modeNames.map(mapModenames).join(" ")}`;

			if (data.Light.Mode == 0 || data.Light.Mode == 2 || data.Light.Mode == 3 || data.Light.Mode == 5 || data.Light.Mode == 6)
				document.getElementById("actions").innerHTML =
					`
					<div class="row">
						<h2>Color</h2>
					</div>
					${colors.map(mapColors).join(" ")}
				`;
			else
				document.getElementById("actions").innerHTML = "";

			clearTimeout(sendDataTimeout);
			sendDataTimeout = setTimeout(sendData, 2000);
		}
	};
}

function getResources() {
	var xhttp = new XMLHttpRequest();

	xhttp.open("GET", "getRes");
	xhttp.send();

	xhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			res = JSON.parse(this.responseText);

			modeNames = res.Light.ModeNames;
			colors = res.Light.Colors;
		}
	};
}

function mapModenames(name, index) {
	return `
		<div class="row">
			<button class="button lightMode column ${index == data.Light.Mode ? " button-primary" : ""}" id = "buttonOnOff" onclick = "sendData('LMO', ${index})" > ${name}</button >
		</div>
	`;
}

function str2hex(value) {
	str = value.toString(16);

	while (str.length < 6)
		str = "0" + str;

	return str
}

function mapColors(colorRow, index) {
	var displayFull = colorRow[0] == 0;

	if (!displayFull) {
		return `
		<div class="row">
			${colorRow.map(mapColorRow).join(" ")}
		</div>
	`;
	} else {
		var displayColor = colorRow[1];
		var displayColorStr = str2hex(displayColor);
		var currentMod = data.Light.Mode;
		var currentColor = data.Light.Rgb[currentMod];

		return `
		<div class="row">
			<a class="column button color full${displayColor == currentColor ? " selected" : ""}" style="background-color: #${displayColorStr};" onclick = "sendData('RGB', '${displayColorStr}', ${currentMod})"></a>
		</div>
	`;
	}
}

function mapColorRow(displayColor, index) {
	var currentMod = data.Light.Mode;
	var currentColor = data.Light.Rgb[currentMod];
	var displayColorStr = str2hex(displayColor);

	return `
		<a class="column button color${displayColor == currentColor ? " selected" : ""}" style="background-color: #${displayColorStr};" onclick = "sendData('RGB', '${displayColorStr}', ${currentMod})"></a>
	`;
}

getResources();
sendData();