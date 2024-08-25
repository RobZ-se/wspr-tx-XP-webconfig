Use the ZachTek WSPR-TX XP transmitter without an GPS antenna. 

This version adds software for WiFi connection and a NTP client so that the unit can operate without GPS.
WiFi is setup via a captive webb portal. 
The unit will start as an Access point. (named WSPT-TX-XP)
At the portal, the user can choose WiFi network from a list and enter the password. 
The  WSPR-TX XP will then reboot and connect to the selected WiFi network.
The WiFi setting is persistent. 
To clear WiFi settings, send the newly added, command [CCW] S (Command Clear WiFi Setting).
Captive webportal by  tzapu / WiFiManager.
//Robert
