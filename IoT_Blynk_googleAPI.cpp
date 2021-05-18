#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h> //ESP8266 Library
#include <ArduinoJson.h> //For phrasing JSON file download from https://github.com/bblanchon/ArduinoJson
#include <stdlib.h>

char auth[] = "YOUR_BLYNK_APLICATION_KEY";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YOUR_SSID";
char pass[] = "YOUR _PASSWORD";

const char* ssid1 = "YOUR_SSID"; //Enter your Wi-Fi SSID
const char* password1 = "YOUR _PASSWORD"; //Enter you Wi-Fi Password

//*********************************************************** TO NIE DZIALA BO NIE WIEM JAK DODAWAC ELEMENTY TYPU CONST CHAR*
String home_coord = "52.318688,21.110463"; // Marki Fabryczna
String work_coord = "52.202894,21.0007119"; // Wydział Mechatroniki
String API1 = "YOUR_GOOGLE_API_KEY"; // API key
String URL = "https://maps.googleapis.com/maps/api/distancematrix/json?origins=" + home_coord + "&destinations=" + work_coord + "&mode=driving&traffic_model=best_guess&departure_time=now&language=en-EN&sensor=false&key=" + API1;
//*****************************************************************************************

WidgetLED led1(V1);
WidgetLED led0(V0);
BlynkTimer timer;

// Variables

int period = 20000;
unsigned long time_now = 0;

BLYNK_WRITE(V30)
{
    int pinValueMail = param.asInt(); // assigning incoming value from pin V1 to a variable
    if(pinValueMail>0){
      Blynk.email("wojtek96x@gmail.com", "Subject: Button Logger", "You just pushed the button...");
      }
}

BLYNK_WRITE(V20)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue>0){
      if (WiFi.status() == WL_CONNECTED) { //If Wi-Fi connected successfully 
        led1.on();
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;  //start a HTTPClinet as http 
        
          if (https.begin(*client, URL)) {  // HTTPS *****************************************************
            int httpCode = https.GET(); //pass a get request                                                               
            Blynk.virtualWrite(V5, httpCode); 
             
              if (httpCode > 0) { //Check the returning code
                led0.on();
                String payload = https.getString();
                 /*Phrasing Data using the JSON librarey */ //Use https://arduinojson.org/assistant/ to get the phrasing values for your JSON string 
        // rozmiar JSON
                const size_t capacity = 4*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + 280;
        
                DynamicJsonDocument doc(capacity);
                deserializeJson(doc, payload);
      
                      //Address the value sin to desired variables 
//*************************************************************************************                
                const char* destination_addresses_0 = doc["destination_addresses"][0]; // "św. Andrzeja Boboli 8, 02-525 Warszawa, Poland"
                
                const char* origin_addresses_0 = doc["origin_addresses"][0]; // "Fabryczna 48C, 05-270 Marki, Poland"
                
                JsonObject rows_0_elements_0 = doc["rows"][0]["elements"][0];
                
                const char* rows_0_elements_0_distance_text = rows_0_elements_0["distance"]["text"]; // "23.1 km"
                int rows_0_elements_0_distance_value = rows_0_elements_0["distance"]["value"]; // 23055
                
                const char* rows_0_elements_0_duration_text = rows_0_elements_0["duration"]["text"]; // "27 mins"
                int rows_0_elements_0_duration_value = rows_0_elements_0["duration"]["value"]; // 1642
                
                const char* rows_0_elements_0_duration_in_traffic_text = rows_0_elements_0["duration_in_traffic"]["text"]; // "29 mins"
                int rows_0_elements_0_duration_in_traffic_value = rows_0_elements_0["duration_in_traffic"]["value"]; // 1750
                
                const char* rows_0_elements_0_status = rows_0_elements_0["status"]; // "OK"
                
                const char* status = doc["status"]; // "OK"
//*************************************************************************************

                
                Blynk.virtualWrite(V7, destination_addresses_0);                      // adres docelowy
                Blynk.virtualWrite(V8, origin_addresses_0);                           // adres startowy
                Blynk.virtualWrite(V9, rows_0_elements_0_distance_text);              // dystans w km
                Blynk.virtualWrite(V10, rows_0_elements_0_duration_text);             // czas przejazdu w min
                Blynk.virtualWrite(V11, rows_0_elements_0_duration_in_traffic_text);  // czas w korkach w min
                
              }
            https.end();   //Close http connection
          }
        }           
      }
      else
      {
      led1.off();
      led0.off();
      Blynk.virtualWrite(V5, -1);
      }
}


void setup()
{
  // Debug console
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid1, password1);
  Blynk.begin(auth, ssid, pass);

  while (WiFi.status() != WL_CONNECTED) { //Wait till Wi-Fi is connected
    delay(1000);
    led0.on(); 
  }
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  if(millis() > time_now + period){
          time_now = millis();
          Blynk.virtualWrite(V7, "Pobierz");
          Blynk.virtualWrite(V8, "Pobierz");
          Blynk.virtualWrite(V9, "Pobierz");
          Blynk.virtualWrite(V10, "Pobierz");
          Blynk.virtualWrite(V11, "Pobierz");          
  } 
}