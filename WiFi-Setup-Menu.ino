/** 
 * configuration of WiFi using a Captive Portal
 * edit the file wm_strings_en.h in Arduino/libraries/WiFiManager to change HTML style, reply strings and more
 **/
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

bool wm_nonblocking = false; // change to true to use non blocking
WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

void  WiFi_init() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.setDebugOutput(true);  
  delay(3000);
  Serial.println("\n Starting WiFi Acess Point in captive portal mode");
// wm.resetSettings(); // ----  erase stored WiFi settings (This is for testing purposes. Comment out this line in production)
// ---- COMMENT OUT ABOVE LINE  -------
  if(wm_nonblocking) wm.setConfigPortalBlocking(false);
  std::vector<const char *> menu = {"wifi"};
  wm.setMenu(menu);
  wm.setClass("invert");
  wm.setShowStaticFields(true); // force show static ip fields
  wm.setShowDnsFields(true);    // force show dns field always
  wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
  // wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if(wm.autoConnect("WSPR-TX-XP")) {//SSID Name of Captive portal 
    Serial.print("connecteded to WiFi network- ");
    Serial.println(wm.getWiFiSSID());
  } 
  else {Serial.println("Failed to connect or hit timeout");}
}


String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
}


void resetWiFi(){
  Serial.println("Erasing WiFi Config, and restarting");
  wm.resetSettings();
  ESP.restart();
}
