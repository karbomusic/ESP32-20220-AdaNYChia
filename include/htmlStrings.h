/*+===================================================================
  File:      htmlStrings.h

  Summary:   A place for HTML that is persisted in flash memory,
             via PROGMEM. If we decide to keep using it this way 
             that is. We actually have more RAM than flash so I may
             move it to memory later; as we don't need persistence.  

             I was saving this to flash using SPIFFs which is great
             except it requires a separate filesystem upload which
             is OK, except it's too easy to forget to do when
             you're deploying firmware to a new device. It just
             creates extra deployment steps.
             
  Kary Wall 2/20/2022.
===================================================================+*/

#include <Arduino.h>

// Post restart redirect to home page after 10 seconds.
const String metaRedirect ="<html><head><meta http-equiv=\"refresh\"content=\"10;url=/about\"/></head><body>"
                           "Restarting in 5 seconds...<br>Returning to about page in 10 seconds.</body></html>";

// LED Web control panel
//const char index_html[] PROGMEM = R"rawliteral(
 String index_html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{TITLE}</title>
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
          if(!request)
          { 
               request = makeHttpObject();
          }
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

        function updateSliders(value)
        {
            values = value.split(",");
            var hSlider = document.getElementById("hue").value = values[0];
            var sSlider = document.getElementById("sat").value = values[1];
            var vSlider = document.getElementById("bri").value = values[2];
            document.getElementById("hue-text").innerHTML = "Hue: " + hSlider;
            document.getElementById("sat-text").innerHTML = "Sat: " + sSlider;
            document.getElementById("bri-text").innerHTML = "Bri: " + vSlider;
        }

        function setSwatch(value){
           updateSliders(value);
           if(!request)
           { 
               request = makeHttpObject();
           }
            request.open("GET", '?swat=' + value, false);
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

           if(!request)
           { 
               request = makeHttpObject();
           }
            var querystring;
           
            if(sliderId==="bri") 
            { 
                document.getElementById("bri-text").innerHTML = "Bri: " + v;
                request.open("GET", '?bri=' + v, false); // brightness is global 
            }
            else if(sliderId==="sat")
            {
                document.getElementById("sat-text").innerHTML = "Sat: " + s;
                request.open("GET", '?sat=' + s, false); // saturation is global
            }
            else if(sliderId==="hue")
            {
                document.getElementById("hue-text").innerHTML = "Hue: " + h;
                request.open("GET", '?hue=' + h, false); // hue is global
            }
            else
            {
                console.log("Error: sliderId not recognized");
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

        // entry point
        var request = makeHttpObject();
    </script>
    <style>
        body {
            font-family: calibri, arial;
            background-color: #12182b;
            color: #dddddd;
        }

        .grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            grid-gap: 10px;
            max-width: 370px;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
        }

        .unselectable {
            -webkit-touch-callout: none;
            -webkit-user-select: none;
            -khtml-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
        }

        .solidColorContainer {
            width: 370px;
            margin-left: auto;
            margin-right: auto;
            border-radius: 5px;
            text-align: center;
            border: 1px solid #464646;
            padding: 8px;
        }

        .aniContainer {
            width: 370px;
            margin-left: auto;
            margin-right: auto;
            border-radius: 5px;
            text-align: center;
            border: 1px solid #464646;
            padding: 8px;
        }

        .hsvSlidecontainer {
            width: 370px;
            margin-left: auto;
            margin-right: auto;
            border-radius: 5px;
            text-align: center;
            border: 1px solid #464646;
            padding: 8px;
        }

        .hueSlider {
            -webkit-appearance: none;
            width: 350px;
            height: 35px;
            background: linear-gradient(90deg, rgba(255, 0, 0, 1) 0%, rgba(241, 255, 0, 1) 17%,
                    rgba(84, 252, 69, 1) 33%, rgba(69, 252, 236, 1) 49%, rgba(69, 81, 252, 1) 65%,
                    rgba(252, 69, 250, 1) 81%, rgba(252, 69, 69, 1) 99%);
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

        .swatch {
            background-color: #3c5168;
            color: #dddddd;
            border: 1px solid #dddddd;
            border-radius: 5px;
            padding: 5px;
            margin: 5px;
            width: 70px;
            height: 70px;
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
        <h1 style="text-align: center;">{HEADING}</h1>
        <div class="solidColorContainer">
            <div class="grid">
                <div><button class="swatch" style="background-color:#f2feff" onclick="setSwatch('85,76,254')"></button>
                </div>
                <div><button class="swatch" style="background-color:#c4e6e6" onclick="setSwatch('72,61,254')"></button>
                </div>
                <div><button class="swatch" style="background-color:#9dfbfb" onclick="setSwatch('72,115,130')"></button>
                </div>
                <div><button class="swatch" style="background-color:#92a1a1" onclick="setSwatch('72,61,85')"></button>
                </div>
                <div><button class="swatch" style="background-color:#fbe1ac" onclick="setSwatch('49,140,254')"></button>
                </div>
                <div><button class="swatch" style="background-color:#f9cd77" onclick="setSwatch('28,170,216')"></button>
                </div>
                <div><button class="swatch" style="background-color:#f4a60b" onclick="setSwatch('29,170,149')"></button>
                </div>
                <div><button class="swatch" style="background-color:#c38509" onclick="setSwatch('29,244,88')"></button>
                </div>
                <div><button class="swatch" style="background-color:#a7cdffcc"
                        onclick="setSwatch('107,67,255')"></button></div>
                <div><button class="swatch" style="background-color:#a7cdffcc"
                        onclick="setSwatch('107,67,176')"></button></div>
                <div><button class="swatch" style="background-color:#73b0ffcc"
                        onclick="setSwatch('107,67,100')"></button></div>
                <div><button class="swatch" style="background-color:#245fadcc" onclick="setSwatch('150,200,98')"></button>
                </div>
                <div><button class="swatch" style="background-color:#8c03db" onclick="setSwatch('192,255,93')"></button>
                </div>
                <div><button class="swatch" style="background-color:#9b0000" onclick="setSwatch('0,255,120')"></button>
                </div>
                <div><button class="swatch" style="background-color:#5bd0ff" onclick="setSwatch('136,255,255')"></button>
                </div>
                <div><button class="swatch" style="background-color:#1b17ff" onclick="setSwatch('167,255,166')"></button>
                </div>
                <div><button class="swatch" style="background-color:#00f064" onclick="setSwatch('81,255,255')"></button>
                </div>
                <div><button class="swatch" style="background-color:#009b27" onclick="setSwatch('81,255,168')"></button>
                </div>
                <div><button class="swatch" style="background-color:#00c89d" onclick="setSwatch('100,255,208')"></button>
                </div>
                <div><button class="swatch" style="background-color:#f0ff17" onclick="setSwatch('66,255,255')"></button>
                </div>
            </div>
        </div>
        <br>
        <div class="hsvSlidecontainer">
            <span class="unselectable" id="hue-text">Hue</span> <input id="hue" type="range" onmouseup="setColor(this.id)" ontouchend="setColor(this.id)" 
                min="0" max="255" value="0" class="hueSlider">
            <span class="unselectable" id="sat-text">Sat</span> <input id="sat" type="range" onmouseup="setColor(this.id)" ontouchend="setColor(this.id)"  
                min="0" max="255" value="255" class="slider">
            <span class="unselectable" id="bri-text">Bri</span><input id="bri" type="range" onmouseup="setColor(this.id)" ontouchend="setColor(this.id)"
                min="24" max="255" value="150" class="slider">
        </div>
        <br>
        <div class="aniContainer">
            <div class="center"><button class="button" onclick="setAnimation('1')">Color Streams</button></div>
            <div class="center"><button class="button" onclick="setAnimation('10')">Color Waves</button></div>
            <div class="center"><button class="button" onclick="setAnimation('2')">Red Raindrops</button></div>
            <div class="center"><button class="button" onclick="setAnimation('13')">Twinkle Stars</button></div>
            <div class="center"><button class="button" onclick="setAnimation('12')">Inchworm</button></div>
            <div class="center"><button class="button" onclick="setAnimation('3')">Analog Noise</button></div>
            <div class="center"><button class="button" onclick="setAnimation('4')">Blue Ocean</button></div>
            <div class="center"><button class="button" onclick="setAnimation('5')">Purple Ocean</button></div>
            <div class="center"><button class="button" onclick="setAnimation('11')">Red Ocean</button></div>
            <div class="center"><button class="button" onclick="setAnimation('6')">Scroll Color</button></div>
            <div class="center"><button class="button" onclick="setAnimation('8')">Left to Right</button></div>
            <div class="center"><button class="button" onclick="setAnimation('9')">Campfire</button></div>
            <div class="center"><button class="button" onclick="setAnimation('7')">Color Strobe</button></div> 
            <div class="center"><button class="button" onclick="setAnimation('-1')">Off</button></div>
        </div>
        <br>
    </div>
</body>

</html>
)rawliteral";


