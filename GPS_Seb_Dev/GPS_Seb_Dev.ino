//Connection GPS
#include <LGPS.h> //C:\Program Files (x86)\LinkIt-ONE-IDE-master\hardware\arduino\mtk\libraries\LGPS
#include "position.h"
#include "wificonnect.h"

#include <HttpClient.h> //téléchargé et ajouté à la lib depuis le net https://github.com/amcewen/HttpClient/releases
#include <LWiFi.h>
#include <LDateTime.h>

//Vérifier + Téléverser le code sur le PORT6 Debug, ouvrir le terminal sur le PORT3 Modem pour visualiser
gpsSentenceInfoStruct info;

void setup() {
  // put your setup code here, to run once:obligatoire, même vide
  
  /******connection internet******/
  LTask.begin();
  LWiFi.begin();
  Serial.begin(115200);
  
  wifiprint(); // affichage console etat TCP-IP
  
  pinMode(13, OUTPUT);
  getconnectInfo();
  connectTCP();
  /*********/
  
  LGPS.powerOn();
  Serial.println("Demarrage GPS..."); 
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:= int main(void)

  void TCPsock();
  
  //Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  Serial.println("***************"); 
  Serial.println("Trame GPGGA :"); 
  Serial.println((char*)info.GPGGA); //affichage trame brute GPGGA
  
  Serial.println("Donnees recoltees :"); 
  parseGPGGA((const char*)info.GPGGA); // traitement de la trame GPGGA
  delay(2000);
}
