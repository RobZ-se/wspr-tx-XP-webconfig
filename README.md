Use the ZachTek WSPR-TX Xp transmitter without a GPS antenna. 
connects to a Timeserver via NTP.

This version adds software for WiFi connection and a NTP client so that the unit can operate without GPS.
WiFi is setup via a captive webb portal (eg this unit will start as an Access point. (named WSPT-TX-XP)
After connection to portal, the user can enter the name and password for their own home WiFi network, and then the WSPR-TX Xp will reboot and connect to that WiFi
The WiFi setting is persistent. 
To clear WiFi settings, send the newly added, command [CCW] S (Command Clear WiFi Setting).
Captive webportal by  tzapu / WiFiManager.
//Robert
