const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
<head>
<style>
button {
  background-color: #008CBA;
  border: none;
  color: white;
  padding: 15px 24px;
  border-radius: 4px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 14px;
  margin: 4px 2px;
  cursor: pointer;
}

button:hover {background-color: #3e8e41}

button.button1 {
	border-radius: 50%;
    background-color: #f44336;
}

h1, h2, h3, h4, h5, h6 {
  line-height: 1;
  margin-top: 5px;
  margin-bottom: 5px;
}

p.space {
  margin-top: 28px;
  margin-bottom: 5px;
} 

</style>
</head>
<body>
    <h1>Grand Theft Autonomous</h1>
    <h4><i>Team 09</i></h4>
    <p class="space"><font size="4"><strong>Set Mode:</strong></font></p>
  	<button type="button" onclick="wallFollow()" id="wallFollow_button">Wall Follow </button>
  	<button type="button" onclick="pushPoliceCar()" id="pushPoliceCar_button">Push Police Car </button>
  	<button type="button" onclick="trackTrophy()" id="trackTrophy_button">Track Trophy </button>
  	<button type="button" onclick="trackFakeTrophy()" id="trackFakeTrophy_button">Track Fake Trophy </button>
    <button type="button" onclick="stop()" id="STOP_button" class="button1">STOP </button>  	
    <p><font size="4"><strong>Current Mode: </strong><span id="buttonMode">N/A</span></font></p>

</body>
<script>
        function wallFollow() {
          document.getElementById('buttonMode').innerText = "Following Wall";
          var xhr = new XMLHttpRequest();
          updateMode(1);
          xhr.send();
        }
        function pushPoliceCar() {
          document.getElementById('buttonMode').innerText = "Pushing Police Car";
          var xhr = new XMLHttpRequest();
          updateMode(2);
          xhr.send();
        }
        function trackTrophy() {
          document.getElementById('buttonMode').innerText = "Tracking Trophy";
          var xhr = new XMLHttpRequest();
          updateMode(3);
          xhr.send();
        }
        function trackFakeTrophy() {
          document.getElementById('buttonMode').innerText = "Tracking Fake Trophy";
          var xhr = new XMLHttpRequest();
          updateMode(4);
          xhr.send();
        }
        function stop() {
          document.getElementById('buttonMode').innerText = "";
          var xhr = new XMLHttpRequest();
          updateMode(0);
          xhr.send();          
        }

        function updateMode(val) {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/mode?value=" + val, true);
          xhr.send();
        }
        
</script>
</html>
)===";
