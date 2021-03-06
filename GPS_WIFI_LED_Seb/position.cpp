#include <LGPS.h>
#include "position.h"
#include "wificonnect.h"

char buff[256];
char buffsf[256];

 double latitude;
 double longitude;
 int tmp, hour, minute, second, num, hoursf, hourfr ;


static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr) //traitement de la trame GPGGA
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    
    //heure a San Francisco
    if (hour > 7){ 
      hoursf = hour - 7;
    }
    else {
      hoursf = hour + 17;
    } 
    
    //heure en France
    hourfr = hour + 1;
    
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second   = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "Heure France : %2d-%2d-%2d", hourfr, minute, second);
    Serial.println(buff);
    

	
    /*tmp = getComma(2, GPGGAstr); // ancien calcul trame GPS
	latitude = getDoubleNumber(&GPGGAstr[tmp]); 
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);*/
	
	tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp])/100.0;
    int latitude_int=floor(latitude);
    double latitude_decimal=(latitude-latitude_int)*100.0/60.0;
    latitude=latitude_int+latitude_decimal;
	
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp])/100.0;
    int longitude_int=floor(longitude);
    double longitude_decimal=(longitude-longitude_int)*100.0/60.0;
    longitude=longitude_int+longitude_decimal;
    
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    
    sprintf(buff, "nombres de satellites = %d", num);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
  
 uploadGPS(latitude,longitude);
  
}

