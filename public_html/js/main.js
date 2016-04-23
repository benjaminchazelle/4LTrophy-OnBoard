//refreshTimeSampling;
//refreshTime;
//max_length;
//series;
//GPIO;

function convertRad(input){
        return (Math.PI * input)/180;
}
 
function Distance(lat_a_degre, lon_a_degre, lat_b_degre, lon_b_degre){
     
        R = 6378000 //Rayon de la terre en mètre 
 
    lat_a = convertRad(lat_a_degre);
    lon_a = convertRad(lon_a_degre);
    lat_b = convertRad(lat_b_degre);
    lon_b = convertRad(lon_b_degre);
     
    d = R * (Math.PI/2 - Math.asin( Math.sin(lat_b) * Math.sin(lat_a) + Math.cos(lon_b - lon_a) * Math.cos(lat_b) * Math.cos(lat_a)))
    return d;
}


recordDuration_numero = {"1M" : 0, "15M" : 1, "1H" : 2, "4H" : 3};

refreshTimeout = parseInt(refreshTimeSampling*0.8);

map_initialized = false;

map = null;

current_page = 0;


function setGPIO(pin, value) {
	
	GPIO[pin] = value;
			
	var xhr = typeof XMLHttpRequest != 'undefined'
		? new XMLHttpRequest()
		: new ActiveXObject('Microsoft.XMLHTTP');
	xhr.open('get', "/api/gpio/" + pin + "-" + (value ? 1 : 0), true);
	xhr.timeout = refreshTimeout;
	xhr.onreadystatechange = function() {
		var status;
		var data;

		if (xhr.readyState == 4) {
			status = xhr.status;
			
			if (status == 200) {
			}
		}
	};
	xhr.send();
}



function setGPS(position) {

console.log("GPS LOCATION SENT");
	
	if(position != undefined) {
	
	
	var xhr = typeof XMLHttpRequest != 'undefined'
		? new XMLHttpRequest()
		: new ActiveXObject('Microsoft.XMLHTTP');
	xhr.open('get', "/api/setGPS/" + position.coords.latitude + "/" + position.coords.longitude, true);
	xhr.timeout = refreshTimeout;

	xhr.send();
	
	}
	

	
}


navigator.geolocation.watchPosition(setGPS, function () {}, {enableHighAccuracy:true, maximumAge:30000, timeout:2000});
	
function takePicture() {

	var xhr = typeof XMLHttpRequest != 'undefined'
		? new XMLHttpRequest()
		: new ActiveXObject('Microsoft.XMLHTTP');
	xhr.open('get', "/api/takePicture", true);
	xhr.timeout = refreshTimeout;
	
	xhr.onreadystatechange = function() {
		var status;
		var data;

		if (xhr.readyState == 4) {
			status = xhr.status;
			
			if (status == 200) {
				refreshPictures();
			}
		}
	};
	
	xhr.send();
	
	
	
}	


	
function getPictures(cb) {
	
console.log('getPictures');
	
	
	var xhr = typeof XMLHttpRequest != 'undefined'
		? new XMLHttpRequest()
		: new ActiveXObject('Microsoft.XMLHTTP');
	xhr.open('get', "/api/getPictures/" + current_page, true);
	xhr.timeout = refreshTimeout;
	xhr.onreadystatechange = function() {
		var status;
		var data;

		if (xhr.readyState == 4) {
			status = xhr.status;
			
			if (status == 200) {
				data = JSON.parse(xhr.response);
				
				document.getElementById("photo-page").innerHTML = "";
				
				if(data.pic[0]  != undefined)
					document.getElementById("lastPictureElement").src = data.pic[0];
				
				if(current_page > 0 && data.pic.length == 0)
					current_page--;
				
				for(var i in data.pic) {
					var uri = encodeURIComponent(data.pic[i] + "?" + Math.random());
					document.getElementById("photo-page").innerHTML +=	' \
							<div> \
								<img lowsrc="/images/load.png" onclick="document.getElementById(\'lastPictureElement\').src = \'/cache/'+uri+'\'" src="/cache/'+uri+'" /> \
								<p class="pictureSelecter"><button onclick="select(\'/cache/'+uri+'\')">Share</button></p> \
							</div> \
							';
				}
				
				if(cb != undefined) 
					cb();
				
			}
		}
	};
	xhr.send();
	
	
	
}

function refreshPictures () {
	console.log('refreshPictures');
		getPictures(function () {
			setTimeout(getPictures, 4000);
		});
}	


function length() {
	var textarea = document.getElementById('tweet');
	document.getElementById('tweetLength').innerHTML = textarea.value.length + (textarea.value.length > 1 ? " caractères" : " caractère") 
	
	if(textarea.value.length > 140)
		textarea.style.border = "2px solid red";
	else
		textarea.style.border = "2px solid #eceff1";
}

