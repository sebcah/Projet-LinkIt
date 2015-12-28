//Connection GPS
#include <LGPS.h> //C:\Program Files (x86)\LinkIt-ONE-IDE-master\hardware\arduino\mtk\libraries\LGPS
#include <LGSM.h>
#include <HttpClient.h> //téléchargé et ajouté à la lib depuis le net https://github.com/amcewen/HttpClient/releases
#include <LWiFi.h>
#include <LDateTime.h>

#include "position.h"
#include "wificonnect.h"
#include "connectGSM.h"
#include "led.h"



//Vérifier + Téléverser le code sur le PORT Debug, ouvrir le terminal sur le PORT Modem pour visualiser
gpsSentenceInfoStruct info;

void setup() {
  // put your setup code here, to run once:obligatoire, même vide
  
  pinMode(REDLED, OUTPUT);
  Serial.begin(115200);
  
  /******connection internet******/
  LTask.begin();
  LWiFi.begin();
  wifiprint(); // affichage console etat TCP-IP
  getconnectInfo();
  connectTCP();
  /******connection GPS******/
  LGPS.powerOn();
  Serial.println("Demarrage GPS..."); 

  #if 0
    /*****connection GSM*******/
      while(!LSMS.ready())
    {
        delay(1000);
        Serial.println("Waiting GSM...");
    }
    Serial.println("GSM OK!!");
  #endif 
	
	 // while(!Serial) delay(1000); // attente affichage écran
      
    digitalWrite(REDLED, HIGH);             // LED ON : on est connecté
    delay(1000);
    digitalWrite(REDLED, LOW);              // LED off
    delay(100);
    digitalWrite(REDLED, HIGH);             // LED on
    delay(1000);
    digitalWrite(REDLED, LOW);              // LED off
    delay(100);
    
  delay(3000);
}

void loop() {
  
  Serial.println("***************"); 
  TCPsock(); // pilotage de la LED via le site internet MCS
  //Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  //Serial.println("Trame GPGGA :"); 
  //Serial.println((char*)info.GPGGA); //affichage trame brute GPGGA
  Serial.println("Donnees recoltees :"); 
  parseGPGGA((const char*)info.GPGGA); // traitement de la trame GPGGA
  //connectGSM(); //traitement des SMS
  delay(2000);
}
