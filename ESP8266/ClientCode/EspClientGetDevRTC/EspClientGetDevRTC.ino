/*  *****************************
 *   ESP-8266 NodeMcu v0.9-3.3v
 *  ***************************** 
 *  Dokumentacija modula in knjiĹľnjic: 
 *  http://esp8266.github.io/Arduino/versions/2.1.0/
 *  http://www.esp8266.com/wiki/doku.php?id=nodemcu
 *  
 *  OPIS:
 *  
 *  Koda poĹˇiljanja podatkov na pyhon server sedaj narejena za ESP-8266!
 *  Deluje drugaÄŤe in more biti tudi drugaÄŤe definirana! 
 *  https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide
 *  DrugaÄŤe popolnoma bazira na primeru iz ESP knjiĹľnjice WifiClient in pa 
 *  Ĺˇe delno iz primera na spletni strani od adafruita na prejĹˇnjem linku!
 *  DHT sezor deluje identiÄŤno kot pri arduinu kar piĹˇe tudi v dokumentaciji.
 *  
 *  BMP180 senzor za pritisk deluje brez problema samo z uporabo osnovne knjiĹľnjice
 *  vendar moraĹˇ pazit da priklopiĹˇ na pine D1->SCL,D2->SDA... saj tega v dokumentaciji ni.
 *  https://github.com/adafruit/Adafruit-BMP085-Library
 *  
 *  Celotna stvar deluje zelo podobno kot arduino z wifi shieldom le da se pini
 *  razlikujejo tam in tukaj ter jih mores pogledat v dokumentacijo! 
 *  
 *  Verzija 0.9 mora uporabljat pine kot so napisani na boardu(D1,D2...) ker so
 *  drugace dejasnko zapisani(piĹˇe v dokumentaciji.
 *  
 *  Vsec mi je da je povezava na internet dejansko enostavnejsa od Arduino ethernet
 *  Shielda in boljse deluje, brez nekih nepotrebnih dhcp-jev in MAC adres!
 *  
 *  Tukaj sploh nič ne prekinjamo povezave v kodi, kar skos je povezano na socket, nikoli ne prekinjamo v zanki te povezave!
 *  ***************************************************************************************
 *  DONE:
 *  
 *  Deluje nam celoten postopek poĹˇiljanja kot prej na arduinu in to kar zanesljivo.
 *  Senzor bere brez problema in tudi analogen vhod.
 *  
 *  Narediti je potrebno sedaj dejansko dato ki bo sla iz DHT-ja in vsako posebej
 *  potestirat.DHT lahko tece tudi na 3.3v ! tester deluje ! Analog read tistega
 *  photoresistorja tudi deluje dobro, cist enako delas kot na arduinotu.
 *  
 *  malo preoblikovat to kodo za poiljanje z name=value!... pa se dodat uro gor in 
 *  sezor za zrak. na protoboard vse nardit, ter kljucno prikazat podatke!  
 *  
 *  RTC deluje brez problema "out of the box" z examplom iz uradne knjiĹľnjice MAKUNA: https://github.com/Makuna/Rtc
 *  ceprav je povezan na ista vrata I2C kot tudi BMP 105. Dejansko deluje in tukaj vidimo delovanje samega
 *  I2C protokola kar lahko opisemo v porocilu. preglej primer in probaj skombinirat vse skup v protokol!
 *  Narejena je ta knjiĹľnjica tudi tako da deluje na ESP8266!
 *  Iz kje se ÄŤas kalibrira zmeda in zakaj ÄŤas malo zaostaja za pc-jem:
 *  http://arduino.stackexchange.com/questions/3354/why-is-my-real-time-clock-getting-the-wrong-time-from-my-pc
 *  
 *  TO DO:
 *  -Poglej ali bo delovalo sedaj tudi BMP vzporedno ko sta oba vezana no I2C vrata.
 *  -Ura se kalibrira s pomoÄŤjo __DATE__ in __TIME__, ki jih dobi iz raÄŤunalnika, te moramo mi 
 *  iz serverja s pomoÄŤjo http-ja dobiti in nastaviti! glej primer za pomoÄŤ!
 *  http://playground.arduino.cc/Code/DateTime
 *  -da bi na zacetku server vprasal za uro v setupu in bi jo potem imel vedno nastavljeno!!
 *  dobra ideja mogoce! delovalo tudi kot protokol!
 *  - testirat vectedensko aplikacijo in premislit ce bo mogoce smetal tist delay tam ker
 *    je rekel marko da ima esp vcasih probleme z delayem in je potrebno nadomestit ta del.
 *  *************************************************************************************
 *  OPOMBE:
 *  -server mora bit prizgan obvezno ob zacetku te kode saj si on uro glede na to nastavlja
 *  -tudi baza more biti prizgana da server dela za shranjevanje podatkov.
 */

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <Wire.h>  // must be incuded here so that Arduino library object file references work
#include <RtcDS3231.h>

