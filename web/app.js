const firebaseConfig = {
    apiKey: "AIzaSyC87A38QuE8E06DE9ceJc95G724zz0Ledw",
    authDomain: "anhuynhsmarthome.firebaseapp.com",
    databaseURL: "https://anhuynhsmarthome-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "anhuynhsmarthome",
    storageBucket: "anhuynhsmarthome.appspot.com",
    messagingSenderId: "451784600525",
    appId: "1:451784600525:web:889e6bfbcdc29ca791f284"
  };
  firebase.initializeApp(firebaseConfig);
  var database = firebase.database();

const rootRef = database.ref('plants');

// ===============connect====================
const temp = document.getElementById("temperature");
const id = document.getElementById("id");
const huni = document.getElementById("humidity");
const time = document.getElementById("real_time");
const TEN1 = document.getElementById("name1");
const TEN2 = document.getElementById("name2");
const TEN3 = document.getElementById("name3");

const addbtn = document.getElementById("addbtn");
const updatebtn = document.getElementById("updatebtn");
const removebtn = document.getElementById("removebtn");

let liv_light = document.getElementById('liv_light');
let wifi = document.getElementById('wifi');
let pump = document.getElementById('pump');
let  a=0, b=0, c=0;

liv_light.onclick = function(){
	a+=0.5;
	if (a%2 == 1) {
		console.log("Light on");
		database.ref("/LivingRoom").update({
            "Led_1" : 1
        });
	}
	if (a%2 ==0) {
		console.log("Light off");
		database.ref("/LivingRoom").update({
            "Led_1" : 0
        });
	}
}

wifi.onclick = function(){
	b+=0.5;
	if (b%2 == 1) {
		console.log("Wifi on");
		database.ref("/LivingRoom").update({
            "Led_2" : 1
        });
	}
	if (b%2 ==0) {
		console.log("Wifi off");
		database.ref("/LivingRoom").update({
            "Led_2" : 0
        });

	}
}


pump.onclick = function(){
	c+=0.5;
	if (c%2 == 1) {
		console.log("Pump on");
		database.ref("/LivingRoom").update({
            "Led_1" : 1
        });
	}
	if (c%2 ==0) {
		console.log("Pump off");
		database.ref("/LivingRoom").update({
            "Led_1" : 0
        });
	}
}










var tab1 = document.getElementById("tab1");
tab1.onclick = function(){
    console.log("Đang ở cây 1");
}

var tab2 = document.getElementById("tab2");
tab2.onclick = function(){
    console.log("Đang ở cây 2");
}

var tab3 = document.getElementById("tab3");
tab3.onclick = function(){
    console.log("Đang ở cây 3");
}


// ================real time====================
function startTime() {
    var today = new Date();
    var h = today.getHours();
    var m = today.getMinutes();
    var s = today.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    document.getElementById('txt').innerHTML = h + ":" + m + ":" + s;
    var t = setTimeout(startTime, 500);
}
function checkTime(i) {
    if (i < 10) {i = "0" + i};  // add zero in front of numbers < 10
    return i;
}