let knob;
let knobState;
let indicator;
let StartDeg = 40;

// let serial;
// let portName = '/dev/tty.usbmodem143201'; // fill in your serial port name here

let button;
let permissionGranted = false;
button = document.getElementById('button');
button.addEventListener('click', requestAccess);

function setup(event) {

    if (typeof (DeviceOrientationEvent) !== 'undefined' && typeof (DeviceOrientationEvent.requestPermission) === 'function') {

        DeviceOrientationEvent.requestPermission()
            .catch(() => {
                button.center();
                button.mousePressed(requestAccess);

            })
            .then(() => {
                permissionGranted = true;
            });
    } else {
        ;
    }

    knob = document.getElementsByClassName('face')[0];
    indicator = document.getElementById('centerFace');
    // knob.addEventListener('click', turnKnob);
    knob.addEventListener('touchstart', turnKnob, false);
    knobState = false;

    // window.addEventListener('mousemove', rotateIndicator);
    // window.addEventListener('touchmove', rotateIndicator);
    // initialize the serialport object: 
    // serial = new p5.SerialPort(); // new instance of the serialport library

    // serial.on('error', serialError); // callback for errors
    // serial.open(portName);           // open a serial port

    // serial.on('data', serialEvent); // callback function for serialport data event

}


function turnKnob() {
    knobState = !knobState;
    if (knobState) {
        knob.classList.remove('green');
        knob.classList.add('blue');
    } else {
        knob.classList.remove('blue');
        knob.classList.add('green');
    }

    // knob.style.boxShadow = "inset 0 -3px 6px rgba(64, 191, 128, 0.1), 0 2px 12px rgba(202, 240, 248, 0.7);"
    console.log(knobState)
}

function rotateIndicator(e) {
    if (knobState) {
        var x = Math.floor(map(e.alpha, 0, 360, 0, 360)) + 40;
        // var y = e.offsetY;
        console.log("rotate(" + x.toString() + ")deg", e.offsetY);
        indicator.style.transform = "rotate(" + x.toString() + "deg)";
    }

}

function requestAccess() {
    DeviceOrientationEvent.requestPermission()
        .then(response => {
            if (response == 'granted') {
                permissionGranted = true;
            } else {
                permissionGranted = false;
            }
        })
        .catch(console.error);
    this.remove();

    if (!permissionGranted) return;
}

// function serialError(err) {
//     console.log('Something went wrong with the serial port. ' + err);
// }

document.body.addEventListener('touchstart', function (e) { e.preventDefault(); });
document.body.addEventListener('touchmove', function (e) { e.preventDefault(); });

window.addEventListener('DOMContentLoaded', setup);
window.addEventListener('deviceorientation', rotateIndicator);