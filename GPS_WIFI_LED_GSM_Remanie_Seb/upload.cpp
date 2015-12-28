#include <LWiFiClient.h>
#include <HttpClient.h> 

#include "upload.h"
#include "wificonnect.h"
#include "led.h"

String upload_led;
extern LWiFiClient c2;  

char buffer_latitude[8];
char buffer_longitude[8];

 void uploadstatus(){
  //calling RESTful API to upload datapoint to MCS to report LED status
  //Serial.println("calling connection");

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  
  if(digitalRead(REDLED)==1)
	upload_led = "LED_Display,,1";
  else
	upload_led = "LED_Display,,0";

  int thislength = upload_led.length();
  
  httpsend(thislength,upload_led);
  
}

void uploadGPS(double latitude, double longitude){

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.print("Re-Connecting to WebSite");
    delay(500);
  }
  
  delay(100);

  float latitude_post=latitude;
  float longitude_post=longitude;
  
  //Serial.printf("latitude=%.4f\tlongitude=%.4f\n",latitude,longitude);
  if(latitude>-90 && latitude<=90 && longitude>=0 && longitude<360){
    sprintf(buffer_latitude, "%.4f", latitude);
    sprintf(buffer_longitude, "%.4f", longitude);
  }

  String upload_GPS = "GPS,,"+String(buffer_latitude)+","+String(buffer_longitude)+","+"0"+"\n"+"LATITUDE,,"+buffer_latitude+"\n"+"LONGITUDE,,"+buffer_longitude;//null altitude
  int GPS_length = upload_GPS.length();
  
  httpsend(GPS_length,upload_GPS);
  
  Serial.println();  
}

void httpsend(int message, String upload_message){
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(message);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_message);
  
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    //Serial.println("waiting HTTP response: ");
    //Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  //Serial.print("Content length is: ");
  //Serial.println(bodyLen);
  //Serial.println();
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();
	}
  }
}
