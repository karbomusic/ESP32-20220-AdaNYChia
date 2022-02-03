/*+===================================================================
  File:      strings.h

  Summary:   A place for strings.   
             
  Kary Wall 2022.
===================================================================+*/

#include <Arduino.h>

// Post restart redirect
const String metaRedirect ="<html><head><meta http-equiv=\"refresh\"content=\"10;url=/about\"/></head><body>"
                           "Restarting in 5 seconds...<br>Returning to about page in 20 seconds.</body></html>";

// control panel
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED Man: Studio Floor</title>
    <!-- <link href="styles.css" type="text/css" rel="stylesheet"> -->
    <script type=text/javascript>
        function makeHttpObject()  {
            try { return new XMLHttpRequest(); }
            catch (error) { }
            try { return new ActiveXObject("Msxml2.XMLHTTP"); }
            catch (error) { }
            try { return new ActiveXObject("Microsoft.XMLHTTP"); }
            catch (error) { } 
        }

        function setAnimation(value){
            var request = makeHttpObject();
            request.open("GET", '?animation=' + value, false);
            request.onload = function(e){
                if(request.status === 200)
                {
                    console.log(request.responseText);
                }
                else
                {
                    console.log("Error: " + request.statusText);
                }
            }
            request.onerror = function(e){
                console.error(request.statusText);
            }
            request.send();
        }

        function setColor(sliderId){
            var h = document.getElementById("hue").value;
            var s = document.getElementById("sat").value;
            var v = document.getElementById("bri").value;       
            var request = makeHttpObject();
            var querystring;
           
            if(sliderId==="bri") 
            { 
                request.open("GET", '?bri=' + v, false); // brightness is global 
            }
            else
            {
                request.open("GET", '?hue=' + h + '&sat=' + s + '&bri=' + v, false);
            }
            request.onload = function(e){
                if(request.status === 200)
                {
                    console.log(request.responseText);
                }
                else
                {
                    console.log("Error: " + request.statusText);
                }
            }
            request.onerror = function(e){
                console.error(request.statusText);
            }
            request.send();
        }

    </script>
    <style>
        body {
            font-family: calibri, arial;
            background-color: #12182b;
            color: #dddddd;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
        }

        .aniContainer {
            width: 370px;
            margin-left: auto;
            margin-right: auto;
            border-radius: 5px;
            text-align: center;
            border: 1px solid #464646;
            padding:8px;
           /* padding-right:13px; */
        }
        
        .hsvSlidecontainer {
            width: 370px;
            margin-left: auto;
            margin-right: auto;
            border-radius: 5px;
            text-align: center;
            border: 1px solid #464646;
            padding:8px;
        }

        .hueSlider {
            -webkit-appearance: none;
            width: 350px;
            height: 35px;
            background: linear-gradient(90deg, rgba(255,0,0,1) 0%, rgba(241,255,0,1) 17%, 
            rgba(84,252,69,1) 33%, rgba(69,252,236,1) 49%, rgba(69,81,252,1) 65%, 
            rgba(252,69,250,1) 81%, rgba(252,69,69,1) 99%);
            outline: none;
            opacity: 0.7;
            -webkit-transition: .2s;
            transition: opacity .2s;
            border-radius: 5px;
            margin-bottom: 20px;
        }

        .slider {
            -webkit-appearance: none;
            width: 350px;
            height: 35px;
            background: #414558;
            outline: none;
            opacity: 0.7;
            -webkit-transition: .2s;
            transition: opacity .2s;
            border-radius: 5px;
            margin-bottom: 20px;
        }

        .slider:hover {
            opacity: 1;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 35px;
            background: rgb(12, 37, 117);
            cursor: pointer;
            border-radius: 5px;
            border: 1px solid #dddddd;
        }

        .slider::-moz-range-thumb {
            width: 25px;
            height: 25px;
            background: #04AA6D;
            cursor: pointer;
        }

        .hueSlider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 35px;
            background: rgb(12, 37, 117);
            cursor: pointer;
            border-radius: 5px;
            border: 1px solid #ffffff;
        }

        .hueSlider::-moz-range-thumb {
            width: 25px;
            height: 25px;
            background: #04AA6D;
            cursor: pointer;
        }

        .button {
            background-color: #3c5168;
            color: #dddddd;
            border: 1px solid #dddddd;
            border-radius: 5px;
            padding: 5px;
            margin: 5px;
            width: 350px;
            height: 48px;
            font-size: 14pt;
            border: 0;
        }

        .smallButton {
            background-color: rgb(52, 97, 146);
            color: #dddddd;
            border: 1px solid #dddddd;
            border-radius: 5px;
            padding: 5px;
            margin: 5px;
            width: 50px;
            height: 30px;
            font-size: 10pt;
            border: 0;
        }

        .button:active {
            background-color: #dddddd;
            color: #223344;
        }

        .smallButton:active {
            background-color: #dddddd;
            color: #223344;
        }
    </style>
</head>

<body>
    <div>
        <h1 style="text-align: center;">Studio Floor Lighting</h1>
        <!-- <h2 style="text-align: center;">Change Color</h2> -->
        <div class="hsvSlidecontainer">
            Hue <input id="hue" type="range" onchange="setColor(this.id)"  min="0" max="255" value="0" class="hueSlider">
            Saturation <input id="sat" type="range" onchange="setColor(this.id)"  min="0" max="255" value="255" class="slider">
            Brightness<input id="bri" type="range" onchange="setColor(this.id)"  min="24" max="255" value="150" class="slider">
        </div>
        <h2 style="text-align: center;">Animations</h2>
        <div class="aniContainer">
        <div class="center"><button class="button" onclick="setAnimation('1')">Random Dots</button></div>
        <div class="center"><button class="button" onclick="setAnimation('2')">Random Dots 2</button></div>
        <div class="center"><button class="button" onclick="setAnimation('3')">Noise</button></div>
        <div class="center"><button class="button" onclick="setAnimation('4')">Blue Jumper</button></div>
        <div class="center"><button class="button" onclick="setAnimation('5')">Purple Jumper</button></div>
        <div class="center"><button class="button" onclick="setAnimation('6')">Scroll Color</button></div>
        <div class="center"><button class="button" onclick="setAnimation('7')">Flash Color</button></div>
        <div class="center"><button class="button" onclick="setAnimation('8')">Left to Right</button></div>
        <div class="center"><button class="button" onclick="setAnimation('')">Off</button></div>
    </div>
        <br>
    </div>
</body>
</html>
)rawliteral";


