/**
   StreamHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     2 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ESP8266WiFiMulti WiFiMulti;

int strpos(char *hay, char *needle, int offset)
{
   char haystack[strlen(hay)];
   strncpy(haystack, hay+offset, strlen(hay)-offset);
   char *p = strstr(haystack, needle);
   if (p)
      return p - haystack+offset;
   return -1;
}
void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  WiFi.mode(WIFI_STA);
 WiFiMulti.addAP("CamelloVTR","Cieloestrellado37");
 display.display();
 display.clearDisplay();
}

bool found = false;
bool endfound = false;
bool firstime = true;


void loop() {
 String series[30];
int counter = 0;
int counterstop = 0;
int counterfinal = 999;
endfound = false;
found = false;
int counterserie = 0;
  String allbuffer = "";

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
   
    bool mfln = client->probeMaxFragmentLength("jkanime.net", 443, 1024);
    Serial.printf("\nConnecting to https://jkanime.net\n");
    Serial.printf("Maximum fragment Length negotiation supported: %s\n", mfln ? "yes" : "no");
    if (mfln) {
      client->setBufferSizes(1024, 1024);
    }

    Serial.print("[HTTPS] begin...\n");

    // configure server and url
    const uint8_t fingerprint[20] = {0xd7,0x49,0x42,0xae,0x15,0x69,0xdf,0x1e,0x83,0xf0,0xf1,0xfb,0x11,0x64,0x6e,0xab,0xa8,0x3f,0xa7,0x43};

    client->setFingerprint(fingerprint);

    HTTPClient https;

    if (https.begin(*client, "https://jkanime.net/")) {

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {

          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = https.getSize();
         
          // create buffer for read
          static uint8_t buff[1024] = { 0 };

          // read all data from server
          while (https.connected() && (len > 0 || len == -1)) {
            allbuffer = "";
         
            // get available data size
            size_t size = client->available();

            if (size) {
              // read up to 128 byte
              int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

              // write it to Serial
           
             
              if (len > 0) {
                len -= c;
              }
             allbuffer = String((char*) buff);
             
             display.clearDisplay();
              Serial.println(counter);
              display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  
   display.println("RipNotify");
    if (allbuffer.indexOf("listadoanime-home") != -1)
    {found = true;
      counterstop = counter;}
                 if(found == true)
              { //display.print("encontrado:" );
                //display.println(counterstop);
                Serial.print("encontrado:");
                Serial.println(counterstop);
                   //Serial.write(buff,c);
                   /*if (firstime == true)
                   {temporal = allbuffer;
                    firstime = false;
                   }*/
                   int scrap1,scrap2;
          scrap1 = allbuffer.indexOf("bloqq");
          scrap1 = allbuffer.lastIndexOf("href=",scrap1)+6;
          scrap2 = allbuffer.indexOf('"',scrap1); 
          String title = allbuffer.substring(scrap1, scrap2);
          series[counterserie] = title;
          counterserie++;
          //display.println(title);
         // Serial.println(title);      
                
                }
 
    
      
  if (allbuffer.indexOf("spad") != -1)
           {if (counter != 0)
            { Serial.println("que es esta wea?");
              break;}}
                
   
  counter++;
 
             }

   
  
          



  display.display();
          }

         for(int x = 0 ;x < 29; x++)
         {
          if(series[x].indexOf("https://jkanime") != -1)
          {
               display.clearDisplay();
              display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);        // Draw white text
  display.setCursor(0,20);             // Start at top-left corner
            String tempshow = series[x];
             tempshow.replace("https://jkanime.net/"," ");
             tempshow.replace("/"," ");
             tempshow.replace("-"," ");
             tempshow.toUpperCase();
            Serial.println(tempshow);
             display.println(tempshow);
             delay(2000);
            display.display();
            
            }
         
         }
          Serial.println("[HTTPS] connection closed or file end.\n");
       
        display.display();
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("Unable to connect\n");
    }
  }

  Serial.println("Wait 30s before the next round...");
  delay(5000);
}
