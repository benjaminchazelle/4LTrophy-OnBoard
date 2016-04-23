isWin = /^win/.test(process.platform);

var static = require('node-static');
var child_process = require('child_process');
var spawn = require('child_process').spawn;
var request = require('request'); var requestHelper = request;
var fs = require('fs');
var Twitter = require('twitter');
var im = isWin ? null : require('imagemagick');
 
client = new Twitter({
  consumer_key: '',
  consumer_secret: '',
  access_token_key: '',
  access_token_secret: ''
});

GoProPassword = "";

rmDir = function(dirPath) {
  try { var files = fs.readdirSync(dirPath); }
  catch(e) { return; }
  if (files.length > 0)
	for (var i = 0; i < files.length; i++) {
	  var filePath = dirPath + '/' + files[i];
	  if (fs.statSync(filePath).isFile())
		fs.unlinkSync(filePath);
	  else
		rmDir(filePath);
	}
  fs.rmdirSync(dirPath);
};
	
rmDir("/public_html/cache");

console.log("Initialisation du modèle...")

cache = [];

pictures = [];

dataCollectionModel = {
			"1M" : {"delay" : 60*1000, "refreshTime" : 1000}, //rafraichissement toutes les secondes, stockées sur 1min
			"15M" : {"delay" : 15*60*1000, "refreshTime" : 5000},
			"1H" : {"delay" : 60*60*1000, "refreshTime" : 15000},
			"4H" : {"delay" : 4*60*60*1000, "refreshTime" : 30000}
			};

for(t in dataCollectionModel) {
	dataCollectionModel[t].max_length = dataCollectionModel[t].delay / dataCollectionModel[t].refreshTime;
}

//Construction de l'objet de stockage des séries de données
nameCollectionModel = ["ADC0", "ADC1", "ADC2", "ADC3", "GPSLat", "GPSLng"];
nameCollectionModel.push("clock");

series = {};
for(var i = 0; i < nameCollectionModel.length; i++) {
	series[nameCollectionModel[i]] = {};
	for(t in dataCollectionModel) {
		series[nameCollectionModel[i]][t] = [];
	}
}

//Initialisation de l'objet d'échantillonage 
samples = {};
for(var _s in series) {
	samples[_s] = 0;
}

currentGPSLat = 0;
currentGPSLng = 0;

//Période de rafraichissement pour la construction des séries 
refreshTimeSampling = 1000;

function dataSampling() { //Construire les séries d'échantillonage selon le durée à partir de l'objet d'échantillonage
	
	var time = new Date().getTime();
	
	if(samples.clock != 0) { //Si l'acquisition a commencé
		
		//Pour chaque période d'échantillonage
		for(var t in dataCollectionModel) {
			
			//Si aucune donnée sur la période d'échantillonage ou si la dernière donnée est obselète 
			if( (series["clock"][t].length == 0 || time-series["clock"][t][series["clock"][t].length-1] >= dataCollectionModel[t].refreshTime) ) {
				
				//Pour chaque type de donnée
				for(var s in series) {
					
					//On enlève la première donnée si la série a atteint sa taille maximum
					if(series[s][t].length  > dataCollectionModel[t].max_length-1)
						series[s][t].shift();
					
					//On ajoute à la fin la dernière donnée présente dans l'objet d'échantillonage
					series[s][t].push(samples[s]);	

					
					}
				
				}

			}
			
		// console.log("Data sampled");
		}
	
	setTimeout(dataSampling, refreshTimeSampling);
	
}

//Initialisation GPIO
GPIO = {};

//Photo
var request = require('request');
var fs = require('fs');

function copyPicture(from, to, cb) {
	
	if(to == undefined)
		to = 'lastPicture.jpg';
	
	if(cb != undefined)
		request(from, cb).pipe(fs.createWriteStream(to))
	else
		request(from).pipe(fs.createWriteStream(to))
	
}

function refreshPictures(copy) {
	console.log('refreshPictures');
	request("http://10.5.5.9:8080/DCIM/100GOPRO/", function (error, response, body) {
		// console.log(error, response, body);
		if (error || response.statusCode != 200)
				return;

		var ancres = body.split('<a class="link" href="');
		pictures = [];
		
		for(var i in ancres) {
			if(i > 0)
				pictures.unshift("http://10.5.5.9:8080/DCIM/100GOPRO/" + ancres[i].split('"')[0]);
			}
			
		// pictures = pictures.slice(0);

			
		if(copy != undefined && copy == true)
			copyPicture(pictures[pictures.length-1]);
		
		});
	
}

refreshPictures();

