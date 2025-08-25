const char jsfile[] PROGMEM = R"rawliteral(
        function ToggleLED(ledNumber) {
            const btn = document.getElementById("toggleBtn" + ledNumber);
            const currentState = btn.classList.contains("on") ? 1 : 0;
            const newState = currentState === 1 ? 0 : 1;
            fetch(`/ToggleLED?led=${ledNumber}&state=${newState}`)
            .then(response => response.json())
            .then(data => updateUI(data.led, data.state));
        };
        function updateUI(ledNum, state) {
            const indicator = document.getElementById(`indicator${ledNum}`);
            const button = document.getElementById(`toggleBtn${ledNum}`);

            if (state === 1) {
                indicator.classList.add('on');
                button.classList.remove('off');
                button.classList.add('on');
                button.textContent = 'Turn Off';
            } else {
                indicator.classList.remove('on');
                button.classList.remove('on');
                button.classList.add('off');
                button.textContent = 'Turn On';
            }
        };
        fucntion gettemp(){
        const temp=document.getElementById(`temperatureline`);
        const hum=documen.getElementById(`humidityline`);
        fetch(`/gettemp`).then(response=>response.json()).then(data=> temp.textContent=`Temperature: ${data.temp}`);
        fetch(`/gethum`).then(response=>response.json()).then(data=> hum.textContent=`Humidity: ${data.hum}`);
        }
        window.onload = function() {
            btn1=document.getElementById("toggleBtn1");
            btn2=document.getElementById("toggleBtn2");
            btn3=document.getElementById("toggleBtn3");
            btn1.addEventListener("click", () => ToggleLED(1));
            btn2.addEventListener("click", () => ToggleLED(2));
            btn3.addEventListener("click", () => ToggleLED(3));
            // Start fetching states periodically
            fetchStates();
            setInterval(fetchStates, 2000);
        };
        setInterval(gettemp,1000);
)rawliteral";