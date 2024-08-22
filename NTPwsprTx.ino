void noGPS_WSPR_Tx(int WSPRMessageType)
{
 if ( (GPSS == 00) && (CorrectTimeslot ()) )//If second is zero at even minute then start WSPR transmission. The function CorrectTimeSlot can hold of transmision depending on several user settings. The GadgetData.WSPRData.TimeSlotCode value will influense the behaviour
 {
    if ( (PCConnected) || (Product_Model != 1028) || ((Product_Model == 1028) && OutsideGeoFence ()))//On the WSPR-TX Pico make sure were are outside the territory of UK, Yemen and North Korea before the transmitter is started but allow tranmissions inside the Geo-Fence if a PC is connected so UK users can make test tranmissions on the ground before relase of Picos
    {
      GPSGoToSleep();//Put GPS to sleep to save power
      if (SendWSPRMessage (WSPRMessageType) != 0) //Send a WSPR Type 1 or Type 2 message for 1 minute and 50 seconds
      {
        // there was a serial command that interrupted the WSPR Block so go and handle it
        return;
      }
      if (GadgetData.WSPRData.LocationPrecision == 6)//If higher position precision is set then start a new WSPR tranmission of Type 3
      {
        delay(9000);      //wait 9 seconds so we are at the top of an even minute again
        if (SendWSPRMessage (3) != 0) //Send a WSPR Type 3 message for 1 minute and 50 seconds
        {
          // there was a serial command that interrupted the WSPR Block so go and handle it
          return;
        }
      }
      StorePosition ();//Save the current position;
      if (LastFreq ())  //If all bands have been transmitted on then pause for user defined time and after that start over on the first band again
      {
        if ((GadgetData.TXPause > 60) && ((Product_Model == 1017) || (Product_Model == 1028))  && (!PCConnected)) //If the PC is not connected and the TXdelay is longer than a 60 sec then put the MCU to sleep to save current during this long pause (Mini and Pico models only)
        {
          delay (600); //Let the serial port send data from its buffer before we go to sleep
          PowerSaveOFF();                                //We are back from sleep - turn on GPS and PLL again
          smartdelay(2000); // let the smartdelay routine read a few GPS lines so we can get the new GPS time after our sleep
        }
        else
        { //Regular pause if we did not go to sleep then do a regular pause and send updates to the GUI for the duration
          smartdelay(GadgetData.TXPause * 1000UL); //Pause for the time set by the user
        }
        SendAPIUpdate(UMesWSPRBandCycleComplete);//Inform PC that we have transmitted on the last enabled WSPR band and will start over
      }
      GPSWakeUp();
      NextFreq();// get the frequency for the next HAM band that we will transmit on
      freq = freq + (100ULL * random (-100, 100)); //modify the TX frequency with a random value beween -100 and +100 Hz to avoid possible lengthy colisions with other users on the band
      smartdelay(3000);
    }
  }
}