function resetTweet() {
	var textarea = document.getElementById('tweet');
	document.getElementById('sendPictureElement').src = "";
	textarea.value = "";
	length();
}

function select(src) {
	
	 document.getElementById('sendPictureElement').src = src;
	 
	 ptr_menu_items["#!/share"].click();
	 
	 document.getElementById('tweet').focus();
	 
	 document.getElementById('tweet').selectionStart = 0;
	 document.getElementById('tweet').selectionEnd = 0;
	
}

function sendTweet() {
	
	var textarea = document.getElementById('tweet');
	var img = document.getElementById('sendPictureElement');

	if(textarea.value == "" || img.getAttribute("src") == "") {
		alert("Erreur : Le tweet ne comporte pas d'image ou de texte");
		return;
	}
	
	var xhr = typeof XMLHttpRequest != 'undefined'
		? new XMLHttpRequest()
		: new ActiveXObject('Microsoft.XMLHTTP');
	xhr.open('get', "/api/sendTweet/" + encodeURIComponent(textarea.value) + "/" + encodeURIComponent(img.src), true);
	xhr.timeout = refreshTimeout;
	
	xhr.onreadystatechange = function() {
		var status;
		var data;

		if (xhr.readyState == 4) {
			status = xhr.status;
			
			if (status == 200) {
				alert("Le tweet vole de ses propres ailes ! ~~");
				resetTweet();
			}
		}
	};
	
	xhr.send();
	
}
	
