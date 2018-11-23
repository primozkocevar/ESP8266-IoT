/*  *****************************
 *   ESP-8266 NodeMcu v0.9-3.3v
 *  ***************************** 
 *  Dokumentacija modula in knjiĹľnjic: 
 *  http://esp8266.github.io/Arduino/versions/2.1.0/
 *  http://www.esp8266.com/wiki/doku.php?id=nodemcu
 *  
 *  Dodatki dokumentacije:
 *  https://github.com/adafruit/Adafruit-BMP085-Library
 *  https://github.com/Makuna/Rtc
 *  https://github.com/PaulStoffregen/Time
 *  
 *  OPIS:
 * 
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
 *  
 *  Tukaj sploh nič ne prekinjamo povezave v kodi, kar skos je povezano na socket, nikoli ne prekinjamo v zanki te povezave!
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
 *  RTC verzija:
 *  RTC deluje brez problema "out of the box" z examplom iz uradne knjižnjice MAKUNA: https://github.com/Makuna/Rtc
 *  ceprav je povezan na ista vrata I2C kot tudi BMP 105. Dejansko deluje in tukaj vidimo delovanje samega
 *  I2C protokola kar lahko opisemo v porocilu. preglej primer in probaj skombinirat vse skup v protokol!
 *  Narejena je ta knjižnjica tudi tako da deluje na ESP8266!
 *  Iz kje se čas kalibrira zmeda in zakaj čas malo zaostaja za pc-jem:
 *  http://arduino.stackexchange.com/questions/3354/why-is-my-real-time-clock-getting-the-wrong-time-from-my-pc
 *  
 *  TP verzija:
 *  Sedaj za TP bomo pa naredili primer s softwaresko uro kjer bo glavna uporaba uradne knjižnjice
 *  za čas pri arduinu: https://github.com/PaulStoffregen/Time, brez RTC-ja bomo naredili! delamo s pomočjo
 *  primera iz NTP nastavljanje ure. NTP primer sledimo totalno pri kreaciji!
 *    - da bi na zacetku server vprasal za uro v setupu in bi jo potem imel vedno nastavljeno!!
 *    dobra ideja mogoce! delovalo tudi kot protokol!
 *  
 *  ******************************************************************************************
 *  TO DO:
 *  - testirat vectedensko aplikacijo in premislit ce bo mogoce smetal tist delay tam ker
 *  *************************************************************************************
 *  OPOMBE:
 *  - server mora bit prizgan obvezno ob zacetku te kode saj si on uro glede na to nastavlja
 *  - tudi baza more biti prizgana da server dela za shranjevanje podatkov.
 *  
 *  @author: PrimozK
 */

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <TimeLib.h>

// Settings that should be checked before starting
const char* ssid     = "Jozeko";
const char* password = "metlika1";

// IP adress where the server is (it is maybe good to set a static IP on the server)
const char* server = "192.168.1.9";
// ID of our device
const char* device = "NodeMCUv0.9";

unsigned long delayTime= 20000;

#define DHTPIN D4 //pin with the DHT
#define DHTTYPE DHT22 
// global variables
float t;
float h;
float l;
float p;
const char* sendingTime;

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

  // Setting the time with the UNIX from the function get_time!
  setSyncProvider(get_time);
  // Sync interval for syncing the internal clock in seconds
  setSyncInterval(300);

}

int value = 0;
void loop() {
  delay(delayTime);
  ++value;
  
   // Reading the sensors
   h = dht.readHumidity(); // from 100%
   t = dht.readTemperature(); // Celsius
   l = analogRead(A0)/10.24; // from 100%
   p = bmp.readPressure(); // Pascal
   
   // Create a time string from our setSyncProvider!
   String sendingTime = String(day())+"/"+String(month())+"/"+String(year())+"-"+String(hour())+":"+String(minute())+":"+String(second());
   Serial.println(sendingTime);
   
   // We now create a URI for the request
   // This is where all the data is inserted and here you can add more of it or remove it!
   // http://bottlepy.org/docs/dev/tutorial.html#query-variables
   String url = "/data?temperature="+String(t)+"&humidity="+String(h)+"&light="+String(l)+"&pressure="+String(p)+"&deviceid="+device+"&time="+String(sendingTime);
   data(url);
   // Serial.println(httpRequest(url)); // to je absolutno obvezno da naredimo znotraj te funkcije saj je spremenljivka za cas nepravilnega formata, ta print sproti se aktivira funkcijo.
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


// time has to be written and returned in UNIX format! https://en.wikipedia.org/wiki/Unix_time
time_t get_time()
{
  // http://forum.arduino.cc/index.php?topic=50834.0
  // Convert string to char so we can convert it to unsigned long which we need for return!
  String timee = httpRequest("/getDatetime");
  if( timee.length() > 15){
    return(now());
  }
// ta if zanka nam omogoči izpad serverja, in da nam ura še vedno teče tudi poleg tega da server ne dela!
// vbistvu da se nam ura ne pokvari tudi če enkrat pokličemo get_time ko nismo povezani!
  
  int tL=timee.length()+1;
  char Time[tL];
  timee.toCharArray(Time,tL);
  // http://www.gdsw.at/languages/c/programming-bbrown/c_068.htm
  // converting from chae to unsigned long!
  unsigned long unixTime = atoi(Time);
  // plus two hours becouse of the time zone!
  return unixTime + 60*60*2;

}

void data(String urii){
Serial.println(httpRequest(urii));
}