function takePicture(step) {
	// console.log(step, "tP");
	
	if(step == undefined)
		step = 0;
	
	var urls = 	[
			"http://10.5.5.9:80/camera/CM?t=" + GoProPassword + "&p=%01",	//Photo mode
			"http://10.5.5.9:80/camera/PV?t=" + GoProPassword + "&p=%00",	//Preview disabled
			"http://10.5.5.9:80/camera/PR?t=" + GoProPassword + "&p=%04",	//Photo resolution set
			"http://10.5.5.9:80/bacpac/SH?t=" + GoProPassword + "&p=%01",	//Shot
			];

	if(step < urls.length) {
		// console.log(urls[step], "...");	
		request(urls[step], function (error, response, body) {
			// console.log(urls[step], "OK!");
			if (error || response.statusCode != 200)
					return;
			 
			if(step == 3)
				setTimeout(refreshPictures, 2000, true)
			else
				setTimeout(takePicture, 200, step+1)
			
			});
	
	}

}

function pictureLoop () {
	
	takePicture();
	
	setTimeout(pictureLoop, 60*5*1000);
	
}

pictureLoop();

//Processus d'acquisition des données	
gpioSpawn	= isWin ? child_process.spawn('python', ['./gpio.py']) : child_process.spawn('./GPIO', []);
gpioSpawn.stdout.on('data', function () {

	for(var i=1;i<=8;i++)
		GPIO[i] = false;
	
	console.log("Initialisation des processus...")
	
	acquistionSpawn	= isWin ? child_process.spawn('python', ['./acquisition.py']) : child_process.spawn('./ADC', []);

	acquistionSpawn.stdout.on('data', function (data) { //À chaque acquisition de données analogiques
		
		//Lire l'enregistrement pour l'exporter vers l'objet d'échantillonage 
		
		var raw = data.toString();
		
		var entries = raw.split("\n");
		
		for(var i in entries) { //Pour chaque ligne de donnée
		
			//Séparer le nom de la donnée et de sa valeur
			
			var key_value = entries[i].trim().split(":");
			
			if(key_value.length == 2) {
				
				//Mettre à jour l'objet d'échantillonage
				if(key_value[0] in samples)
					samples[key_value[0]] = parseFloat(key_value[1]);
				
				}
			}
			
		samples.GPSLat = currentGPSLat;
		samples.GPSLng = currentGPSLng;
			
		samples.clock = new Date().getTime();

		//cache.push({"time" :  Math.ceil(samples.clock/1000), "GPSlat" : samples.GPSLat, "GPSlng": samples.GPSLng, "temperature_interne" : samples.ADC0, "temperature_externe" : samples.ADC1});
		
		if(samples.GPSLat != 0 || samples.GPSLng != 0)
			cache.push({"time" :  Math.ceil(samples.clock/1000), "GPSlat" : samples.GPSLat, "GPSlng": samples.GPSLng});
		console.log(8);
		//request.post({url:'http://localhost/iut/4l/distant/gateway.php?password=BORIS', formData: {"data" : JSON.stringify(cache)}}, function (error, response, body) {
		request.post({url:'http://justwoodit.fr/live/gateway.php?password=BORIS', formData: {"data" : JSON.stringify(cache)}}, function (error, response, body) {
		  // console.log(body)
			if (!error && response.statusCode == 200) {
				
				cache = [];
				}
			});	

		});

	dataSampling();

	var Server = new static.Server('./public_html/');

	require('http').createServer(function (request, response) {
		request.addListener('end', function () {

			if(request.url.search("/api/update/") == 0) { //Renvoit les dernières données acquises (classées par types de données et par périodes d'échantillonage) depuis un timestamp
			
				response.writeHead(200, {  "Content-Type": "application/json; charset=utf-8",  "Access-Control-Allow-Origin": "*"});
				// console.log("\nAPI Requested");

				//Récupération du timestamp passé en argument dans l'URL
				var last_update = request.url.substring("/api/update/".length);

				last_update = (last_update != "") ? parseInt(last_update) : 0;
				
				//Création de l'objet de retour
				var new_update = {};
				
				
				
				for(var s in series) {
					//new_update[s] = {"1M" : [], "15M" : [], "1H" : [], "4H" : []};
					new_update[s] = {};
					for(var t in dataCollectionModel) {
						new_update[s][t] = [];
					}
				}	
				
				//Pour chaque durée d'échantillonage "t"
				
				for(var t in new_update["clock"]) {
					
					var last_index = 0;
					
					//Rechercher l'index d'échantillonage à partir duquel il existe des nouvelles données
					
					for(var i = series["clock"][t].length-1 ; i >= 0 ; i--) {
						
						if(series["clock"][t][i] <= last_update) {
							last_index = i;
							break;
							}
						}

					//Mettre à jour chaque série de l'objet de retour sur la durée d'échantillonage "t" à partir du dernier index à jour

					for(var i = last_index + 1; i < series["clock"][t].length ; i++) {
						
						for(var s in series) {

							new_update[s][t].push(series[s][t][i]);
							
						}
						
					}
					
				}
				
				//Ajout des valeurs de l'objet d'échantillonage à l'objet de mise à jour
				for(var s in series) {
					new_update[s].sample = samples[s];
				}
				
				var res_update = {"ADC" : new_update, "GPIO" : GPIO};

				//Renvoyer l'objet de mise à jour
				response.end(JSON.stringify(res_update));
				}
			else if(request.url.search("/api/gpio/") == 0) { //gpio
			
				var GPIO_pinvalue = request.url.substring("/api/gpio/".length).split("-");

				if(GPIO_pinvalue.length == 2) {
					
					var pin = parseInt(GPIO_pinvalue[0]);
					var value = parseInt(GPIO_pinvalue[1]) == 1;
					
					GPIO[pin] = value;
					
					var GPIO_state = "";
					for(i in GPIO) {
						GPIO_state = (GPIO[i] ? "1" : "0") + GPIO_state;
					}
					
					if(!isWin)
						child_process.spawn('./GPIO', [GPIO_state]);

				}
			
			

				
			}

			else if(request.url.search("/cache/") == 0 && request.url.split('/').length == 3) { //cache
			
				var filename = request.url.split('/')[2].split('%3F0')[0];
				var url = decodeURIComponent(filename);
				var path = 'public_html/cache/'+filename;
				
				if(fs.existsSync(path)) {
					response.writeHead(200, {'Content-Type': 'image/jpeg'});
					
					setTimeout(function(){fs.unlink(path);}, 45*60*1000);
					
					response.end(fs.readFileSync(path));					
				}
				else {
					requestHelper(url, function () {
						
						
						response.writeHead(200, {'Content-Type': 'image/jpeg'});
						response.end(fs.readFileSync(path));
						
						if(im != null) {
							im.resize({
							  srcPath: path,
							  dstPath: path,
							  height:   1024,
							  width:   768
							}, function(err, stdout, stderr){
							  if (err) throw err;
							});
						}
						
					}).pipe(fs.createWriteStream(path));
				}
				// console.log("chache", url)
				// request(from, cb).pipe(fs.createWriteStream(to))
			

				}
			else if(request.url.search("/api/import") == 0) { //importation des séries de données 
				response.writeHead(200, {'Content-Type': 'text/javascript'});
				response.end("refreshTimeSampling = " + refreshTimeSampling + "; dataCollectionModel = " + JSON.stringify(dataCollectionModel) + "; nameCollectionModel = " + JSON.stringify(nameCollectionModel) + "; series = " + JSON.stringify(series) + "; GPIO = " + JSON.stringify(GPIO) + ";");
				}
			else if(request.url.search("/api/sendTweet/") == 0 && request.url.split('/').length == 5) { // tweeter
				
				var txt = decodeURIComponent(request.url.split('/')[3]);
				var img = decodeURIComponent(request.url.split('/')[4]);

				copyPicture(img, "tweet.jpg", function () {
					
					var data = require('fs').readFileSync('tweet.jpg');

					// Make post request on media endpoint. Pass file data as media parameter
					client.post('media/upload', {media: data}, function(error, media, response){

					  if (!error) {

						// If successful, a media object will be returned.
						// console.log(media);

						// Lets tweet it
						var status = {
						  status: txt,
						  lat: currentGPSLat,
						  long: currentGPSLng,
						  display_coordinates: true,
						  media_ids: media.media_id_string // Pass the media id string
						}

						client.post('statuses/update', status, function(error, tweet, response){
						  if (!error) {
							console.log(response);
							console.log(tweet);
						  }
						});

					  }
					});
					
				});

				


				response.writeHead(200, {'Content-Type': 'text/plain'});
				response.end("Photo taken");
				}
			else if(request.url.search("/api/takePicture") == 0) { // prendre une photo
				console.log('Photo taken');
				takePicture();

				response.writeHead(200, {'Content-Type': 'text/plain'});
				response.end("Photo taken");
				}
			else if(request.url.search("/api/getPictures/") == 0 && request.url.split('/').length == 4) { // prendre une photo
				
				refreshPictures();
				
				console.log("$$$" + request.url.split('/')[3]);
				
				var p = request.url.split('/')[3];
				
				var back = {length : pictures.length, pic : pictures.slice(4*p, 4*p+4)};

				response.writeHead(200, {'Content-Type': 'application/json; charset=utf-8'});
				response.end(JSON.stringify(back));
				}
			else if(request.url.search("/api/setGPS") == 0 && request.url.split('/').length == 5) { //definir position GPS
				
				currentGPSLat = request.url.split('/')[3];
				currentGPSLng = request.url.split('/')[4];

				response.writeHead(200, {'Content-Type': 'text/plain'});
				response.end("GPS location set");
				}
			else if(request.url.search("/api") == 0) { //présentation de l'API
				response.writeHead(200, {'Content-Type': 'text/plain'});
				response.end("API 1.0\n/api/update/{time}\n/api/import");
				}
			else if(request.url.search("/manage/deleteAll") == 0) { //présentation de l'API
			
				requestHelper("http://10.5.5.9/camera/DA?t=" + GoProPassword + "&p=");
			
				response.writeHead(200, {'Content-Type': 'text/plain'});
				response.end("All picture deleted");
				}
			else {
				Server.serve(request, response);
				}
			
			}).resume();
		}).listen(8081);

	console.log("Server started\n");

	});