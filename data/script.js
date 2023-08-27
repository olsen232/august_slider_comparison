var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);
function onload(event) {
    initWebSocket();
    getCurrentValue();
}
function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen
        = onOpen;
    websocket.onclose
        = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
    console.log('Connection opened');
}
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
    console.log(event.data);
}
function getCurrentValue() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("pwmSlider2").value = this.responseText;
            document.getElementById("textSliderValue2").innerHTML = this.responseText;
        }
    };
    xhr.open("GET", "/currentValue", true);
    xhr.send();
}
function updateSliderPWM(element) {
    var sliderValue = document.getElementById("pwmSlider2").value;
    document.getElementById("textSliderValue2").innerHTML = sliderValue;
    console.log(sliderValue);
    websocket.send(sliderValue);
}