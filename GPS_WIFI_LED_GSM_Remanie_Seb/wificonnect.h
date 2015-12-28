// fichier wificonnect.h

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

void wifiprint();

void TCPsock();

void getconnectInfo();

void connectTCP();

void heartBeat();




