// RobZ................  TIME TIME TIME TIME ....................................

bool iniNTP(){
  Serial.println("NTP starting.\nFetching date and time from pool.ntp.org");
  timeClient.begin();// Initialize a NTPClient to get time
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Time is ");
  Serial.println(formattedTime);  
  
  return timeClient.isTimeSet();
}