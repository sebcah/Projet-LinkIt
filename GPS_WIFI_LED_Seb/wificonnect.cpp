#include "wificonnect.h"

#define WIFI_AP "freebox_ISA" // wifi name
#define WIFI_PASSWORD "belier2010" // password name
//#define WIFI_AP "freebox_TSNLVB_EXT"
//#define WIFI_PASSWORD "iletaitunpetitnavire"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define per 50
#define per1 3
#define DEVICEID "DciBuwJ9" // on MCS website, created with the project
#define DEVICEKEY "biiuBqGb8s60iHWT" // on MCS website, created with the project
#define SITE_URL "api.mediatek.com"

#include <HttpClient.h> //téléchargé et ajouté à la lib depuis le net https://github.com/amcewen/HttpClient/releases
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LDateTime.h>

char buffer_latitude[8];
char buffer_longitude[8];

/************connection internet**************/

unsigned int rtc;
unsigned int lrtc;
unsigned int rtc1;
unsigned int lrtc1;
char port[4]={0};
char connection_info[21]={0};
char ip[21]={0};             
int portnum;
int val = 0;
String tcpdata = String(DEVICEID) + "," + String(DEVICEKEY) + ",0";
String upload_led;
String tcpcmd_led_on = "LED_Control,1";
String tcpcmd_led_off = "LED_Control,0";

LWiFiClient c;
LWiFiClient c2;

HttpClient http(c2);

void wifiprint(){
    
  //while(!Serial) delay(1000); /* comment out this line when Serial is not present, ie. run this demo without connect to PC */
    
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }
  
  Serial.println("calling connection");
  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
    
}

void TCPsock(){
   //Check for TCP socket command from MCS Server, pilotage de la LED
  String tcpcmd="";
  while (c.available())
   {
      int v = c.read();
      if (v != -1)
      {
        Serial.print((char)v);
        tcpcmd += (char)v;
        if (tcpcmd.substring(40).equals(tcpcmd_led_on)){
          digitalWrite(13, HIGH);
          Serial.println();
          Serial.println("Switch LED ON ");
          tcpcmd="";
        }else if(tcpcmd.substring(40).equals(tcpcmd_led_off)){  
          digitalWrite(13, LOW);
          Serial.println();
          Serial.println("Switch LED OFF");
          tcpcmd="";
        }
      }
   }
  LDateTime.getRtc(&rtc);
  if ((rtc - lrtc) >= per) {
    heartBeat();
    lrtc = rtc;
  }
  //Check for report datapoint status interval
  LDateTime.getRtc(&rtc1);
  if ((rtc1 - lrtc1) >= per1) {
    uploadstatus(); // vérifie si LED bien allumée ou éteinte
    lrtc1 = rtc1;
  }
  /****************************/
}

 void uploadstatus(){
  //calling RESTful API to upload datapoint to MCS to report LED status
  //Serial.println("calling connection");
  LWiFiClient c2;  

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  
  if(digitalRead(13)==1)
	upload_led = "LED_Display,,1";
  else
	upload_led = "LED_Display,,0";

  int thislength = upload_led.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_led);
  
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
  
void getconnectInfo(){
  //calling RESTful API to get TCP socket connection
  c2.print("GET /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/connections.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.println("Connection: close");
  c2.println();
  
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
  Serial.println();
  char c;
  int ipcount = 0;
  int count = 0;
  int separater = 0;
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      c = v;
      Serial.print(c);
      connection_info[ipcount]=c;
      if(c==',')
      separater=ipcount;
      ipcount++;    
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();
    }  
  }
  Serial.print("The connection info: ");
  Serial.println(connection_info);
  int i;
  for(i=0;i<separater;i++)
  {  ip[i]=connection_info[i];
  }
  int j=0;
  separater++;
  for(i=separater;i<21 && j<5;i++)
  {  port[j]=connection_info[i];
     j++;
  }
  Serial.println("The TCP Socket connection instructions:");
  Serial.print("IP: ");
  Serial.println(ip);
  Serial.print("Port: ");
  Serial.println(port);
  portnum = atoi (port);
  Serial.println(portnum);

} //getconnectInfo

void connectTCP(){
  //establish TCP connection with TCP Server with designate IP and Port
  c.stop();
  Serial.println("Connecting to TCP");
  Serial.println(ip);
  Serial.println(portnum);
  while (0 == c.connect(ip, portnum))
  {
    Serial.println("Re-Connecting to TCP");    
    delay(1000);
  }  
  Serial.println("send TCP connect");
  c.println(tcpdata);
  c.println();
  Serial.println("Everything is okay - waiting TCP response:");
} //connectTCP

void heartBeat(){
  Serial.println("send TCP heartBeat");
  c.println(tcpdata);
  c.println();
    
} //heartBeat

void uploadGPS(double latitude, double longitude){

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.print(".");
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
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(GPS_length);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_GPS);
  delay(500);

  int errorcount = 0;
  
  while (!c2.available())
  {
    Serial.print(".");
    delay(100);
  }
  int err = http.skipResponseHeaders();
  int bodyLen = http.contentLength();
  
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
  Serial.println();
}
