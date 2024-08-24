// RobZ................  TIME TIME TIME TIME ....................................

bool iniNTP(){
  Serial.println("NTP starting.\nFetching date and time from pool.ntp.org");
  timeClient.begin();// Initialize a NTPClient to get time
  timeClient.setTimeOffset(0);
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("UTC time:");
  Serial.println(formattedTime);  
  return timeClient.isTimeSet();
}