window.onload = function() {
	
	//Gestion du menu
	var menu_items = document.getElementsByTagName("ASIDE")[0].getElementsByTagName("A");
	ptr_menu_items = {};
	
	for(var i = 0; i < menu_items.length; i++) {
		
		ptr_menu_items[menu_items[i].hash] = menu_items[i];
		
		//menu_items[i].parentNode.className = (menu_items[i].hash == hash) ? "selected" : "";
		menu_items[i].onclick = function () {
			
			var hash = this.hash;
			

			
			var sections = document.getElementsByTagName("SECTION");
			
			var sectionName = hash.substring(3);
			
			var section = document.getElementById(sectionName);

			if(section) {
				
				for(var i = 0; i < sections.length; i++) {
					
					sections[i].style.display = 'none';
				}
			
				
				section.style.display = 'block';
				
				if(hash == "#!/map" && !map_initialized) {
					map_initialized = true;
					map = L.map('map-inner').setView([47, 2], 10); //France
					
					// L.tileLayer('http://otile1.mqcdn.com/tiles/1.0.0/osm/{z}/{x}/{y}.jpg', {
					L.tileLayer('/tiles/{z}/{x}/{y}.png', {
						attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
					}).addTo(map);
					
					map.on('dragstart', function(e) {
						
						recenter_map = false;
						
						clearTimeout(recenter_mapTimer);
						recenter_mapTimer = setTimeout(function () {recenter_map = true;}, 15000);
						
					});
				}
				
				for(var j = 0; j < menu_items.length; j++) {
					menu_items[j].parentNode.className = (menu_items[j].hash == hash) ? "selected" : "";
				}
			}
			
		};
			
	}
	
	if(window.location.hash in ptr_menu_items)
		ptr_menu_items[window.location.hash].onclick();
	
	//GPIO
	/*	document.getElementById("gpio6").onclick = function () {

			setGPIO(1, !GPIO[1]);
			
		};
	*/
	//On formate les séries pour utilisation en graphique
	formated_series = {};

	for( var s in series) {
		formated_series[s] = {};
		
		for(var t in recordDuration_numero) {
			
			formated_series[s][t] = [];
			
			for(var i in series[s][t]) {
				formated_series[s][t].push({x : new Date(series["clock"][t][i]), y : series[s][t][i], s : false});
			}
		}
	}
	
	

	//Création de la map
	
	
	recenter_map = true;
	recenter_mapTimer = null;
	
	map_lastTimePosition = null;
	
	marker = null;



	//Création des graphiques
	charts = {};

	for(var n in nameCollectionModel) {
		var s = nameCollectionModel[n];
		
		var id = "chart_" + s;

		if(document.getElementById(id)) {

			var chartOption = {
				title : {},
				axisX : {},
				data : []
				};

			for(var t in recordDuration_numero) {

				var visible = recordDuration_numero[t] == 0;

				chartOption.data.push({
					type: "line",
					markerType : "none",
					visible : visible,
					dataPoints : formated_series[s][t]
					});

				}	
			
			charts[s] = new CanvasJS.Chart(id, chartOption);
			charts[s].render();
			}
		}

	//Rafraichissement des données
	last_update = ((series["clock"]["1M"].length-1) in series["clock"]["1M"]) ? series["clock"]["1M"][series["clock"]["1M"].length-1] : 0;
	
	function dataRefresh() {
		
		var xhr = typeof XMLHttpRequest != 'undefined'
			? new XMLHttpRequest()
			: new ActiveXObject('Microsoft.XMLHTTP');
		xhr.open('get', "/api/update/" + last_update, true);
		xhr.timeout = refreshTimeout;
		xhr.onreadystatechange = function() {
			var status;
			var data;

			if (xhr.readyState == 4) {
				status = xhr.status;
				
				if (status == 200) {
					
					var res_update = JSON.parse(xhr.response);
					
					var new_update = res_update["ADC"];
					/*GPIO = res_update["GPIO"];

					//Mise à jour de l'index de dernier mise à jour
					if(new_update["clock"]["1M"].length > 0)
						last_update = new_update["clock"]["1M"][new_update["clock"]["1M"].length-1];

					//Pour chaque série et chaque période de stockage
					for(var s in series) {
						
						for(var t in recordDuration_numero) {

							//Si la dernière donnée dans le grahique est un échnatillon, on le supprime
							if(formated_series[s][t][formated_series[s][t].length-1].s == true) {
								formated_series[s][t].pop();
								}

							//On met à jour les graphiques avec les dernières données
							for(var i in new_update[s][t]) {

								if(formated_series[s][t].length > dataCollectionModel[t].max_length)
									formated_series[s][t].shift();

								formated_series[s][t].push( {x : new Date(new_update["clock"][t][i] ), y : new_update[s][t][i], s : false } );
								}	

							//Si on travaille des données actuellement affichées
							if(s in charts && charts[s].options.data[recordDuration_numero[t]].visible) {

								//On rédéfinit xMin et xMax
								charts[s].options.axisX.minimum = formated_series[s][t][0].x;
								charts[s].options.axisX.maximum = new Date(charts[s].options.axisX.minimum.getTime() + (formated_series[s][t].length)*dataCollectionModel[t].refreshTime);

								//Si lors de ce rafraichissement, on a reçu aucun donnée, on place une donnée échantillon en fin de graphique
								if(new_update["clock"][t].length == 0){
									formated_series[s][t].push( {x : charts[s].options.axisX.maximum, y : new_update[s].sample, s : true } );
									}					

								charts[s].render();
								}
							}
						}
					*/	
						
						
					//GPS Update
					
					if(map != null && (new_update["GPSLat"].sample != 0 || new_update["GPSLng"].sample != 0)) {

						if(marker == null)
							marker = L.marker([new_update["GPSLat"].sample, new_update["GPSLng"].sample]).addTo(map);
						else {
							
							var a = marker.getLatLng();
							
							if(map_lastTimePosition != null) {
								
								//console.log(map_lastTimePosition);
								//console.log(new_update["clock"].sample);
								var time = (new_update["clock"].sample - map_lastTimePosition) / 1000;
								
								var distance = Distance(a.lat, a.lng, new_update["GPSLat"].sample, new_update["GPSLng"].sample);

								var vitesse = ( distance / time ) * 3.6;
								
								document.getElementById("speed").innerHTML = vitesse.toFixed(1) + " km/h"
								}
							
								map_lastTimePosition = new_update["clock"].sample;
								
								marker.setLatLng([new_update["GPSLat"].sample, new_update["GPSLng"].sample]);

								if(recenter_map) {
										map.setView([new_update["GPSLat"].sample, new_update["GPSLng"].sample]);						
								}
							}	
						}
					} 
					
					
				setTimeout(dataRefresh, refreshTimeSampling);
				}
			};
		xhr.send();
		}

	dataRefresh();
	
	//Gestion des contrôles de temps
	var controls = document.getElementsByClassName("control");
	
	for(var i=0; i < controls.length; i++) {
		controls[i].onchange = function (e) {
			
			var s = e.target.name;
			
			for(var ut in charts[e.target.name].options.data) {
				charts[e.target.name].options.data[ut].visible = false;
				}
				
			var t = e.target.value;
			
			charts[s].options.data[recordDuration_numero[t]].visible = true;

			charts[s].options.axisX.minimum = formated_series[s][t][0].x;
			charts[s].options.axisX.maximum = new Date(charts[s].options.axisX.minimum.getTime() + (formated_series[s][t].length)*dataCollectionModel[t].refreshTime);
			charts[s].render();
			
			};
			
		controls[i].value = "1M";
		}
		
	resetTweet();

	getPictures();
	};
	
