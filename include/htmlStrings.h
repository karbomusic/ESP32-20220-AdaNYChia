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
    <title>LEDMan</title>
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
            console.log("animation=" + value);
            var request = makeHttpObject();
            request.open("GET", '?animation=' + value, false);
            request.send();
        }

        function setBrightness(bValue){
            document.getElementById("briteval").innerHTML = bValue;
            var request = makeHttpObject();
            request.open("GET", '?brite=' + bValue, false);
            request.send();
        }
    </script>
    <style>
        body {
            font-family: calibri, arial;
            background-color: #141d27;
            color: #dddddd;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
            width: 200px;
        }

        .slidecontainer {
            width: 12em;
            margin-left: auto;
            margin-right: auto;
            border-radius: 5px;
            text-align: center;
        }

        .slider {
            -webkit-appearance: none;
            width: 190px;
            height: 35px;
            background: #d3d3d3;
            outline: none;
            opacity: 0.7;
            -webkit-transition: .2s;
            transition: opacity .2s;
            border-radius: 5px;
        }

        .slider:hover {
            opacity: 1;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 35px;
            background: rgb(52, 97, 146);
            cursor: pointer;
            border-radius: 5px;
        }

        .slider::-moz-range-thumb {
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
            width: 10em;
            height: 2.2em;
            font-size: 1.2em;
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
        <h1 style="text-align: center;">Studio Floor Matrix</h1>
        <br>
        <div class="center"><button class="button" onclick="setAnimation('1')">Random Dots</button></div>
        <div class="center"><button class="button" onclick="setAnimation('2')">Random Dots 2</button></div>
        <div class="center"><button class="button" onclick="setAnimation('3')">Noise</button></div>
        <div class="center"><button class="button" onclick="setAnimation('4')">Blue Jumper</button></div>
        <div class="center"><button class="button" onclick="setAnimation('5')">Purple Jumper</button></div>
        <div class="center"><button class="button" onclick="setAnimation('6')">Scroll Color</button></div>
        <div class="center"><button class="button" onclick="setAnimation('7')">Flash Color</button></div>
        <div class="center"><button class="button" onclick="setAnimation('8')">Left to Right</button></div>
        <div class="center"><button class="button" onclick="setAnimation('')">Off</button></div>
        <br>
        <div class="slidecontainer">
            <input type="range" onchange="setBrightness(this.value)" min="15" max="255" value="255" class="slider" id="bSlider">
            <span id="briteval">Brightness</span>
        </div>
    </div>
</body>
</html>
)rawliteral";

