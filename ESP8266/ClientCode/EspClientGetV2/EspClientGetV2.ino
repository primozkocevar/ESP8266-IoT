/*  *****************************
 *   ESP-8266 NodeMcu v0.9-3.3v
 *  ***************************** 
 *  Dokumentacija modula in knjižnjic: 
 *  http://esp8266.github.io/Arduino/versions/2.1.0/
 *  http://www.esp8266.com/wiki/doku.php?id=nodemcu
 *  
 *  OPIS:
 *  
 *  Koda pošiljanja podatkov na pyhon server sedaj narejena za ESP-8266!
 *  Deluje drugače in more biti tudi drugače definirana! 
 *  https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide
 *  Drugače popolnoma bazira na primeru iz ESP knjižnjice WifiClient in pa 
 *  še delno iz primera na spletni strani od adafruita na prejšnjem linku!
 *  DHT sezor deluje identično kot pri arduinu kar piše tudi v dokumentaciji.
 *  
 *  BMP180 senzor za pritisk deluje brez problema samo z uporabo osnovne knjižnjice
 *  vendar moraš pazit da priklopiš na pine D1->SCL,D2->SDA... saj tega v dokumentaciji ni.
 *  https://github.com/adafruit/Adafruit-BMP085-Library
 *  
 *  Celotna stvar deluje zelo podobno kot arduino z wifi shieldom le da se pini
 *  razlikujejo tam in tukaj ter jih mores pogledat v dokumentacijo! 
 *  
 *  Verzija 0.9 mora uporabljat pine kot so napisani na boardu(D1,D2...) ker so
 *  drugace dejasnko zapisani(piše v dokumentaciji.
 *  
 *  Vsec mi je da je povezava na internet dejansko enostavnejsa od Arduino ethernet
 *  Shielda in boljse deluje, brez nekih nepotrebnih dhcp-jev in MAC adres!
 *  ***************************************************************************************
 *  DONE:
 *  
 *  Deluje nam celoten postopek pošiljanja kot prej na arduinu in to kar zanesljivo.
 *  Senzor bere brez problema in tudi analogen vhod.
 *  
 *  Narediti je potrebno sedaj dejansko dato ki bo sla iz DHT-ja in vsako posebej
 *  potestirat.DHT lahko tece tudi na 3.3v ! tester deluje ! Analog read tistega
 *  photoresistorja tudi deluje dobro, cist enako delas kot na arduinotu.
 *  
 *  malo preoblikovat to kodo za poiljanje z name=value!... pa se dodat uro gor in 
 *  sezor za zrak. na protoboard vse nardit, ter kljucno prikazat podatke!  
 *  
 *  TO DO:
 *  
 *  dodat uro zunanjo
 *  *************************************************************************************
 */

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

const char* ssid     = "Jozeko";
const char* password = "metlika1";

// IP adress where the server is currently
const char* server = "192.168.1.4";

#define DHTPIN D4 //pin s to oznako je na DHT-ju
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

void setup() {
  dht.begin(); 
  bmp.begin(); 
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(30000);
  ++value;
  
  // Reading sensors here
  float h = dht.readHumidity(); // from 100%
  float t = dht.readTemperature(); // Celsius
  float l = analogRead(A0)/10.24; // from 100%
  float p = bmp.readPressure(); // Pascal


  Serial.print("connecting to ");
  Serial.println(server);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8080;
  if (!client.connect(server, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/data?temperature="+String(t)+"&humidity="+String(h)+"&light="+String(l)+"&pressure="+String(p);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