// Settings that should be checked before starting
const char* ssid     = "Jozeko";
const char* password = "metlika1";

// IP adress where the server is
const char* server = "192.168.1.9";
// ID of our device
const char* device = "NodeMCUv0.9";

#define DHTPIN D4 //pin with the DHT
#define DHTTYPE DHT22 
#define countof(a) (sizeof(a) / sizeof(a[0]))

// global variables
float t;
float h;
float l;
float p;
const char* sendingTime;

RtcDS3231 Rtc;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

void setup() {
  dht.begin(); 
  bmp.begin(); 
  Serial.begin(115200);
  delay(10);
  //Connecting to Wifi network 
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

  //--------RTC SETUP ------------
  Rtc.Begin();
 /* 
  // Getting the time from the server!!
  // tukaj poteka protokol.
  String datee = httpRequest("/getDate");
  Serial.println(datee);
  String timee = httpRequest("/getTime");
  Serial.println(timee);
  // https://www.arduino.cc/en/Tutorial/StringRemove
  datee.remove(12);
  timee.remove(9);

  // PROBLEM konvercije stringa v char*
  // tukej imamo nek problem ker nam vn vrže, celotna povezava s serverjem in uro pa delaaaaaa!!!!
  // samo še sama ura nas nekej heca, ker čuden signal sprejema.(poglej konverzijo)
  //http://stackoverflow.com/questions/7383606/converting-an-int-or-string-to-a-char-array-on-arduino/7391187
  //https://www.arduino.cc/en/Reference/Char
  int dL=datee.length()+1;
  int tL=timee.length()+1;
  char Date[dL];
  char Time[tL];
  timee.toCharArray(Time,tL);
  datee.toCharArray(Date,dL);
  Serial.println(Date);
  Serial.println(Time); // podatki po pretvorbi */

  
  Serial.println(__DATE__);
  Serial.println(__TIME__); // primerjej ta dva zapisa in kaj je razlika
  // enostavno ga ne prebere lih pravilno (oblika je pomoje nepravilna ker __DATE__ deluje
  RtcDateTime compiled = RtcDateTime(__DATE__,__TIME__);
  // we create a need for time from the server so we can check if our RTC time is running correctly
  // removed the ifs for checking for problems so run the example to find them if any problems accur with the RTC!
  RtcDateTime now = Rtc.GetDateTime();
  //if (now < compiled) 
  //{
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
  //}
  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);  
}

int value = 0;

void loop() {
  delay(30000);
  ++value;
  
   // Reading the sensors
   h = dht.readHumidity(); // from 100%
   t = dht.readTemperature(); // Celsius
   l = analogRead(A0)/10.24; // from 100%
   p = bmp.readPressure(); // Pascal

   // delovanje RTCDateTime https://github.com/Makuna/Rtc/blob/master/src/RtcDateTime.h
   RtcDateTime now = Rtc.GetDateTime();
   measureTimeAndSend(now);

}


void measureTimeAndSend(const RtcDateTime& dt)
{

   char datestring[20];
           
   snprintf_P(datestring, 
   countof(datestring),
   PSTR("%02u/%02u/%04u-%02u:%02u:%02u"), // you should not use space here becouse it wont send it
   dt.Day(),         
   dt.Month(),
   dt.Year(),
   dt.Hour(),
   dt.Minute(),
   dt.Second() );
   sendingTime=datestring;
   Serial.println(datestring);
   // We now create a URI for the request
   // This is where all the data is inserted and here you can add more of it or remove it!
   // http://bottlepy.org/docs/dev/tutorial.html#query-variables
   String url = "/data?temperature="+String(t)+"&humidity="+String(h)+"&light="+String(l)+"&pressure="+String(p)+"&deviceid="+device+"&time="+String(sendingTime);
   Serial.println(httpRequest(url)); // to je absolutno obvezno da naredimo znotraj te funkcije saj je spremenljivka za cas nepravilnega formata, ta print sproti se aktivira funkcijo.
}

// the function for the GET request in:String of URI (very much used)
String httpRequest(String url)
{
  String line;
  
  Serial.print("connecting to ");
  Serial.println(server);
  
  // WifiClient class for setting up the TCP connection
  WiFiClient client;
  const int httpPort = 8080;
  if (!client.connect(server, httpPort)) {
    return("connection failed");
  }
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      return(">>> Client Timeout !");
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
     line = client.readStringUntil('\r');
  }

  
return(line); // returning the response we get from the server
}


