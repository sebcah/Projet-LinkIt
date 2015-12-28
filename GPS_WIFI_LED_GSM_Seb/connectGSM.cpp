#include <LGSM.h>

extern double latitude;
extern double longitude;
extern int numsat;

void connectGSM()
{
    Serial.println("Waiting SMS...");
    char p_num[20];
    int len = 0;
    char dtaget[500];

    if(LSMS.available()) // Check if there is new SMS
    {
    
        LSMS.remoteNumber(p_num, 20); // display Number part
        Serial.println("#####################");
        Serial.println("There is new message.");
        
        Serial.print("Number:");
        Serial.println(p_num);
        Serial.print("Content:"); // display Content part     

        while(true)
        {
            int v = LSMS.read();
            if(v < 0)
            break;

            dtaget[len++] = (char)v;
            Serial.print((char)v);
        }

        Serial.println();
        LSMS.flush(); // delete message

        
        if((dtaget[0] == 'O' && dtaget[1] == 'N') || (dtaget[0] == 'o' && dtaget[1] == 'n'))
        {
            digitalWrite(13, HIGH);        // lamp on
            Serial.println("Switch LED ON ");
        }

        else if((dtaget[0] == 'O' && dtaget[1] == 'F' && dtaget[2] == 'F') || (dtaget[0] == 'o' && dtaget[1] == 'f' && dtaget[2] == 'f'))
        {
            digitalWrite(13, LOW);         // lamp off
            Serial.println("Switch LED OFF ");
        }
        
        else if((dtaget[0] == 'G' && dtaget[1] == 'P' && dtaget[2] == 'S') || (dtaget[0] == 'g' && dtaget[1] == 'p' && dtaget[2] == 's'))
        {
            LSMS.beginSMS(p_num); //envoie d'un SMS au numéro reçu
            LSMS.print("latitude : ");
            LSMS.print(latitude);
            LSMS.print(" , longitude : ");
            LSMS.print(longitude);
            LSMS.print("Nombre de Satellites : ");
            LSMS.print(numsat);
            LSMS.endSMS();
            Serial.println("Données GPS envoyées par SMS");
        }
        
        Serial.println("#####################");
    }
}
