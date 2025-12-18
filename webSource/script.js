function ledOn() {
  fetch("/api/led/on", { method: "POST" });
}

function ledOff() {
  fetch("/api/led/off", { method: "POST" });
}

// open event stream
const evtSource = new EventSource("/events");

evtSource.onmessage = function(event) {
  console.log("ICH WURDE AUFGERUFEN UND ES GAB EINEN FETTEN FEHLER!")
  const data = JSON.parse(event.data);
  if (data.temperature !== undefined) {
    document.getElementById("temp").innerText = data.temperature;
  }
  if (data.fs_usage !== undefined) {
    document.getElementById("fs").innerText = data.fs_usage;
  }
};