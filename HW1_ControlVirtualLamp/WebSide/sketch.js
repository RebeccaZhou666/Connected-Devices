// variables for the DOM elements:
let portSelector;
let lampSwitch;
let lampIntensity;
// variable for the serialport object:
let serial;
let portName = '/dev/tty.usbmodem143201'; // fill in your serial port name here
// previous state of the serial input from the button:
let lastButtonState = 0;

// this function is called when the page is loaded. 
// element event listeners are  added here:
function setup(event) {

    lampSwitch = document.getElementById('lampSwitch');
    // lampSwitch.addEventListener('click', setPowerState);

    // initialize the serialport object:
    serial = new p5.SerialPort(); // new instance of the serialport library

    serial.on('error', serialError); // callback for errors
    serial.open(portName);           // open a serial port

    serial.on('data', serialEvent); // callback function for serialport data event

}

function serialError(err) {
    console.log('Something went wrong with the serial port. ' + err);
}

function setLamp(button, intensity) {
    // change its value, depending on its current value:
    if (button) {
        lampSwitch.checked = true;
        document.getElementById("bg_white").style.opacity = map(intensity, 0, 1023, 0, 0.9) + 0.1;
        document.getElementById("lamp").style.opacity = map(intensity, 0, 1023, 0, 0.5) + 0.5;
        document.getElementById("light").style.opacity = map(intensity, 0, 1023, 0, 1);
    } else {
        lampSwitch.checked = false;
        document.getElementById("bg_white").style.opacity = 0.1;
        document.getElementById("lamp").style.opacity = 0.5;
        document.getElementById("light").style.opacity = 0;
        // console.log(button, intensity);
    }

}

// // make a serial port selector object:
// function printList(portList) {
//     // create a select object:
//     portSelector = document.getElementById('portSelector');
//     // portList is an array of serial port names
//     for (var i = 0; i < portList.length; i++) {
//         // add this port name to the select object:
//         var option = document.createElement("option");
//         option.text = portList[i];
//         portSelector.add(option);
//     }
//     // set an event listener for when the port is changed:
//     portSelector.addEventListener('change', openPort);
// }

// function openPort() {
//     let item = portSelector.value;
//     // if there's a port open, close it:
//     if (serial.serialport != null) {
//         serial.close();
//     }
//     // open the new port:
//     serial.open(item);
// }


function serialEvent() {
    // read a line of incoming data:
    var inData = serial.readLine();
    // if the line is not empty, parse it to JSON:
    if (inData) {
        var sensors = JSON.parse(inData);

        console.log(sensors.button, sensors.intensity);

        setLamp(sensors.button, sensors.intensity);

        if (sensors.button) {
            if (sensors.button != lastButtonState) {
                // setLamp(sensors.button, sensors.intensity);
                lastButtonState = sensors.button;
                // setLamp(sensors.button, sensors.intensity);
            }
            setLamp(sensors.button, sensors.intensity);
        } else {
            setLamp(sensors.button, sensors.intensity);
        }


    }
}

// add a listener for the page to load:
window.addEventListener('DOMContentLoaded', setup);