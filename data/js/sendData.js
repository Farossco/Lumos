var modeNames;

var data;

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

			document.getElementById("buttonOnOff").setAttribute(`onClick`, `sendData('LON', ${(data.Light["On"] ? 0 : 1)})`);

			document.getElementById("lightModes").innerHTML = `${modeNames.map(mapModenames).join(" ")}`;
		}
	};
}

function getStrings() {
	var xhttp = new XMLHttpRequest();

	xhttp.open("GET", "getStrings");
	xhttp.send();

	xhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {

			data = JSON.parse(this.responseText);

			modeNames = data.Light.ModeNames;
		}
	};
}

function mapModenames(name, index) {

	return `
		<div class="row">
			<button class="button column ${index == data.Light.Mod ? " button-primary" : ""}" id = "buttonOnOff" onclick = "sendData('LMO', ${index})" > ${name}</button >
		</div>
	`;
}

getStrings();
setInterval(sendData, 500);