

/*
  Software for Zachtek WSPR Transmitter products
  For Arduino Pro Mini ATMega 328 8MHz, 3.3V boards or ATMega328P-AU chips


  Hardware connections:
  --------------------------
  
  GPIO 0 is for the Status LED, lights when pin is set to Low
  GPIO 9 and 10 are Sofware serial port to GPS module
  GPIO 4 and 5 are for I2C communication to the Si5351 PLL
  GPIO 12,13 and 14 are Relay control pins
  

  Version History:
  -------------------------------------
  0.51 First Beta for Serial API
  0.51 Expanded the Serial API and changed all information messages to {MIN} messages
  0.52 [DGF] API updates will change SignalGen output freq if running.
  0.54 Added TX status updates when sending  CCM status to improve the PC client display.
  0.55 Added support for factory data in EEPROM, stores TCXO and hardware info.
  0.56 Changed 80 TX freq to new standard of 3.570,100MHz
  0.57 Split Firmware in t o two separate version for the WSPR-TX_ LP1 and Desktop products and changed the "Product_Model" from Factory EEPROM data to constant
  0.58 Fixed Frequency information output calculation errors when Freq=<1MHz
  0.59 Fixed dim Red TX LED, (Pin was not set to output by setup routine)
  0.60 Fixed wrong TX Freq on 10,12 and 15 Band
  0.61 Added routines to Set and Get LP filters with factory data API [FLP]
  0.62 Added functionality to automatically use one of the Low Pass filter in WSPR and SignalGen routines (New routines - PickLP,SetLPFilter)
  0.63 Changed Software Version and Revision to constants that can be read by the Serial API [FSV] and [FSR] and merged Firmware for LP1 and Desktop in one version again as they were before v0.57
  0.64 Added function BandNumOfHigestLP to find the bandnumber of highest fitted LP filter, expanded on the PickLP filter routine
  0.65 Fixed bug that forced Hardware Revision to 4
  0.66 Fixed relay driving bug that affected Desktop transmitter with hardware revision higher than 4
  0.67 Added support for relay driving the WSPR-TX_LP1 with the Mezzanine LP4 card that contains relays
  0.68 Added support for manual override relay control over the Serial API and Relay update messages. ([CSL] command and {LPI} status message, shortened Start LED Blink
  0.69 Added support for hardware WSPR-TX Mini 1021, added function readVcc() that returns Arduino Voltage, added PowerSave functions save current on the Mini
  0.70 Added power saving for the Mini, GPS turned off if TX pause is longer than a minute.
  0.71 Corrected 2m Frequency, not in use but nice to have correct. Enabled PLL power saving for the Mini if TX pause is longer than a minute. Current draw for mini is: 40mA waiting to TX,  60mA TX,
  0.72 Sends GPS updates when in Idle and Signal gen mode and when pausing in WSRP Beacon mode. Improved Serial port respons when pausing in WSPR Beacon mode,
       On the Mini the GPS and the Si5351 is put to power save during long TX Pauses. Current draw is now around 40 mA regardless if transmitting or not.
  0.73 Sends Satellite positions and their received SNR to the config program, changed to NeoGPS library
  0.74 Improvement in GUI responsiveness when in WSPR Beacon. Additional power saving for the Mini, MCU goes to sleep if TX pause is longer than a minute.
  0.75 Added MCU VCC Voltage info, added support for Pico model, Sends less GPS info in Idle mode
  0.76 Added Support for WSPR-TX Pico, GPS position updates in idle and signal gen mode, status LED now fast blink during WSPR Beacon TX instead of steady lit
       The Pico will always boot in to WSPR Beacon regardless of Boot configuration, this is a failsafe.
       Moved check if Call Sign is set from SendWSPRBlock() to DoWSPR()
  0.77 Changed around the orders of hardware check in Setup()
  0.78 The WSPR Beacon will now stay in Beacon mode even if the user changed something in the PC GUI like changed bands, click Save button etc.
  0.79 Support the new Desktop V1R10 with new improved LP filters. Fixed TX Pause limit of 32000, it can now go to 99999 seconds (27.7 Hours)
  0.80 Added routine FreqToBand () to improve signal generator filtering. The Signal Generator now picks the correct low pass filter instead of always using the highest one
       Fixed smartdelay routine so it only transmits status once a second to avoid Configuration GUI lockup due to excessive amount of data
  0.81 Added Altitude encoding in the power field option.
  0.82 Improved handling of serial data when in WSPR Beacon mode so it is less likely to exit the Beacon mode when serial API queries are sent from PC.
       Slight improvement of the smartdelay routine
  0.83 Only put the MCU to sleep in WSPR Beacon TX Pauses when the PC is not connected.
       This ensures responsiveness in the PC GUI and avoid the misconception that the device has hang during TX Pauses
  0.84 Fixed Pico GPS Sleep functions so the GPS would restart properly in brownout conditions.
       Fixed so WSPR beacon goes back again after Serial API command is handled (A bug made the beacon go back only after a GPS fix, now goes back immediately even if no GPS fix)
  0.85 Picos combined 20m and 30m low pass filter correctly reported to PC program, Now the idle routine will reset an unresponsive GPS after some time. 0.84 did the same in when in the WSPR beacon mode
  0.86 Improved the paus timing accuracy when the Mini or Pico is sleeping during long TX Pauses.
  0.87 Small code optimization by replacing 0 prints with procedure calls, added I2CInit to Startup routine, Status LED will blink every 5 seconds during TX Pause (if MCU is not going to sleep in case it will blink every 9 seconds(Only Mini and Pico))
  0.88 Support for LP1 add-on card BLP4, prod model #1029, adjusted Low pass filter calculation in FreqToBand ()
  0.89 Fixed a bug that indicated 10-pole low pass filters for LP1 instead of the standard 7-pole in the DecodeSerialCMD routine.
  0.90 Removed dependency on JTencode library by copying the needed code.
  0.91 Adjust start of transmission as the GPS data is a bit delayed, It will now start the transmission 1 second earlier leading to a lower "DT" number in WSJT-X
  0.92 Includes Geo-Fencing for the Pico, will not transmit over restricted parts of the word
  0.93 Modified GPSWakeUp routine to reset GPS on all models when waking up from sleep
  0.94 Fixed bug that cased GPS reset after each transmission
  0.95 WSPR Mini re-added after it had been removed by accident in an earlier version
  0.96 Added support for Type 3 messages for increased position precision, changed altitude calculation for the Pico, if Type 3 message is sent then its pwr field corresponds to 20m per dB
       Added auto detection of Si5351 I2C address, 96 or 98.
  0.97 Changed how the [OBD] was Set/Get to avoid feedback loop in the PC GUI that would Set and then Clear a value repeatedly
  1.00 Fixed a bug that caused Type 3 transmission even if it was not configured.
  1.01 Desktop model with 10m filter installed now indicates it can do 17m as well besides the 12m and 15m
  1.02 Fixed a bug with certain short callsings sending incorrect call signs once in a while. Affected only call signs that had less than three letters after their number, e.g.
       a call sign in the format AA1AAA worked fine but AA1AA and AA1A once in about 10 to 100 transmissions generated incorrect transmissions.
       Also fixed a bug that caused Type 3 transmissions (six character Maidenhed positions) to only work with callsigns that were six character long
  1.10 Added transmit schedule. Added Prefix and Suffix support for Callsign.
  1.11 Fixed bug that causes incorrect results if the altitude is negative and the option "Encode Altitude as power" is used, thanks to John Maca - AB5SS for identifying the bug and a fix for it.
  1.12 Added tracking schedule where the TX will only transmit if it moves. If stationary it will TX once every hour. Enabled by setting the GadgetData.WSPRData.TimeSlotCode to 17
  2:01 Changed to ESP8285 as target MCU
  2:02 Added blank start up values for prefix and suffix, dont echo back [OLP] when set
  2:03 Added support for non standard i2C adres on the Si5351, the address has to be set in the code
  2:04 Removed dependency on EtherKit Si5351 library  and instead used Hans Summers Demo code for si5351 control. This makes it possible to autodetect I2C adress instead of having is static assigned like in firmware 2:03
  2:05 Added option to pick what GPS Constellations to use, GPS, BeiDou or both. Option [OSC] Sets it. Default is GPS Only.
  2.15 Increased the revision number to match the 1.15 revision as they are the same besides being compiled for different hardvare (V1=ATMega328, V2=ESP8285)
  2.16 Support for new relay driving on HW Revsion 36 boards. 
  2.17 Support for external reference. Sense line is on IO 14. Set Frequency with command [DER], it is set to 10MHz by default. 
       [CCR] G queries if (E)xternal or (I)nternal reference is in use.
  2.18 Fix for two new bugs introduced in 2.17 1 Frequency calculation bug. 2. Relay driving on the XP model
 



  To compile :
  1 Use the menu File, Preferences and add the following string to "Additional Board Manager URL" field: http://arduino.esp8266.com/stable/package_esp8266com_index.json 
  2 Restart the Arduino IDE software and then go to the boards Manager in the tools menu and download the esp8266 board definition
  3 Set the Board to "Generic ESP8285 module using the menu Tools, Board, Esp8266 Boards ..
  4 Set the Reset Method to DTR using the meny Tools, Reset Method:
  5 Set the Upload speed to 57600 (up to 921600 is possible on newest hardware, try and see if it works is you are inpatient :-)
  6 Set the Serial port used under Tools, Ports
  7 Install library "NeoGPS by Slash Devin" 
  8 locate and modify the file NMEAGPS_cfg.h. it is part of the NeoGPS library and on a Windows computer it is usually in ..\Documents\Arduino\libraries\NeoGPS\src
    in that file add the following lines and save it:
    #define NMEAGPS_PARSE_GSV
    #define NMEAGPS_PARSE_SATELLITE_INFO
    #define NMEAGPS_PARSE_SATELLITES

  //Harry
*/

/*
This version adds WiFi and a NTP client so that the unit can operate without GPS.
Initial WiFi setting is setup via a Captive webb portal (eg this unit will start as an Access point. (named WSPT-TX-XP)
After connection to portal, the user can enter the name and password for their own home WiFi network, and then the WSPR-TX Xp will reboot and connect to that WiFi
The WiFi setting is persistent. 
To cleear WiFi settings, send the (new) command [CCW] S (Command Clear WiFi Setting)
Install library "WiFiManager.h" by tzapu // https://github.com/tzapu/WiFiManager
replace  Arduino\libraries\WiFiManager\wm_strings_en.h with the supplied version

//Robert
*/


const uint8_t SoftwareVersion = 2;    //0 to 255. 0=Beta
const uint8_t SoftwareRevision = 18;  //0 to 255

// Product model. WSPR-TX XP (formely LP1)                =1011
// Product model. WSPR-TX Desktop                         =1012
// Product model. WSPR-TX Mini                            =1017
// Product model. SSG                                     =1024
// Product model. WSPR-TX Pico                            =1028
// Product model. WSPR-TX XP Plus                         =1048

const uint16_t Product_Model = 1048;

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <NMEAGPS.h>  //NeoGps by SlashDevin"
#include "Wire.h"
// .................... RobZ .....................
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
// ---------------- end RobZ -------------------------

NMEAGPS gps;  // This parses the GPS characters
gps_fix fix;  // This holds on to the latest values

// Data structures
enum E_Band {
  LF2190m = 0,
  LF630m = 1,
  HF160m = 2,
  HF80m = 3,
  HF40m = 4,
  HF30m = 5,
  HF20m = 6,
  HF17m = 7,
  HF15m = 8,
  HF12m = 9,
  HF10m = 10,
  HF6m = 11,
  VHF4m = 12,
  VHF2m = 13,
  UHF70cm = 14,
  UHF23cm = 15
};

enum E_Mode {
  WSPRBeacon,
  SignalGen,
  Idle
};

enum E_LocatorOption {
  Manual,
  GPS
};

enum E_PowerOption {
  Normal,
  Altitude
};

enum E_SufixPreFixOption {
  Sufix,
  Prefix,
  None
};

enum E_GPSConstellations {
  Cons_GPS,
  Cons_BeiDou,
  Cons_GPSAndBeiDou
};

struct S_WSPRData {
  char CallSign[7];                    //Radio amateur Call Sign, zero terminated string can be four to six char in length + zero termination
  E_SufixPreFixOption SuPreFixOption;  //If a suffix or Prefix to the Callsign is used or not
  char Prefix[4];                      // Prefix three chars max and a zero termination
  uint8_t Sufix;                       // Sufix code in WSPR format, e.g single digit is 0-9, single char (A to Z) is 10-35, double digits (10-99) is 36-125
  E_LocatorOption LocatorOption;       //If transmitted Maidenhead locator is based of GPS location or if it is using MaidneHead4 variable.
  uint8_t LocationPrecision;           //Determines if a second Type 3 transmission will be sent to increase the postiton precision.
  char MaidenHead4[5];                 //Maidenhead locator with four characters and a zero termination
  char MaidenHead6[7];                 //Maidenhead locator with six characters and a zero termination
  E_PowerOption PowerOption;           //If transmitted Power is based on TXPowerdBm field or is calculated from GPS Altitude.
  uint8_t TXPowerdBm;                  //Power data in dBm min=0 max=60
  uint8_t TimeSlotCode;                //Determine on what time slot a tranmsission will be made. If TimeslotCode is 0 to 4 a ten minute scheduled transmission will be used.
  //0=TX om minute 0,10,20.. 1=TX on minute 2,12,22..  ..4=TX om minute 08,18,28 etc. If Timeslotcode is 5 to 14 a twenty minute schedule code will be used.
  //5=TX on minute 0,20,40.  6=TX on minute 2,22,42.  ..14=TX on minute 18,38,58.
  //if the TimeslotCode is 15 a special band coordinated schedule is used.
  //If the TimeslotCode is 16 then no schedule is used, e.g transmission can occur at any time
  //If the TimeslotCode is 17 then transmisson will only occur if GPS derived Maidenhead position has been updated since last transmission. E.g it becomes a tracker that only transmits position updates.
  E_GPSConstellations GPSConstellations;
};

struct S_GadgetData {
  char Name[40];           //Optional Name of the device.
  E_Mode StartMode;        //What mode the Gadget should go to after boot.
  S_WSPRData WSPRData;     //Data needed to transmit a WSPR packet.
  bool TXOnBand[16];       //Arraycount corresponds to the Enum E_Band, True =Transmit Enabled, False = Transmit disabled on this band
  unsigned long TXPause;   //Number of seconds to pause after having transmitted on all enabled bands.
  uint64_t GeneratorFreq;  //Frequency for when in signal Generator mode. Freq in centiHertz.
  uint32_t ExtRefFreq;     //The frequency of the external reference oscillator in Hz, usually 10000000 (10MHz)
};



struct S_FactoryData {
  uint8_t HW_Version;    // Hardware version
  uint8_t HW_Revision;   // Hardware revision
  uint32_t RefFreq;      //The frequency of the built in TCXO Reference Oscillator in Hz, usually 26000000 (26MHz)
  uint8_t LP_A_BandNum;  //Low Pass filter A Band number (0-15) Ham Band as defined by E_Band  Eg. if a 20m LowPass filter is fitted on LP_A then LP_A_BandNum will be set to 6 by factory config software
  uint8_t LP_B_BandNum;  //Low Pass filter B Band number (0-15)
  uint8_t LP_C_BandNum;  //Low Pass filter C Band number (0-15)
  uint8_t LP_D_BandNum;  //Low Pass filter D Band number (0-15)
};


//Constants

//#define SI5351A_H

#define SI_CLK0_CONTROL 16  // Register definitions
#define SI_CLK1_CONTROL 17
#define SI_CLK2_CONTROL 18
#define SI_SYNTH_PLL_A 26
#define SI_SYNTH_PLL_B 34
#define SI_SYNTH_MS_0 42
#define SI_SYNTH_MS_1 50
#define SI_SYNTH_MS_2 58
#define SI_PLL_RESET 177

#define SI_R_DIV_1 0b00000000  // R-division ratio definitions
#define SI_R_DIV_2 0b00010000
#define SI_R_DIV_4 0b00100000
#define SI_R_DIV_8 0b00110000
#define SI_R_DIV_16 0b01000000
#define SI_R_DIV_32 0b01010000
#define SI_R_DIV_64 0b01100000
#define SI_R_DIV_128 0b01110000

#define SI_CLK_SRC_PLL_A 0b00000000
#define SI_CLK_SRC_PLL_B 0b00100000

#define WSPR_SYMBOL_COUNT 162

#define WSPR_FREQ23cm 129650150000ULL  //23cm 1296.501,500MHz (Overtone, not implemented)
#define WSPR_FREQ70cm 43230150000ULL   //70cm  432.301,500MHz (Overtone, not implemented)
#define WSPR_FREQ2m 14449500000ULL     //2m    144.490,000MHz //Not working. No decode in bench test with WSJT-X decoding Software, The PLL steps are not fine-grained enogh to make it work.
#define WSPR_FREQ4m 7009250000ULL      //4m     70.092,500MHz
#define WSPR_FREQ6m 5029450000ULL      //6m     50.294,500MHz
#define WSPR_FREQ10m 2812610000ULL     //10m    28.126,100MHz
#define WSPR_FREQ12m 2492610000ULL     //12m    24.926,100MHz
#define WSPR_FREQ15m 2109610000ULL     //15m    21.096.100MHz
#define WSPR_FREQ17m 1810610000ULL     //17m    18.106,100MHz
#define WSPR_FREQ20m 1409710000ULL     //20m    14.097,100MHz
#define WSPR_FREQ30m 1014020000ULL     //30m    10.140,200MHz
#define WSPR_FREQ40m 704010000ULL      //40m     7.040,100MHz
#define WSPR_FREQ80m 357010000ULL      //80m     3.570,100MHz
#define WSPR_FREQ160m 183810000ULL     //160m    1.838,100MHz
#define WSPR_FREQ630m 47570000ULL      //630m      475.700kHz
#define WSPR_FREQ2190m 13750000ULL     //2190m     137.500kHz


#define FactorySpace true
#define UserSpace false

#define UMesCurrentMode 1
#define UMesLocator 2
#define UMesTime 3
#define UMesGPSLock 4
#define UMesNoGPSLock 5
#define UMesFreq 6
#define UMesTXOn 7
#define UMesTXOff 8
#define UMesLPF 9
#define UMesVCC 10
#define UMesWSPRBandCycleComplete 11


const uint8_t LP_A = 0;
const uint8_t LP_B = 1;
const uint8_t LP_C = 2;
const uint8_t LP_D = 3;



// Hardware defines

#define Pin_SCL 5
#define Pin_SDA 4
#define StatusLED 0  //LED that indicates current status. Yellow on LP1, Desktop and Mini models, white on Pico
uint8_t Relay1;
uint8_t Relay2;
uint8_t Relay3;
#define ExtRefIndicator 14  //Input to detect the use of an external reference oscillator.

//#define TransmitLED 15   //Red LED next to RF out SMA that will turn on when Transmitting (Pico model do not have a TX LED)
//#define GPSPower A1    //Sleep-Wake signal of the GPS on the WSPR-TX Pico
//#define SiPower A3     //Power the Si5351 from this pin on the WSPR-TX Mini


//Global Variables
S_GadgetData GadgetData;    //Create a datastructure that holds all relevant data for a WSPR Beacon
S_FactoryData FactoryData;  //Create a datastructure that holds information of the hardware
E_Mode CurrentMode;         //What mode are we in, WSPR, signal generator or nothing

//from Jason Mildrums JTEncode class
char callsign[7];
char locator[5];
uint8_t power;

//GeoFence grids by Matt Downs - 2E1GYP and Harry Zachrisson - SM7PNV , save some RAM by putting the string in program memory
const char NoTXGrids[] PROGMEM = { "IO78 IO88 IO77 IO87 IO76 IO86 IO75 IO85 IO84 IO94 IO83 IO93 IO82 IO92 JO02 IO81 IO91 JO01 IO70 IO80 IO90 IO64 PN31 PN41 PN20 PN30 PN40 PM29 PM39 PM28 PM38 LK16 LK15 LK14 LK13 LK23 LK24 LK25 LK26 LK36 LK35 LK34 LK33 LK44 LK45 LK46 LK47 LK48 LK58 LK57 LK56 LK55" };  //Airborne transmissions of this sort are not legal over the UK, North Korea, or Yemen.

uint8_t Si5351I2CAddress;         //The I2C address on the Si5351 as detected on startup
uint8_t CurrentBand = 0;          //Keeps track on what band we are currently tranmitting on, See E_Band for definition. 0=136kHz
uint8_t CurrentLP = 0;            //Keep track on what Low Pass filter is currently switched in
const uint8_t SerCMDLength = 50;  //Max number of char on a command in the SerialAPI

uint8_t i2cSendRegister(uint8_t reg, uint8_t data);
//uint8_t i2cReadRegister(uint8_t reg, uint8_t *data);

uint8_t symbolSequence[WSPR_SYMBOL_COUNT];
uint8_t tx_buffer[WSPR_SYMBOL_COUNT];

uint64_t freq;  //Holds the Output frequency when we are in signal generator mode or in WSPR mode
int GPSH;       //GPS Hours
int GPSM;       //GPS Minutes
int GPSS;       //GPS Seconds
int fixstate;   //GPS Fix state-machine. 0=Init, 1=wating for fix,2=fix accuired
boolean PCConnected;
uint32_t LoopGPSNoReceiveCount;  //If GPS stops working while in ídle mode this will increment
char LastMaidenHead6[7];         //Holds the Maidenhead position from last transmission, used when GadgetData.WSPRData.TimeSlotCode=17 to determine if the transmitter has moved since last TX
// The serial connection to the GPS device
SoftwareSerial GPSSerial(9, 10);  //GPS Serial port, RX on pin GPIO9, TX on GPIO10

void si5351aSetFrequency(uint32_t frequency);

//Routine that facilitates reprograming the clock PLL
void pico_i2c_writereg_asm(uint32_t a, uint32_t b) {
  asm volatile(".global pico_i2c_writereg_asm\n.align 4\npico_i2c_writereg_asm:\n_s32i.n  a3, a2, 0\n_memw\n_l32i.n a3, a2, 0\nbbci  a3, 25, .term_pico_writereg\n.reloop_pico_writereg:\n_memw\n_l32i.n a3, a2, 0\nbbsi  a3, 25, .reloop_pico_writereg\n.term_pico_writereg:\n_ret.n");
}
#define pico_i2c_writereg(reg, hostid, par, val) pico_i2c_writereg_asm((hostid << 2) + 0x60000a00 + 0x300, (reg | (par << 8) | (val << 16) | 0x01000000))


void setup() {
  // Underclock 2.5 times
  //  pico_i2c_writereg(103,4,1,0x48);
  // pico_i2c_writereg(103,4,2,0xf1);


  //WiFi.mode( WIFI_OFF );
  //WiFi.forceSleepBegin();
  delay(1);
  EEPROM.begin(512);
  int i;
  Wire.begin();
  PCConnected = false;
  fixstate = 0;  //GPS fixstate=No location fix
  //Initialize the serial ports, The hardware port is used for communicating with a PC.
  //The Soft Serial is for communcating with the GPS
  Serial.begin(9600);  //USB Serial port
  Serial.setTimeout(2000);
  GPSSerial.begin(9600);  //Init software serial port to communicate with the on-board GPS module
  delay(100);

  Serial.println(' ');
  //Read all the Factory data from EEPROM at position 400
  if (LoadFromEPROM(FactorySpace))  //Read all Factory data from EEPROM
  {

  } else  //No factory data was found in EEPROM, set some defaults

  {
    Serial.println(F("{MIN} No factory data found !"));
    Serial.println(F("{MIN} You need to run factory setup to complete the configuration, guessing on calibration values for now"));
    FactoryData.HW_Version = 2;                              // Hardware version
    FactoryData.RefFreq = 25999980;                          //Reference Oscillator frequency
    if ((Product_Model == 1011) || (Product_Model == 1048))  //XP or XP Plus Model, set some defaults
    {
      FactoryData.HW_Revision = 38;   // Hardware revision
      FactoryData.LP_A_BandNum = 98;  //Low Pass filter A is Link
      FactoryData.LP_B_BandNum = 99;  //Low Pass filter B is Nothing
      FactoryData.LP_C_BandNum = 99;  //Low Pass filter C is Nothing
      FactoryData.LP_D_BandNum = 99;  //Low Pass filter D is Nothing
    }

    if (Product_Model == 1012)  //Desktop Model, set default version
    {


      //2190To80 rev 38
      FactoryData.HW_Revision = 38;  // Hardware revision
      FactoryData.LP_A_BandNum = 3;  //Low Pass filter A is 80m
      FactoryData.LP_B_BandNum = 2;  //Low Pass filter B is 160m
      FactoryData.LP_C_BandNum = 1;  //Low Pass filter C is 6300m
      FactoryData.LP_D_BandNum = 0;  //Low Pass filter D is 2190m


      /*  
               //40To6m rev 38
               FactoryData.HW_Revision = 38; // Hardware revision
               FactoryData.LP_A_BandNum = 11; //Low Pass filter A is 6m
               FactoryData.LP_B_BandNum = 4; //Low Pass filter B is 40m
               FactoryData.LP_C_BandNum = 6; //Low Pass filter C is 20m (+30m)
               FactoryData.LP_D_BandNum = 10;//Low Pass filter D is 10m (+17m + 15m and 12m)
               
               //80To10 rev 38
               FactoryData.HW_Revision = 38  ; // Hardware revision
               FactoryData.LP_A_BandNum = 3; //Low Pass filter A is 80m
               FactoryData.LP_B_BandNum = 4; //Low Pass filter B is 40m
               FactoryData.LP_C_BandNum = 6; //Low Pass filter C is 20m (+30m)
               FactoryData.LP_D_BandNum = 10;//Low Pass filter D is 10m (+17m + 15m and 12m)
              
               //2190To80 rev 38
               FactoryData.HW_Revision = 38; // Hardware revision
               FactoryData.LP_A_BandNum = 3; //Low Pass filter A is 80m
               FactoryData.LP_B_BandNum = 2; //Low Pass filter B is 160m
               FactoryData.LP_C_BandNum = 1; //Low Pass filter C is 6300m
               FactoryData.LP_D_BandNum = 0; //Low Pass filter D is 2190m
               
               //Low Model
               FactoryData.LP_A_BandNum = 0;  //Low Pass filter A is 2190m
               FactoryData.LP_B_BandNum = 1;  //Low Pass filter B is 630m
               FactoryData.LP_C_BandNum = 99; //Low Pass filter C is open circuit
               FactoryData.LP_D_BandNum = 99; //Low Pass filter D is open circuit

      */
    }
    /*
    if (Product_Model == 1029)    //LP1 Model with Mezzanine BLP4 addon, set default LP as a MidPlus version
    {
      FactoryData.HW_Revision = 36; // Hardware revision
      FactoryData.LP_A_BandNum = 2; //Low Pass filter A is 160m
      FactoryData.LP_B_BandNum = 3; //Low Pass filter B is 80m
      FactoryData.LP_C_BandNum = 4; //Low Pass filter C is 40m
      FactoryData.LP_D_BandNum = 6; //Low Pass filter D is 20m
    }
    */

    if (Product_Model == 1028)  //Pico Model, set default LP to 20m
    {
      FactoryData.HW_Revision = 5;    // Hardware revision
      FactoryData.LP_A_BandNum = 6;   //Low Pass filter A is 20m
      FactoryData.LP_B_BandNum = 99;  //Low Pass filter B is open circuit
      FactoryData.LP_C_BandNum = 99;  //Low Pass filter C is open circuit
      FactoryData.LP_D_BandNum = 99;  //Low Pass filter D is open circuit
    }
  }

  if (LoadFromEPROM(UserSpace))  //Read all UserSpace data from EEPROM at position 0
  {
    CurrentMode = GadgetData.StartMode;
    GadgetData.WSPRData.CallSign[6] = 0;     //make sure Call sign is null terminated in case of incomplete data saved
    GadgetData.WSPRData.MaidenHead4[4] = 0;  //make sure Maidenhead locator is null terminated in case of incomplete data saved
    GadgetData.WSPRData.MaidenHead6[6] = 0;  //make sure Maidenhead locator is null terminated in case of incomplete data saved
  } else                                     //No user data was found in EEPROM, set some defaults
  {

    CurrentMode = SignalGen;
    GadgetData.ExtRefFreq = 10000000;
    GadgetData.Name[0] = 'W';
    GadgetData.Name[1] = 'S';
    GadgetData.Name[2] = 'P';
    GadgetData.Name[3] = 'R';
    GadgetData.Name[4] = ' ';
    GadgetData.Name[5] = 'T';
    GadgetData.Name[6] = 'X';
    GadgetData.Name[7] = 0;
    GadgetData.StartMode = Idle;
    GadgetData.WSPRData.CallSign[0] = 'A';
    GadgetData.WSPRData.CallSign[1] = 'A';
    GadgetData.WSPRData.CallSign[2] = '0';
    GadgetData.WSPRData.CallSign[3] = 'A';
    GadgetData.WSPRData.CallSign[4] = 'A';
    GadgetData.WSPRData.CallSign[5] = 'A';
    GadgetData.WSPRData.CallSign[6] = 0;
    GadgetData.WSPRData.LocatorOption = GPS;
    GadgetData.WSPRData.MaidenHead4[0] = 'A';
    GadgetData.WSPRData.MaidenHead4[1] = 'A';
    GadgetData.WSPRData.MaidenHead4[2] = '0';
    GadgetData.WSPRData.MaidenHead4[3] = '0';
    GadgetData.WSPRData.MaidenHead4[4] = 0;  //Null termination
    GadgetData.WSPRData.MaidenHead6[0] = 'A';
    GadgetData.WSPRData.MaidenHead6[1] = 'A';
    GadgetData.WSPRData.MaidenHead6[2] = '0';
    GadgetData.WSPRData.MaidenHead6[3] = '0';
    GadgetData.WSPRData.MaidenHead6[4] = 'A';
    GadgetData.WSPRData.MaidenHead6[5] = 'A';
    GadgetData.WSPRData.MaidenHead6[6] = 0;  //Null termination
    GadgetData.WSPRData.LocationPrecision = 4;
    GadgetData.WSPRData.PowerOption = Normal;  //Use the Power encoding for normal power reporting
    GadgetData.WSPRData.TXPowerdBm = 23;       //Set deafult power to 0.2W
    GadgetData.WSPRData.TimeSlotCode = 16;     //TX on any even minute (no scheduling)
    GadgetData.WSPRData.SuPreFixOption = None;
    GadgetData.WSPRData.Prefix[0] = ' ';  // Prefix three chars max and a zero termination
    GadgetData.WSPRData.Prefix[1] = ' ';
    GadgetData.WSPRData.Prefix[2] = ' ';
    GadgetData.WSPRData.Prefix[3] = 0;  //Null termination
    GadgetData.WSPRData.Sufix = 1;
    GadgetData.WSPRData.GPSConstellations = Cons_GPS;  //Only use GPS system
    if (Product_Model == 1017)                         //The WSPR mini
    {
      GadgetData.WSPRData.TXPowerdBm = 13;  // WSPR Mini has 20mW output power
    }
    if (Product_Model == 1028)  //The WSPR Pico
    {
      GadgetData.WSPRData.TXPowerdBm = 10;        // WSPR Pico has 10mW output power
      GadgetData.WSPRData.CallSign[5] = 'B';      //Set other than default Callsign so it will start WSPR automatically even if not configured, helps in the testing of new devices
      GadgetData.WSPRData.LocationPrecision = 6;  //Use six letter Maidnhead postion reports by transmitting Type3 messages
    }
    for (int i = 0; i < 16; i++) {
      GadgetData.TXOnBand[i] = false;  //Disable TX on all bands.
    }
    GadgetData.TXOnBand[5] = true;          //enable TX on 30m
    GadgetData.TXOnBand[6] = true;          //enable TX on 20m
    GadgetData.TXPause = 480;               //Number of seconds to pause after transmisson
    GadgetData.GeneratorFreq = 1000000000;  // Outtput 10MHz at startup to aid in calibration
    Serial.println(F("{MIN} No user data was found, setting default values"));
  }


  if (FactoryData.HW_Revision > 35)  //On hardware revsion 36 and higher the Relays are driven from different pins on the MCU and there can be an inpput to sense the presense of an external reference oscillator
  {
    Relay1 = 13;
    Relay2 = 15;
    Relay3 = 2;
  } else {
    Relay1 = 14;
    Relay2 = 12;
    Relay3 = 13;
  }



  switch (Product_Model) {
    case 1011:
      Serial.println(F("{MIN} ZachTek WSPR-TX XP transmitter"));
      //De-energize any relays connected to option port
      pinMode(Relay1, OUTPUT);
      pinMode(Relay2, OUTPUT);
      pinMode(Relay3, OUTPUT);
      digitalWrite(Relay1, LOW);
      digitalWrite(Relay2, LOW);
      digitalWrite(Relay3, LOW);
      break;

    case 1012:
      Serial.println(F("{MIN} ZachTek WSPR Desktop transmitter"));
      //De-energize all relays
      pinMode(Relay1, OUTPUT);
      pinMode(Relay2, OUTPUT);
      pinMode(Relay3, OUTPUT);
      digitalWrite(Relay1, LOW);
      digitalWrite(Relay2, LOW);
      digitalWrite(Relay3, LOW);
      break;
      /*
    case 1017:
      Serial.println(F("{MIN} ZachTek WSPR Mini transmitter"));
      //pinMode(SiPower, OUTPUT);
      //digitalWrite(SiPower, LOW);  //Turn on power to the Si5351
      break;  
 */
      /*
    case 1020:
      Serial.println(F("{MIN} ZachTek WSPR-TX XP transmitter with Mezzanine LP4 board"));
      //De-energize all relays
      pinMode(Relay1, OUTPUT);
      pinMode(Relay2, OUTPUT);
      pinMode(Relay3, OUTPUT);
      digitalWrite(Relay1, LOW);
      digitalWrite(Relay2, LOW);
      digitalWrite(Relay3, LOW);
      break;
*/
    case 1024:
      Serial.println(F("{MIN} ZachTek Super Simple Signal Generator"));
      //pinMode(SiPower, OUTPUT);
      //digitalWrite(SiPower, LOW);  //Turn on power to the Si5351
      break;

    case 1028:
      Serial.println(F("{MIN} ZachTek WSPR Pico transmitter"));
      //The Pico is assumed to never be used as a stationary transmitter,
      //it will most likely fly in a ballon beacon so set some settings to avoid a user releasing a ballon with a missconfigured beacon
      GadgetData.WSPRData.LocatorOption = GPS;     // Always set the Locator option to GPS calculated as a failsafe
      GadgetData.WSPRData.PowerOption = Altitude;  // Always encode Altitude in the power field as a failsafe
      CurrentMode = WSPRBeacon;                    // Always boot the WSPR Pico in to beacon mode as a failsafe
      break;
      /*
    case 1029:
      Serial.println(F("{MIN} ZachTek WSPR-TX_LP1 transmitter with Mezzanine BLP4 board"));
      //De-energize all relays
      pinMode(Relay1, OUTPUT);
      pinMode(Relay2, OUTPUT);
      pinMode(Relay3, OUTPUT);
      digitalWrite(Relay1, LOW);
      digitalWrite(Relay2, LOW);
      digitalWrite(Relay3, LOW);
      break;
*/
    case 1048:
      Serial.println(F("{MIN} ZachTek WSPR-TX XP Plus transmitter"));
      pinMode(ExtRefIndicator, INPUT);  //Enable input to detect the use of an external refence oscillator.
      //De-energize any relays connected to option port
      pinMode(Relay1, OUTPUT);
      pinMode(Relay2, OUTPUT);
      pinMode(Relay3, OUTPUT);
      digitalWrite(Relay1, LOW);
      digitalWrite(Relay2, LOW);
      digitalWrite(Relay3, LOW);
      break;
  }


  // Use the Red LED as a Transmitt indicator and the Yellow LED as Status indicator
  pinMode(StatusLED, OUTPUT);
  //pinMode(TransmitLED, OUTPUT);

  Serial.print(F("{MIN} Firmware version "));
  Serial.print(SoftwareVersion);
  Serial.print((":"));
  if (SoftwareRevision < 10) SerialPrintZero();
  Serial.println(SoftwareRevision);


  //Blink StatusLED to indicate Reboot
  LEDBlink(16);
  random(RandomSeed());
  PowerSaveOFF();
  DetectSi5351I2CAddress();
  GPSSetConstellation(GadgetData.WSPRData.GPSConstellations);
  if (Product_Model == 1048)  // If the hardware has capability for external frequency reference then check it's status and present
  {
    if (digitalRead(ExtRefIndicator) == LOW) Serial.println(F("{MIN} Using External reference"));
  }

  // ..........................  RobZ  ...........................
  WiFi_init();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Now connecting to Timeserver ");
    iniNTP();
  }
  // --------------------- end RobZ -----------------------------

  switch (CurrentMode) {
    case SignalGen:
      DoSignalGen();
      break;

    case WSPRBeacon:
      CurrentBand = 0;
      DoWSPR();
      break;

    case Idle:
      DoIdle();
      break;
  }
}



void loop() {

  if (Serial.available()) {  //Handle  Serial API request from the PC
    DoSerialHandling();
  }
  if (CurrentMode == WSPRBeacon) DoWSPR();  //If in WSPR beacon mode but it broke out of beacon loop to handle a Serial data from the PC then go back to the WSPR routine
  while (gps.available(GPSSerial)) {        //Handle Serial data from the GPS as they arrive
    fix = gps.read();
    SendAPIUpdate(UMesTime);
    LoopGPSNoReceiveCount = 0;
    if ((GPSS % 4) == 0)  //Send some nice-to-have info every 4 seconds, this is a lot of data so we dont want to send it to often to risk choke the Serial output buffer
    {
      SendSatData();                   //Send Satellite position and SNR information to the PC GUI
      SendAPIUpdate(UMesVCC);          //Send power supply voltage at the MCU to the PC GUI
      SendAPIUpdate(UMesCurrentMode);  // Send info of what routine is running to the PC GUI
      if (fix.valid.location && fix.valid.time) {
        SendAPIUpdate(UMesGPSLock);
        if (GadgetData.WSPRData.LocatorOption == GPS) {  //If GPS should update the Maidenhead locator
          calcLocator(fix.latitude(), fix.longitude());
        }
        SendAPIUpdate(UMesLocator);
      } else {
        SendAPIUpdate(UMesNoGPSLock);
      }
    }
    smartdelay(200);
  }
  LoopGPSNoReceiveCount++;
  if (LoopGPSNoReceiveCount > 600000)  //GPS have not sent anything for a long time, GPS is possible in sleep mode or has not started up correctly. This can happen if a brown-out/reboot happens while the GPS was sleeping
  {
    LoopGPSNoReceiveCount = 0;
    Serial.println(F("{MIN} Resetting GPS"));
    GPSWakeUp();  //Try to get GPS going again by sending wake up command
    smartdelay(2000);
  }
}



//Parts from NickGammon Serial Input example
//http://www.gammon.com.au/serial
void DoSerialHandling() {
  static char SerialLine[SerCMDLength];  //A single line of incoming serial command and data
  static uint8_t input_pos = 0;
  char InChar;
  PCConnected = true;
  while (Serial.available() > 0) {
    InChar = Serial.read();
    switch (InChar) {
      case '\n':                    // end of text
        SerialLine[input_pos] = 0;  // terminating null byte
        // terminator reached, process Command
        DecodeSerialCMD(SerialLine);
        // reset buffer for next time
        input_pos = 0;
        break;

      case '\r':  // discard carriage return
        break;

      default:
        // keep adding if not full ... allow for terminating null byte
        if (input_pos < (SerCMDLength - 1))
          SerialLine[input_pos++] = InChar;
        break;

    }  // end of switch
  }    // end of processIncomingByte
}


//Serial API commands and data decoding
void DecodeSerialCMD(const char *InputCMD) {
  char CharInt[13];
  bool EnabDisab;
  uint32_t i;
  if ((InputCMD[0] == '[') && (InputCMD[4] == ']')) {  //A Command,Option or Data input

    if (InputCMD[1] == 'C') {  //Commmand

      //Current Mode
      if ((InputCMD[2] == 'C') && (InputCMD[3] == 'M')) {
        if (InputCMD[6] == 'S') {  //Set option
          if (InputCMD[8] == 'S') {
            DoSignalGen();
          }
          if (InputCMD[8] == 'W') {
            //CurrentBand = 0;
            DoWSPR();
          }
          if (InputCMD[8] == 'N') {
            DoIdle();
          }
        }     //Set Current Mode
        else  //Get
        {
          SendAPIUpdate(UMesCurrentMode);
        }  //Get Current Mode
      }    //[CCM]


      //Check what frequency Reference is in use, External or Internal  [CCR]
      if ((InputCMD[2] == 'C') && (InputCMD[3] == 'R')) {
        if (InputCMD[6] == 'G')  //Get option
        {
          Serial.print(F("{CCR} "));
          if (Product_Model == 1048)  //If the product supports external reference then scheck if in use.
          {
            if (digitalRead(ExtRefIndicator) == LOW) {
              Serial.println("E");
            } else {
              Serial.println("I");
            }
          } else  //This hardware only has an internal frequency reference
          {
            Serial.println("I");
          }
        }  //Get
      }    //[CCR]


      //Store Current configuration data to EEPROM [CSE]
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'E')) {
        if (InputCMD[6] == 'S') {  //Set option
          SaveToEEPROM(UserSpace);
          Serial.println(F("{MIN} Configuration saved"));
        }
      }  //[CSE]

// ------------------------- RobZ -----------------------------------------
      //NEW COMMAND - Clear WiFi settings and restart [CCW]
      if ((InputCMD[2] == 'C') && (InputCMD[3] == 'W')) {
        if (InputCMD[6] == 'S') {  //Set option
          resetWiFi();
        }
      }  //[CCW]
//---------------------------- End RobZ ----------------------------------------

      //Set Low Pass filter. LP filters are automatically set by the WSPR Beacon and Signal Gen. routines
      //but can be temporarily overrided by this command for testing purposes [CSL]
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'L')) {
        if (InputCMD[6] == 'S') {  //Set option
          if (InputCMD[8] == 'A') {
            CurrentLP = 0;
          }
          if (InputCMD[8] == 'B') {
            CurrentLP = 1;
          }
          if (InputCMD[8] == 'C') {
            CurrentLP = 2;
          }
          if (InputCMD[8] == 'D') {
            CurrentLP = 3;
          }
          DriveLPFilters();
        }  //[CSL]
      }
      exit;
    }  //End of command section

    if (InputCMD[1] == 'O') {  //Option

      //TX Pause
      if ((InputCMD[2] == 'T') && (InputCMD[3] == 'P')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = InputCMD[10];
          CharInt[3] = InputCMD[11];
          CharInt[4] = InputCMD[12];
          CharInt[5] = 0;
          //GadgetData.TXPause = atoi(CharInt);
          GadgetData.TXPause = StrTouint64_t(CharInt);
        } else  //Get Option
        {
          Serial.print(F("{OTP} "));
          if (GadgetData.TXPause < 10000) SerialPrintZero();
          if (GadgetData.TXPause < 1000) SerialPrintZero();
          if (GadgetData.TXPause < 100) SerialPrintZero();
          if (GadgetData.TXPause < 10) SerialPrintZero();
          Serial.println(GadgetData.TXPause);
        }
      }  //TX Pause


      //StartMode [OSM]
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'M')) {
        if (InputCMD[6] == 'S') {  //Set option
          if (InputCMD[8] == 'S') {
            GadgetData.StartMode = SignalGen;
          }
          if (InputCMD[8] == 'W') {
            GadgetData.StartMode = WSPRBeacon;
          }
          if (InputCMD[8] == 'N') {
            GadgetData.StartMode = Idle;
          }
        }     //Set Start Mode
        else  //Get
        {
          Serial.print(F("{OSM} "));
          switch (GadgetData.StartMode) {
            case Idle:
              Serial.println(("N"));
              break;
            case WSPRBeacon:
              Serial.println(("W"));
              break;
            case SignalGen:
              Serial.println(("S"));
              break;
          }
        }  //Get Start Mode
      }    //StartMode

      //Band TX enable
      if ((InputCMD[2] == 'B') && (InputCMD[3] == 'D')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = 0;
          CharInt[3] = 0;  //What band to set/clear
          EnabDisab = false;
          if (InputCMD[11] == 'E') EnabDisab = true;
          GadgetData.TXOnBand[atoi(CharInt)] = EnabDisab;  //Enable or disable on this band
        }                                                  //Set Band TX enable
        else                                               //Get
        {
          //Get Option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = 0;
          CharInt[3] = 0;  //What band is requested
          Serial.print(F("{OBD} "));
          i = atoi(CharInt);
          if (i < 10) SerialPrintZero();
          Serial.print(i);
          if (GadgetData.TXOnBand[i]) {
            Serial.println((" E"));
          } else {
            Serial.println((" D"));
          }
        }  //Get Band TX enable
      }    //Band TX enable

      //Location Option
      if ((InputCMD[2] == 'L') && (InputCMD[3] == 'C')) {
        if (InputCMD[6] == 'S') {  //Set Location Option
          if (InputCMD[8] == 'G') {
            GadgetData.WSPRData.LocatorOption = GPS;
            Serial.println(F("{OLC G} "));             //Echo back setting
            if (fix.valid.location && fix.valid.time)  //If position is known then send it to the PC
            {
              GPSH = fix.dateTime.hours;
              GPSM = fix.dateTime.minutes;
              GPSS = fix.dateTime.seconds;
              calcLocator(fix.latitude(), fix.longitude());
              Serial.print(F("{DL4} "));
              Serial.println(GadgetData.WSPRData.MaidenHead4);
              Serial.print(F("{DL6} "));
              Serial.println(GadgetData.WSPRData.MaidenHead6);
            }
          }
          if (InputCMD[8] == 'M') {
            GadgetData.WSPRData.LocatorOption = Manual;
          }
        }     //Set Location Option
        else  //Get Location Option
        {
          Serial.print(F("{OLC} "));
          if (GadgetData.WSPRData.LocatorOption == GPS) {
            Serial.println(("G"));
          } else {
            Serial.println(("M"));
          }
        }  //Get Location Option
      }    //Location Option

      //Locator Precision [OLP]
      if ((InputCMD[2] == 'L') && (InputCMD[3] == 'P')) {
        if (InputCMD[6] == 'S') {  //Set Locator Precision
          if (InputCMD[8] == '6') {
            GadgetData.WSPRData.LocationPrecision = 6;
          } else {
            GadgetData.WSPRData.LocationPrecision = 4;
          }
          //Echo back setting
          //Serial.print (F("{OLP} "));
          //Serial.println (GadgetData.WSPRData.LocationPrecision);
        }     //Set Locator Precision
        else  //Get Locator Precision
        {
          Serial.print(F("{OLP} "));
          Serial.println(GadgetData.WSPRData.LocationPrecision);
        }  //Get Locator Precision
      }    //Locator Precision

      //Power encoding Option
      if ((InputCMD[2] == 'P') && (InputCMD[3] == 'W')) {
        if (InputCMD[6] == 'S') {  //Set Location Option
          if (InputCMD[8] == 'N') {
            GadgetData.WSPRData.PowerOption = Normal;
          }
          if (InputCMD[8] == 'A') {
            GadgetData.WSPRData.PowerOption = Altitude;
          }
        }     //Set Power Encoding Option
        else  //Get Location Option
        {
          Serial.print(F("{OPW} "));
          if (GadgetData.WSPRData.PowerOption == Normal) {
            Serial.println(("N"));
          } else {
            Serial.println(("A"));
          }
        }  //Get Power Encoding Option
      }    //Power encoding Option
      exit;

      //Time slot  [OTS]
      if ((InputCMD[2] == 'T') && (InputCMD[3] == 'S')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = 0;
          CharInt[3] = 0;
          GadgetData.WSPRData.TimeSlotCode = atoi(CharInt);
        } else  //Get
        {
          Serial.print(F("{OTS} "));
          if (GadgetData.WSPRData.TimeSlotCode < 10) SerialPrintZero();
          Serial.println(GadgetData.WSPRData.TimeSlotCode);
        }
      }  //Time slot

      //PreFix/Suffix [OPS]
      if ((InputCMD[2] == 'P') && (InputCMD[3] == 'S')) {
        if (InputCMD[6] == 'S') {  //Set option
          if (InputCMD[8] == 'P') {
            GadgetData.WSPRData.SuPreFixOption = Prefix;
          }
          if (InputCMD[8] == 'S') {
            GadgetData.WSPRData.SuPreFixOption = Sufix;
          }
          if (InputCMD[8] == 'N') {
            GadgetData.WSPRData.SuPreFixOption = None;
          }
        }     //Set Prefix/Suffix
        else  //Get
        {
          Serial.print(F("{OPS} "));
          switch (GadgetData.WSPRData.SuPreFixOption) {
            case Prefix:
              Serial.println(("P"));
              break;
            case Sufix:
              Serial.println(("S"));
              break;
            case None:
              Serial.println(("N"));
              break;
          }
        }  //Get Prefix/Suffix
      }    //PreFix/Suffix

      //Satellite constellations option [OSC]
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'C')) {
        if (InputCMD[6] == 'S') {  //Set option
          if (InputCMD[8] == 'G') {
            GadgetData.WSPRData.GPSConstellations = Cons_GPS;
          }
          if (InputCMD[8] == 'B') {
            GadgetData.WSPRData.GPSConstellations = Cons_BeiDou;
          }
          if (InputCMD[8] == 'A') {
            GadgetData.WSPRData.GPSConstellations = Cons_GPSAndBeiDou;
          }
          GPSSetConstellation(GadgetData.WSPRData.GPSConstellations);
        }     //Set Satellite constellations
        else  //Get
        {
          Serial.print(F("{OSC} "));
          switch (GadgetData.WSPRData.GPSConstellations) {
            case Cons_GPS:
              Serial.println(("G"));
              break;
            case Cons_BeiDou:
              Serial.println(("B"));
              break;
            case Cons_GPSAndBeiDou:
              Serial.println(("A"));
              break;
          }
        }  //Get Satellite constellations
      }    //Satellite constellations
    }      //All Options

    //Data
    if (InputCMD[1] == 'D') {

      //Callsign [DCS]
      if ((InputCMD[2] == 'C') && (InputCMD[3] == 'S')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 5; i++) {
            GadgetData.WSPRData.CallSign[i] = InputCMD[i + 8];
          }
          GadgetData.WSPRData.CallSign[6] = 0;
        } else  //Get
        {
          Serial.print(F("{DCS} "));
          Serial.println(GadgetData.WSPRData.CallSign);
        }
      }  //Callsign

      //Callsign Sufix [DSF]
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'F')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = InputCMD[10];
          CharInt[3] = 0;
          GadgetData.WSPRData.Sufix = atoi(CharInt);
        } else  //Get
        {
          Serial.print(F("{DSF} "));
          if (GadgetData.WSPRData.Sufix < 100) SerialPrintZero();
          if (GadgetData.WSPRData.Sufix < 10) SerialPrintZero();
          Serial.println(GadgetData.WSPRData.Sufix);
        }
      }  //Callsign Sufix

      //Callsing Prefix  [DPF]
      if ((InputCMD[2] == 'P') && (InputCMD[3] == 'F')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 2; i++) {
            GadgetData.WSPRData.Prefix[i] = InputCMD[i + 8];
          }
          GadgetData.WSPRData.Prefix[3] = 0;
        } else  //Get
        {
          Serial.print(F("{DPF} "));
          Serial.println(GadgetData.WSPRData.Prefix);
        }
      }  //Callsign Prefix

      //Locator 4
      if ((InputCMD[2] == 'L') && (InputCMD[3] == '4')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 3; i++) {
            GadgetData.WSPRData.MaidenHead4[i] = InputCMD[i + 8];
          }
          GadgetData.WSPRData.MaidenHead4[4] = 0;
        } else  //Get
        {
          Serial.print(F("{DL4} "));
          Serial.println(GadgetData.WSPRData.MaidenHead4);
        }
      }  //Locator 4

      //Locator 6
      if ((InputCMD[2] == 'L') && (InputCMD[3] == '6')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 5; i++) {
            GadgetData.WSPRData.MaidenHead6[i] = InputCMD[i + 8];
          }
          GadgetData.WSPRData.MaidenHead6[6] = 0;
        } else  //Get
        {
          Serial.print(F("{DL6} "));
          Serial.println(GadgetData.WSPRData.MaidenHead6);
        }
      }  //Locator 6

      //Name
      if ((InputCMD[2] == 'N') && (InputCMD[3] == 'M')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 38; i++) {
            GadgetData.Name[i] = InputCMD[i + 8];
          }
          GadgetData.Name[39] = 0;
        } else  //Get
        {
          Serial.print(F("{DNM} "));
          Serial.println(GadgetData.Name);
        }
      }  //Name

      //Power data
      if ((InputCMD[2] == 'P') && (InputCMD[3] == 'D')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = 0;
          CharInt[3] = 0;
          GadgetData.WSPRData.TXPowerdBm = atoi(CharInt);
        } else  //Get
        {
          Serial.print(F("{DPD} "));
          if (GadgetData.WSPRData.TXPowerdBm < 10) SerialPrintZero();
          Serial.println(GadgetData.WSPRData.TXPowerdBm);
        }
      }  //Power Data


      //Generator Frequency
      if ((InputCMD[2] == 'G') && (InputCMD[3] == 'F')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 11; i++) {
            CharInt[i] = InputCMD[i + 8];
          }
          CharInt[12] = 0;
          GadgetData.GeneratorFreq = StrTouint64_t(CharInt);
          if (CurrentMode == SignalGen) DoSignalGen();
        } else  //Get
        {
          Serial.print(F("{DGF} "));
          Serial.println(uint64ToStr(GadgetData.GeneratorFreq, true));
        }
      }  //Generator Frequency

      //External Reference Oscillator Frequency
      if ((InputCMD[2] == 'E') && (InputCMD[3] == 'R')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 8; i++) {
            CharInt[i] = InputCMD[i + 8];
          }
          CharInt[9] = 0;
          GadgetData.ExtRefFreq = StrTouint64_t(CharInt);
        } else  //Get
        {
          Serial.print(F("{DER} "));
          Serial.println(uint64ToStr(GadgetData.ExtRefFreq, true));
        }
      }  //External Reference Oscillator Frequency

      exit;
    }  //Data

    //Factory data
    if (InputCMD[1] == 'F') {

      //Product model Number
      if ((InputCMD[2] == 'P') && (InputCMD[3] == 'N')) {
        if (InputCMD[6] == 'G') {  //Get option
          Serial.print(F("{FPN} "));
          if (Product_Model < 10000) SerialPrintZero();
          Serial.println(Product_Model);
        }
      }  //Product model Number

      //Hardware Version
      if ((InputCMD[2] == 'H') && (InputCMD[3] == 'V')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = InputCMD[10];
          CharInt[3] = 0;
          FactoryData.HW_Version = atoi(CharInt);
        }     //Set
        else  //Get Option
        {
          Serial.print(F("{FHV} "));
          if (FactoryData.HW_Version < 100) SerialPrintZero();
          if (FactoryData.HW_Version < 10) SerialPrintZero();
          Serial.println(FactoryData.HW_Version);
        }
      }  //Hardware Version

      //Hardware Revision
      if ((InputCMD[2] == 'H') && (InputCMD[3] == 'R')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[8];
          CharInt[1] = InputCMD[9];
          CharInt[2] = InputCMD[10];
          CharInt[3] = 0;
          FactoryData.HW_Revision = atoi(CharInt);
          Serial.println(' ');
        }     //Set
        else  //Get Option
        {
          Serial.print(F("{FHR} "));
          if (FactoryData.HW_Revision < 100) SerialPrintZero();
          if (FactoryData.HW_Revision < 10) SerialPrintZero();
          Serial.println(FactoryData.HW_Revision);
        }
      }  //Hardware Revision

      //Software Version
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'V')) {
        if (InputCMD[6] == 'G') {  //Get option
          Serial.print(F("{FSV} "));
          if (SoftwareVersion < 100) SerialPrintZero();
          if (SoftwareVersion < 10) SerialPrintZero();
          Serial.println(SoftwareVersion);
        }
      }  //Software Version

      //Software Revision
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'R')) {
        if (InputCMD[6] == 'G') {  //Get option
          Serial.print(F("{FSR} "));
          if (SoftwareRevision < 100) SerialPrintZero();
          if (SoftwareRevision < 10) SerialPrintZero();
          Serial.println(SoftwareRevision);
        }
      }  //Software Revision

      //Low pass filter config
      if ((InputCMD[2] == 'L') && (InputCMD[3] == 'P')) {
        if (InputCMD[6] == 'S') {  //Set option
          CharInt[0] = InputCMD[10];
          CharInt[1] = InputCMD[11];
          CharInt[2] = 0;
          switch (InputCMD[8]) {
            case 'A':
              FactoryData.LP_A_BandNum = atoi(CharInt);
              break;
            case 'B':
              FactoryData.LP_B_BandNum = atoi(CharInt);
              break;
            case 'C':
              FactoryData.LP_C_BandNum = atoi(CharInt);
              break;
            case 'D':
              FactoryData.LP_D_BandNum = atoi(CharInt);
              break;
          }

        }     //Set
        else  //Get Option
        {
          //If Hardvare is Desktop then soome filters can do more than one band, indicate by sending out these extra bands to the PC config software
          //The same goes for the Pico and the LP1 with Mezzanine BLP4
          //The PC will indicate these bands with the little green square in the GUI
          if ((Product_Model == 1012) || (Product_Model == 1028) || (Product_Model == 1029)) {
            //If 10m LP filter is fitted then indicate it can do 15m and 12m as well
            if ((FactoryData.LP_A_BandNum == 10) || (FactoryData.LP_B_BandNum == 10) || (FactoryData.LP_C_BandNum == 10) || (FactoryData.LP_D_BandNum == 10)) {
              Serial.println(F("{FLP} A 09"));  //Indicate 12m band
              Serial.println(F("{FLP} A 08"));  //Indicate 15m band
              Serial.println(F("{FLP} A 07"));  //Indicate 17m band
            }
            //If 20m LP filter is fitted then indicate it can do 30m as well
            if ((FactoryData.LP_A_BandNum == 6) || (FactoryData.LP_B_BandNum == 6) || (FactoryData.LP_C_BandNum == 6) || (FactoryData.LP_D_BandNum == 6)) {
              Serial.println(F("{FLP} A 05"));  //Indicate 30m band
            }
          }
          Serial.print(F("{FLP} A "));
          if (FactoryData.LP_A_BandNum < 10) SerialPrintZero();
          Serial.println(FactoryData.LP_A_BandNum);
          Serial.print(F("{FLP} B "));
          if (FactoryData.LP_B_BandNum < 10) SerialPrintZero();
          Serial.println(FactoryData.LP_B_BandNum);
          Serial.print(F("{FLP} C "));
          if (FactoryData.LP_C_BandNum < 10) SerialPrintZero();
          Serial.println(FactoryData.LP_C_BandNum);
          Serial.print(F("{FLP} D "));
          if (FactoryData.LP_D_BandNum < 10) SerialPrintZero();
          Serial.println(FactoryData.LP_D_BandNum);
        }
      }  //Low pass filter config

      //Reference Oscillator Frequency
      if ((InputCMD[2] == 'R') && (InputCMD[3] == 'F')) {
        if (InputCMD[6] == 'S') {  //Set option
          for (int i = 0; i <= 8; i++) {
            CharInt[i] = InputCMD[i + 8];
          }
          CharInt[9] = 0;
          FactoryData.RefFreq = StrTouint64_t(CharInt);

        } else  //Get
        {
          Serial.print(F("{FRF} "));
          Serial.println(uint64ToStr(FactoryData.RefFreq, true));
        }
      }  //Reference Oscillator Frequency

      //Store Current Factory configuration data to EEPROM
      if ((InputCMD[2] == 'S') && (InputCMD[3] == 'E')) {
        if (InputCMD[6] == 'S') {  //Set option
          SaveToEEPROM(FactorySpace);
          Serial.println(F("{MIN} Factory data saved"));
        }
      }

      exit;
    }  //Factory
  }
}


uint64_t StrTouint64_t(String InString) {
  uint64_t y = 0;

  for (int i = 0; i < InString.length(); i++) {
    char c = InString.charAt(i);
    if (c < '0' || c > '9') break;
    y *= 10;
    y += (c - '0');
  }
  return y;
}

String uint64ToStr(uint64_t p_InNumber, boolean p_LeadingZeros) {
  char l_HighBuffer[7];     //6 digits + null terminator char
  char l_LowBuffer[7];      //6 digits + null terminator char
  char l_ResultBuffer[13];  //12 digits + null terminator char
  String l_ResultString = "";
  uint8_t l_Digit;

  sprintf(l_HighBuffer, "%06lu", p_InNumber / 1000000L);  //Convert high part of 64bit unsigned integer to char array
  sprintf(l_LowBuffer, "%06lu", p_InNumber % 1000000L);   //Convert low part of 64bit unsigned integer to char array
  l_ResultString = l_HighBuffer;
  l_ResultString = l_ResultString + l_LowBuffer;  //Copy the 2 part result to a string

  if (!p_LeadingZeros)  //If leading zeros should be removed
  {
    l_ResultString.toCharArray(l_ResultBuffer, 13);
    for (l_Digit = 0; l_Digit < 12; l_Digit++) {
      if (l_ResultBuffer[l_Digit] == '0') {
        l_ResultBuffer[l_Digit] = ' ';  // replace zero with a space character
      } else {
        break;  //We have found all the leading Zeros, exit loop
      }
    }
    l_ResultString = l_ResultBuffer;
    l_ResultString.trim();  //Remove all leading spaces
  }
  return l_ResultString;
}


void DoSignalGen() {
  if (Si5351I2CAddress == 0) {
    Serial.println(F("{MIN}Hardware ERROR! No Si5351 PLL device found on the I2C buss!"));
  } else {
    CurrentMode = SignalGen;
    freq = GadgetData.GeneratorFreq;
    PickLP(FreqToBand());  //Use the correct low pass filter
    si5351aSetFrequency(freq);
    digitalWrite(StatusLED, LOW);
    //digitalWrite(TransmitLED, HIGH);
    SendAPIUpdate(UMesCurrentMode);
    SendAPIUpdate(UMesFreq);
  }
}


void DoIdle() {
  PowerSaveOFF();
  CurrentMode = Idle;
  digitalWrite(StatusLED, HIGH);
  //digitalWrite(TransmitLED, LOW);
  si5351aOutputOff(SI_CLK0_CONTROL);
  SendAPIUpdate(UMesCurrentMode);
}


void DoWSPR() {
  int i;
  uint8_t pwr1, pwr2;  //Used in Altitude to power reporting (balloon coding)
  uint32_t AltitudeInMeter;
  boolean ConfigError;
  // uint32_t GPSNoReceiveCount; //If GPS stops working in WSPR Beacon mode this will increment
  int WSPRMessageTypeToUse;

  if (GadgetData.WSPRData.SuPreFixOption == None)  //if standard Call Sign with no Sufix then send a Standards Type 1 message, else Send a Type 2 Message to include the Sufix
  {
    WSPRMessageTypeToUse = 1;
  } else {
    WSPRMessageTypeToUse = 2;
  }
  if (Si5351I2CAddress == 0) {
    Serial.println(F("{MIN}Hardware ERROR! No Si5351 PLL device found on the I2C buss!"));
  } else {
    CurrentMode = WSPRBeacon;
    ConfigError = false;

    //Make sure at least one band is enabled for tranmission
    if (NoBandEnabled()) {
      Serial.println(F("{MIN}Tranmission is not enabled on any band"));
      ConfigError = true;
    }

    //Make sure  call sign is set
    if ((GadgetData.WSPRData.CallSign[0] == 'A') && (GadgetData.WSPRData.CallSign[1] == 'A') && (GadgetData.WSPRData.CallSign[2] == '0') && (GadgetData.WSPRData.CallSign[3] == 'A') && (GadgetData.WSPRData.CallSign[4] == 'A') && (GadgetData.WSPRData.CallSign[5] == 'A'))  //Do not actually key the transmitter if the callsign has not been changed from the default one AA0AAA
    {
      Serial.println(F("{MIN}Call Sign not set"));
      ConfigError = true;
    }

    if (ConfigError) {
      Serial.println(F("{MIN}Can not start WSPR Beacon"));
      DoIdle();  // Go back to ideling
    } else {
      CurrentBand = 0;
      NextFreq();                                  //Cycle to next enabled band to transmit on
      freq = freq + (100ULL * random(-100, 100));  //modify TX frequency with a random value beween -100 and +100 Hz
      si5351aOutputOff(SI_CLK0_CONTROL);
      SendAPIUpdate(UMesCurrentMode);

      //LOOP HERE FOREVER OR UNTIL INTERRUPTED BY A SERIAL COMMAND
      while (!Serial.available()) {         //Do until incoming serial command
        while (gps.available(GPSSerial)) {  //If GPS data is available - process it
          LoopGPSNoReceiveCount = 0;
          fix = gps.read();
          SendAPIUpdate(UMesTime);
          if (Serial.available()) {  // If serialdata was received on control port then handle command
            return;
          }
          if (fix.valid.location && fix.valid.time) {
            GPSH = fix.dateTime.hours;
            GPSM = fix.dateTime.minutes;
            GPSS = fix.dateTime.seconds;
            if (GadgetData.WSPRData.LocatorOption == GPS) {  //If GPS should update the Maidenhead locator
              calcLocator(fix.latitude(), fix.longitude());
            }
            if ((GPSS == 00) && (CorrectTimeslot()))  //If second is zero at even minute then start WSPR transmission. The function CorrectTimeSlot can hold of transmision depending on several user settings. The GadgetData.WSPRData.TimeSlotCode value will influense the behaviour
            {
              if ((PCConnected) || (Product_Model != 1028) || ((Product_Model == 1028) && OutsideGeoFence()))  //On the WSPR-TX Pico make sure were are outside the territory of UK, Yemen and North Korea before the transmitter is started but allow tranmissions inside the Geo-Fence if a PC is connected so UK users can make test tranmissions on the ground before relase of Picos
              {
                GPSGoToSleep();  //Put GPS to sleep to save power
                // -------------------- Altitude coding to Power ------------------------------------
                if (GadgetData.WSPRData.PowerOption == Altitude)  // If Power field should be used for Altitude coding
                {
                  AltitudeInMeter = (uint32_t)fix.altitude();
                  pwr1 = ValiddBmValue(AltitudeInMeter / 300);                  //Max 18km altitude, every dBm count as 300m and max dBm that can be reported is 60
                  pwr2 = ValiddBmValue((AltitudeInMeter - (pwr1 * 300)) / 20);  //Finer calculations for the second power transmission (if any - depends on user setting) every dBm in this report is 20m. The two reports will be added on the receive side
                  GadgetData.WSPRData.TXPowerdBm = pwr1;
                }

                if (SendWSPRMessage(WSPRMessageTypeToUse) != 0)  //Send a WSPR Type 1 or Type 2 message for 1 minute and 50 seconds
                {
                  // there was a serial command that interrupted the WSPR Block so go and handle it
                  return;
                }
                if (GadgetData.WSPRData.LocationPrecision == 6)  //If higher position precision is set then start a new WSPR tranmission of Type 3
                {
                  delay(9000);                                      //wait 9 seconds so we are at the top of an even minute again
                  if (GadgetData.WSPRData.PowerOption == Altitude)  // If Power field should be used for Altitude coding
                  {
                    GadgetData.WSPRData.TXPowerdBm = pwr2;
                  }
                  if (SendWSPRMessage(3) != 0)  //Send a WSPR Type 3 message for 1 minute and 50 seconds
                  {
                    // there was a serial command that interrupted the WSPR Block so go and handle it
                    return;
                  }
                }
                StorePosition();  //Save the current position;
                if (LastFreq())   //If all bands have been transmitted on then pause for user defined time and after that start over on the first band again
                {
                  if ((GadgetData.TXPause > 60) && ((Product_Model == 1017) || (Product_Model == 1028)) && (!PCConnected))  //If the PC is not connected and the TXdelay is longer than a 60 sec then put the MCU to sleep to save current during this long pause (Mini and Pico models only)
                  {
                    delay(600);  //Let the serial port send data from its buffer before we go to sleep
                    //Si5351PowerOff ();                             //Turn off the PLL to save power (Mini Only)
                    //MCUGoToSleep (GadgetData.TXPause - 10);        //Set MCU in sleep mode until there is 10 seconds left of delay
                    PowerSaveOFF();                           //We are back from sleep - turn on GPS and PLL again
                    smartdelay(2000);                         // let the smartdelay routine read a few GPS lines so we can get the new GPS time after our sleep
                  } else {                                    //Regular pause if we did not go to sleep then do a regular pause and send updates to the GUI for the duration
                    smartdelay(GadgetData.TXPause * 1000UL);  //Pause for the time set by the user
                  }
                  SendAPIUpdate(UMesWSPRBandCycleComplete);  //Inform PC that we have transmitted on the last enabled WSPR band and will start over
                }
                GPSWakeUp();
                NextFreq();                                  // get the frequency for the next HAM band that we will transmit on
                freq = freq + (100ULL * random(-100, 100));  //modify the TX frequency with a random value beween -100 and +100 Hz to avoid possible lengthy colisions with other users on the band
                smartdelay(3000);
              }
            } else  //We have GPS fix but it is not top of even minute so dubble-blink to indicate waiting for top of minute
            {
              //SendAPIUpdate(UMesTime);
              if (GPSS < 57)  //Send some nice-to-have info only if the WSPR start is at least 3 seconds away. The last 3 seconds we want to do as little as possible so we can time the start of transmission exactly on the mark
              {
                SendAPIUpdate(UMesGPSLock);  //Send Locked status
                SendAPIUpdate(UMesLocator);  //Send position
                SendSatData();               //Send Satellite postion and SNR information to the PC GUI
              }
              LEDBlink(2);
              smartdelay(100);
            }
          } else {                         //Waiting for GPS location fix
            SendSatData();                 //Send Satellite postion and SNR information to the PC GUI while we wait for the GPS location fix
            LEDBlink(1);                   //singleblink to indicate waiting for GPS Lock
            SendAPIUpdate(UMesNoGPSLock);  //Send No lock status
            // .................. RobZ ......................
            //smartdelay(400);
            if (timeClient.isTimeSet()) {
              noGPS_WSPR_Tx(WSPRMessageTypeToUse);
            } else smartdelay(400);
            // ------------------ end RobZ -------------------
          }
        }  //GPS serial data loop
        LoopGPSNoReceiveCount++;
        if (LoopGPSNoReceiveCount > 600000)  //GPS have not sent anything for a long time, GPS is possible in sleep mode or has not started up correctly. This can happen if a brown-out/reboot happens while the GPS was sleeping
        {
          LoopGPSNoReceiveCount = 0;
          Serial.println(F("{MIN} Resetting GPS"));
          GPSReset();  //Try to get GPS going again
          smartdelay(2000);
        }
      }  //Incoming serial command
    }
  }
}


// Transmitt a WSPR message for 1 minute 50 seconds on frequency freq
int SendWSPRMessage(uint8_t WSPRMessageType) {
  uint8_t i;
  uint8_t Indicator;
  uint8_t BlinkCount;
  unsigned long startmillis;
  unsigned long endmillis;
  boolean TXEnabled = true;
  int errcode;
  errcode = 0;
  boolean blinked;
  memset(tx_buffer, 0, sizeof(tx_buffer));                                                                                                 //clear WSPR symbol buffer
  wspr_encode(GadgetData.WSPRData.CallSign, GadgetData.WSPRData.MaidenHead4, GadgetData.WSPRData.TXPowerdBm, tx_buffer, WSPRMessageType);  //Send a WSPR message for 2 minutes
  //PrintBuffer ('B');
  // Send WSPR for two minutes

  digitalWrite(StatusLED, LOW);
  //digitalWrite(TransmitLED, HIGH);
  startmillis = millis();
  for (i = 0; i < 162; i++)  //162 WSPR symbols to transmit
  {
    blinked = false;
    endmillis = startmillis + ((i + 1) * (unsigned long)683);  // intersymbol delay in WSPR is 682.687 milliseconds (1.4648 baud)
    uint64_t tonefreq;
    tonefreq = freq + ((tx_buffer[i] * 146));  //146 centiHz (Tone spacing is 1.4648Hz in WSPR)
    if (TXEnabled) si5351aSetFrequency(tonefreq);
    //wait untill tone is transmitted for the correct amount of time
    while ((millis() < endmillis) && (!Serial.available()))
      ;  //Until time is up or there is serial data received on the control Serial port
    {
      if (!blinked) {  //do pulsing blinks on Status LED every WSPR symbol to indicate WSPR Beacon transmission
        //Send Status updates to the PC
        Indicator = i;
        Serial.print(F("{TWS} "));
        if (CurrentBand < 10) SerialPrintZero();
        Serial.print(CurrentBand);
        Serial.print(" ");
        if (GadgetData.WSPRData.LocationPrecision == 6) Indicator = Indicator / 2;  //If four minutes TX time then halve the indicator value so it will be full after four minutes instead of 2 minutes
        if (WSPRMessageType == 3) Indicator = Indicator + 81;                       //If this is the second 2 minute transmission then start to from 50%
        if (Indicator < 10) SerialPrintZero();
        if (Indicator < 100) SerialPrintZero();
        Serial.println(Indicator);
        for (int BlinkCount = 0; BlinkCount < 6; BlinkCount++) {
          digitalWrite(StatusLED, LOW);
          delay(5);
          digitalWrite(StatusLED, HIGH);
          delay(50);
        }
        blinked = true;
      }
    }
    if (Serial.available())  // If serialdata was received on Control port then abort and handle command
    {
      errcode = 1;
      break;
    }
  }
  // Switches off Si5351a output
  si5351aOutputOff(SI_CLK0_CONTROL);
  digitalWrite(StatusLED, HIGH);
  //digitalWrite(TransmitLED, LOW);
  return errcode;
}


//Maidenhead code from Ossi Väänänen https://ham.stackexchange.com/questions/221/how-can-one-convert-from-lat-long-to-grid-square
void calcLocator(double lat, double lon) {
  int o1, o2, o3;
  int a1, a2, a3;
  double remainder;
  // longitude
  remainder = lon + 180.0;
  o1 = (int)(remainder / 20.0);
  remainder = remainder - (double)o1 * 20.0;
  o2 = (int)(remainder / 2.0);
  remainder = remainder - 2.0 * (double)o2;
  o3 = (int)(12.0 * remainder);

  // latitude
  remainder = lat + 90.0;
  a1 = (int)(remainder / 10.0);
  remainder = remainder - (double)a1 * 10.0;
  a2 = (int)(remainder);
  remainder = remainder - (double)a2;
  a3 = (int)(24.0 * remainder);
  GadgetData.WSPRData.MaidenHead4[0] = (char)o1 + 'A';
  GadgetData.WSPRData.MaidenHead4[1] = (char)a1 + 'A';
  GadgetData.WSPRData.MaidenHead4[2] = (char)o2 + '0';
  GadgetData.WSPRData.MaidenHead4[3] = (char)a2 + '0';
  GadgetData.WSPRData.MaidenHead4[4] = 0;

  GadgetData.WSPRData.MaidenHead6[0] = (char)o1 + 'A';
  GadgetData.WSPRData.MaidenHead6[1] = (char)a1 + 'A';
  GadgetData.WSPRData.MaidenHead6[2] = (char)o2 + '0';
  GadgetData.WSPRData.MaidenHead6[3] = (char)a2 + '0';
  GadgetData.WSPRData.MaidenHead6[4] = (char)o3 + 'A';
  GadgetData.WSPRData.MaidenHead6[5] = (char)a3 + 'A';
  GadgetData.WSPRData.MaidenHead6[6] = 0;
}

boolean NewPosition()  //Returns true if the postion has changed since the last transmission
{
  boolean NewPos = false;
  for (int i = 0; i < GadgetData.WSPRData.LocationPrecision; i++)  //Check if the position has changed, test it using either four or six letter Maidenhead precision based on user setting
  {
    if (GadgetData.WSPRData.MaidenHead6[i] != LastMaidenHead6[i]) NewPos = true;
  }
  return NewPos;
}

void StorePosition()  //Saves the current position
{
  for (int i = 0; i < 7; i++) {
    LastMaidenHead6[i] = GadgetData.WSPRData.MaidenHead6[i];
  }
}

//Part of the code from the TinyGPS example but here used for the NeoGPS
//Delay loop that checks if the GPS serial port is sending data and in that case passes it of to the GPS object
static void smartdelay(unsigned long delay_ms) {
  boolean Blink;
  int BlinkCount = 0;

  Blink = (delay_ms > 10000);  // If longer than 10 seconds of delay then Blink StatusLED once in a while
  // This custom version of delay() ensures that the gps object
  // is being "fed".
  long TimeLeft;
  unsigned long EndTime = delay_ms + millis();

  do {
    while (gps.available(GPSSerial)) fix = gps.read();  //If GPS data available - process it
    TimeLeft = EndTime - millis();

    if ((TimeLeft > 4000)) {
      //Send API update
      Serial.print(F("{MPS} "));
      Serial.println(TimeLeft / 1000);
      delay(1000);
      if (Blink) {
        BlinkCount++;
        if (BlinkCount > 4)  //Blink every 5 seconds
        {
          LEDBlink(1);
          BlinkCount = 0;
        }
      }
    }
  } while ((TimeLeft > 0) && (!Serial.available()));  //Until time is up or there is serial data received from the computer, in that case end early
  if (delay_ms > 4000) Serial.println(F("{MPS} 0"));  //When pause is complete send Pause 0 to the GUI so it looks neater. But only if it was at least a four second delay
}




boolean DetectSi5351I2CAddress() {
  uint8_t I2CResult;
  boolean Result;
  Si5351I2CAddress = 96;  //Try with the normal adress of 96
  Wire.beginTransmission(Si5351I2CAddress);

  if (Wire.endTransmission() == 0) {
    //We found it
    //Serial.println("Detected at adress 96");
    Result = true;
  } else {
    //Serial.println("Not Detected at adress 96");
    Si5351I2CAddress = 98;  //Try the alternative address of 98
    Wire.beginTransmission(Si5351I2CAddress);
    if (Wire.endTransmission() == 0) {
      //Serial.println("Detected at adress 98");
      Result = true;
    } else {
      //Serial.println("Not Detected at adress 98 either, no Si5351!");
      Result = false;
      Si5351I2CAddress = 0;
    }
  }
  return Result;
}



uint8_t i2cSendRegister(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(Si5351I2CAddress);  // Start session with the Si5351PLL at I2C address
  Wire.write(reg);                           // Register number
  Wire.write(data);                          // Data to put into register
  Wire.endTransmission();
  return 0;
}

/*
uint8_t i2cReadRegister(uint8_t reg, uint8_t *data)
{
  uint8_t stts;

  stts = i2cStart();
  if (stts != I2C_START) return 1;

  stts = i2cByteSend((Si5351I2CAddress << 1));
  if (stts != I2C_SLA_W_ACK) return 2;

  stts = i2cByteSend(reg);
  if (stts != I2C_DATA_ACK) return 3;

  stts = i2cStart();
  if (stts != I2C_START_RPT) return 4;

  stts = i2cByteSend((Si5351I2CAddress << 1) + 1);
  if (stts != I2C_SLA_R_ACK) return 5;

  *data = i2cByteRead();

  i2cStop();

  return 0;
}

*/

/*
// Init TWI (I2C)
//
void i2cInit()
{
  TWBR = 92;
  TWSR = 0;
  TWDR = 0xFF;
  PRR = 0;
}
*/


//PLL routines from Hans Summer demo code https://www.qrp-labs.com/images/uarduino/uard_demo.ino
//
// Set up specified PLL with mult, num and denom
// mult is 15..90
// num is 0..1,048,575 (0xFFFFF)
// denom is 0..1,048,575 (0xFFFFF)
//
void setupPLL(uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom) {
  uint32_t P1;  // PLL config register P1
  uint32_t P2;  // PLL config register P2
  uint32_t P3;  // PLL config register P3

  P1 = (uint32_t)(128 * ((float)num / (float)denom));
  P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
  P2 = (uint32_t)(128 * ((float)num / (float)denom));
  P2 = (uint32_t)(128 * num - denom * P2);
  P3 = denom;

  i2cSendRegister(pll + 0, (P3 & 0x0000FF00) >> 8);
  i2cSendRegister(pll + 1, (P3 & 0x000000FF));
  i2cSendRegister(pll + 2, (P1 & 0x00030000) >> 16);
  i2cSendRegister(pll + 3, (P1 & 0x0000FF00) >> 8);
  i2cSendRegister(pll + 4, (P1 & 0x000000FF));
  i2cSendRegister(pll + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  i2cSendRegister(pll + 6, (P2 & 0x0000FF00) >> 8);
  i2cSendRegister(pll + 7, (P2 & 0x000000FF));
}


//PLL routines from Han Summer demo code https://www.qrp-labs.com/images/uarduino/uard_demo.ino
//
// Set up MultiSynth with integer Divider and R Divider
// R Divider is the bit value which is OR'ed onto the appropriate
// register, it is a #define in si5351a.h
//
void setupMultisynth(uint8_t synth, uint32_t Divider, uint8_t rDiv) {
  uint32_t P1;  // Synth config register P1
  uint32_t P2;  // Synth config register P2
  uint32_t P3;  // Synth config register P3

  P1 = 128 * Divider - 512;
  P2 = 0;  // P2 = 0, P3 = 1 forces an integer value for the Divider
  P3 = 1;

  i2cSendRegister(synth + 0, (P3 & 0x0000FF00) >> 8);
  i2cSendRegister(synth + 1, (P3 & 0x000000FF));
  i2cSendRegister(synth + 2, ((P1 & 0x00030000) >> 16) | rDiv);
  i2cSendRegister(synth + 3, (P1 & 0x0000FF00) >> 8);
  i2cSendRegister(synth + 4, (P1 & 0x000000FF));
  i2cSendRegister(synth + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  i2cSendRegister(synth + 6, (P2 & 0x0000FF00) >> 8);
  i2cSendRegister(synth + 7, (P2 & 0x000000FF));
}


// Switches off Si5351a output
void si5351aOutputOff(uint8_t clk) {
  i2cSendRegister(clk, 0x80);  // Refer to SiLabs AN619 to see
  //bit values - 0x80 turns off the output stage
  //digitalWrite(TransmitLED, LOW);
  SendAPIUpdate(UMesTXOff);
}




/*


// Set CLK0 output ON and to the specified frequency
// Frequency is in the range 10kHz to 150MHz and given in centiHertz (hundreds of Hertz)
// Example: si5351aSetFrequency(1000000200);
// will set output CLK0 to 10.000,002MHz
//
// This example sets up PLL A
// and MultiSynth 0
// and produces the output on CLK0
//
void si5351aSetFrequency(uint64_t frequency) //Frequency is in centiHz
{
  static uint64_t oldFreq;
  int32_t FreqChange;
  uint64_t pllFreq;
  //uint32_t xtalFreq = XTAL_FREQ;
  uint32_t l;
  float f;
  uint8_t mult;
  uint32_t num;
  uint32_t denom;
  uint32_t Divider;
  uint8_t rDiv;
  uint8_t OutputDivider;
  uint32_t ReferenceFrequency;

  ReferenceFrequency=FactoryData.RefFreq; //Use the built in TCXO stored frequency in the PLL calculations (as stored at last calibration)
   //On hardware revsion 36 and higher there can be an external frequency reference input, check if it is active and in that case use that frequency for the PLL calculations
  if (Product_Model == 1048) //If XP Plus then chack if the external reference is used
  { 
    if (digitalRead(ExtRefIndicator) == LOW)  
    {
     ReferenceFrequency=GadgetData.ExtRefFreq;//External Reference is present, use that frequency for PLL calculations  
    }
  }
  OutputDivider=1;
  //If Outoput frequency is lower than 1MHz then use the output divider to get as low as possible in frequence before hitting the limit on Si5351
  if (frequency < 100000000ULL)   
    {
      OutputDivider=128;  
      rDiv=SI_R_DIV_128;  
  } 

    Divider = 90000000000ULL / (frequency * OutputDivider);// Calculate the division ratio. 900MHz is the maximum VCO freq

    pllFreq = Divider * frequency * OutputDivider; // Calculate the pllFrequency:
    mult = pllFreq / (ReferenceFrequency * 100UL); // Determine the multiplier to
    //get to the required pllFrequency
    l = pllFreq % (ReferenceFrequency * 100UL); // It has three parts:
    f = l; // mult is an integer that must be in the range 15..90
    f *= 1048575; // num and denom are the fractional parts, the numerator and denominator
    f /= ReferenceFrequency; // each is 20 bits (range 0..1048575)
    num = f; // the actual multiplier is mult + num / denom
    denom = 1048575; // For simplicity we set the denominator to the maximum 1048575
    num = num / 100;
  

  // Set up PLL A with the calculated  multiplication ratio
  setupPLL(SI_SYNTH_PLL_A, mult, num, denom);

  // Set up MultiSynth Divider 0, with the calculated Divider.
  // The final R division stage can divide by a power of two, from 1..128.
  // reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)
  // If you want to output frequencies below 1MHz, you have to use the
  // final R division stage
  setupMultisynth(SI_SYNTH_MS_0, Divider, rDiv);

  // Reset the PLL. This causes a glitch in the output. For small changes to
  // the parameters, you don't need to reset the PLL, and there is no glitch
  FreqChange = frequency - oldFreq;

  if ( abs(FreqChange) > 100000) //If changed more than 1kHz then reset PLL (completely arbitrary choosen)
  {
    i2cSendRegister(SI_PLL_RESET, 0xA0);
  }

  // Finally switch on the CLK0 output (0x4F)
  // and set the MultiSynth0 input to be PLL A
  i2cSendRegister(SI_CLK0_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
  oldFreq = frequency;
  //digitalWrite(TransmitLED, HIGH);
  Serial.print (F("{TFQ} "));
  Serial.println (uint64ToStr(frequency, false));
  SendAPIUpdate(UMesTXOn);
}


*/




// Set CLK0 output ON and to the specified frequency
// Frequency is in the range 10kHz to 150MHz and given in centiHertz (hundreds of Hertz)
// Example: si5351aSetFrequency(1000000200);
// will set output CLK0 to 10.000,002MHz
//
// This example sets up PLL A
// and MultiSynth 0
// and produces the output on CLK0
//
void si5351aSetFrequency(uint64_t frequency)  //Frequency is in centiHz
{
  static uint64_t oldFreq;
  int32_t FreqChange;
  uint64_t pllFreq;
  //uint32_t xtalFreq = XTAL_FREQ;
  uint32_t l;
  float f;
  uint8_t mult;
  uint32_t num;
  uint32_t denom;
  uint32_t Divider;
  uint8_t rDiv;
  uint8_t OutputDivider;
  uint32_t ReferenceFrequency;

  ReferenceFrequency = FactoryData.RefFreq;  //Use the built in TCXO stored frequency in the PLL calculations (as stored at last calibration)
                                             //On hardware revsion 36 and higher there can be an external frequency reference input, check if it is active and in that case use that frequency for the PLL calculations
  if (Product_Model == 1048)                 //If XP Plus then chack if the external reference is used
  {
    if (digitalRead(ExtRefIndicator) == LOW) {
      ReferenceFrequency = GadgetData.ExtRefFreq;  //External Reference is present, use that frequency for PLL calculations
    }
  }
  OutputDivider = 1;
  rDiv = SI_R_DIV_1;
  /*
  //Calculate what output divider to use, use as high as poosible to get fine-grained frequency steps
  if (frequency < 6400000000ULL) 
    {
      OutputDivider=2;  
      rDiv=SI_R_DIV_2;  
  }
  if (frequency < 3200000000ULL)   
    {
      OutputDivider=4;  
      rDiv=SI_R_DIV_4;  
  } 
   if (frequency < 1600000000ULL) 
    {
      OutputDivider=8;  
      rDiv=SI_R_DIV_8;  
  }
  if (frequency < 800000000ULL)   
    {
      OutputDivider=16;  
      rDiv=SI_R_DIV_16;  
  } 
    if (frequency < 400000000ULL)   
    {
      OutputDivider=32;  
      rDiv=SI_R_DIV_32;  
  } 
   if (frequency < 200000000ULL) 
    {
      OutputDivider=64;  
      rDiv=SI_R_DIV_64;  
  }
  */
  if (frequency < 100000000ULL) {
    OutputDivider = 128;
    rDiv = SI_R_DIV_128;
  }

  Divider = 90000000000ULL / (frequency * OutputDivider);  // Calculate the division ratio. 900MHz is the maximum VCO freq

  pllFreq = Divider * frequency * OutputDivider;  // Calculate the pllFrequency:
  mult = pllFreq / (ReferenceFrequency * 100UL);  // Determine the multiplier to
  //get to the required pllFrequency
  l = pllFreq % (ReferenceFrequency * 100UL);  // It has three parts:
  f = l;                                       // mult is an integer that must be in the range 15..90
  f *= 1048575;                                // num and denom are the fractional parts, the numerator and denominator
  f /= ReferenceFrequency;                     // each is 20 bits (range 0..1048575)
  num = f;                                     // the actual multiplier is mult + num / denom
  denom = 1048575;                             // For simplicity we set the denominator to the maximum 1048575
  num = num / 100;


  // Set up PLL A with the calculated  multiplication ratio
  setupPLL(SI_SYNTH_PLL_A, mult, num, denom);

  // Set up MultiSynth Divider 0, with the calculated Divider.
  // The final R division stage can divide by a power of two, from 1..128.
  // reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)
  // If you want to output frequencies below 1MHz, you have to use the
  // final R division stage
  setupMultisynth(SI_SYNTH_MS_0, Divider, rDiv);

  // Reset the PLL. This causes a glitch in the output. For small changes to
  // the parameters, you don't need to reset the PLL, and there is no glitch
  FreqChange = frequency - oldFreq;

  if (abs(FreqChange) > 100000)  //If changed more than 1kHz then reset PLL (completely arbitrary choosen)
  {
    i2cSendRegister(SI_PLL_RESET, 0xA0);
  }

  // Finally switch on the CLK0 output (0x4F)
  // and set the MultiSynth0 input to be PLL A
  i2cSendRegister(SI_CLK0_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
  oldFreq = frequency;
  //digitalWrite(TransmitLED, HIGH);
  Serial.print(F("{TFQ} "));
  Serial.println(uint64ToStr(frequency, false));
  SendAPIUpdate(UMesTXOn);
}



//Create a random seed by doing CRC32 on 100 analog values from port A0
//CRC calculation from Christopher Andrews : https://www.arduino.cc/en/Tutorial/EEPROMCrc
unsigned long RandomSeed(void) {

  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  uint8_t ByteVal;
  unsigned long crc = ~0L;

  for (int index = 0; index < 100; ++index) {
    ByteVal = analogRead(A0);
    crc = crc_table[(crc ^ ByteVal) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (ByteVal >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

//Returns true if the user has not enabled any bands for TX
boolean NoBandEnabled(void) {
  boolean NoOne = true;
  for (int FreqLoop = 0; FreqLoop < 13; FreqLoop++) {
    if (GadgetData.TXOnBand[FreqLoop]) NoOne = false;
  }
  return NoOne;
}

//Determine what band to transmit on, cycles upward in the TX enabled bands, e.g if band 2,5,6 and 11 is enbled for TX then the cycle will be 2-5-6-11-2-5-6-11-...
void NextFreq(void) {
  if (NoBandEnabled()) {
    freq = 0;
  } else {
    do {
      CurrentBand++;
      if (CurrentBand > 12) CurrentBand = 0;
    } while (!GadgetData.TXOnBand[CurrentBand]);

    switch (CurrentBand) {
      case 0:
        freq = WSPR_FREQ2190m;
        break;
      case 1:
        freq = WSPR_FREQ630m;
        break;
      case 2:
        freq = WSPR_FREQ160m;
        break;
      case 3:
        freq = WSPR_FREQ80m;
        break;
      case 4:
        freq = WSPR_FREQ40m;
        break;
      case 5:
        freq = WSPR_FREQ30m;
        break;
      case 6:
        freq = WSPR_FREQ20m;
        break;
      case 7:
        freq = WSPR_FREQ17m;
        break;
      case 8:
        freq = WSPR_FREQ15m;
        break;
      case 9:
        freq = WSPR_FREQ12m;
        break;
      case 10:
        freq = WSPR_FREQ10m;
        break;
      case 11:
        freq = WSPR_FREQ6m;
        break;
      case 12:
        freq = WSPR_FREQ4m;
    }
    Serial.print("{TBN} ");  //Send API update to inform what band we are using at the moment
    if (CurrentBand < 10) SerialPrintZero();
    Serial.println(CurrentBand);
    //We have found what band to use, now pick the right low pass filter for this band
    PickLP(CurrentBand);
  }
}

//Function returns True if the band we just transmitted on was the highest band the user want to transmit on.
boolean LastFreq(void) {
  boolean Last = true;
  int TestBand;

  TestBand = CurrentBand;
  if (TestBand == 12) {
    Last = true;
  } else {
    do {
      TestBand++;
      if (GadgetData.TXOnBand[TestBand]) Last = false;
    } while (TestBand < 12);
  }
  return Last;
}


//CRC calculation from Christopher Andrews : https://www.arduino.cc/en/Tutorial/EEPROMCrc
//Calculate CRC on either Factory data or Userspace data
unsigned long GetEEPROM_CRC(boolean EEPROMSpace) {

  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;
  int Start;
  int Length;

  if (EEPROMSpace == FactorySpace) {
    Start = 400;
    Length = sizeof(FactoryData);
  } else {
    Start = 0;
    Length = sizeof(GadgetData);
  }
  for (int index = Start; index < (Start + Length); ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}


//Load FactoryData or UserSpace Data from ATMega EEPROM
bool LoadFromEPROM(boolean EEPROMSpace) {
  int Start;
  int Length;
  unsigned long CRCFromEEPROM, CalculatedCRC;

  if (EEPROMSpace == FactorySpace)  //Factory data
  {
    Start = 400;
    Length = sizeof(FactoryData);
    EEPROM.get(Start, FactoryData);               //Load all the data from EEPROM
    CalculatedCRC = GetEEPROM_CRC(FactorySpace);  //Calculate the CRC of the data
  } else                                          //User data
  {
    Start = 0;
    Length = sizeof(GadgetData);
    EEPROM.get(Start, GadgetData);             //Load all the data from EEPROM
    CalculatedCRC = GetEEPROM_CRC(UserSpace);  //Calculate the CRC of the data
  }
  EEPROM.get(Start + Length, CRCFromEEPROM);  //Load the saved CRC at the end of the data
  return (CRCFromEEPROM == CalculatedCRC);    //If  Stored and Calculated CRC are the same return true
}


//Save FactoryData or UserSpace Data to Arduino EEPROM
void SaveToEEPROM(boolean EEPROMSpace) {
  int Start;
  int Length;
  unsigned long CRCFromEEPROM;
  if (EEPROMSpace == FactorySpace) {
    Start = 400;
    Length = sizeof(FactoryData);
    EEPROM.put(Start, FactoryData);  //Save all the Factory data to EEPROM at adress400
  } else                             //UserSpace
  {
    Start = 0;
    Length = sizeof(GadgetData);
    EEPROM.put(Start, GadgetData);  //Save all the User data to EEPROM at adress0
  }
  CRCFromEEPROM = GetEEPROM_CRC(EEPROMSpace);  //Calculate CRC on the saved data
  EEPROM.put(Start + Length, CRCFromEEPROM);   //Save the CRC after the data
  EEPROM.commit();
}


void SendAPIUpdate(uint8_t UpdateType) {
  switch (UpdateType) {
    case UMesCurrentMode:

      Serial.print(F("{CCM} "));
      switch (CurrentMode) {
        case Idle:
          Serial.println(("N"));
          Serial.println(F("{TON} F"));  //Also Send TX Off info
          break;
        case WSPRBeacon:
          Serial.println(("W"));
          Serial.println(F("{TON} F"));  //Also send TX Off info, WSPR routine will change this if it currently transmitting
          break;
        case SignalGen:
          Serial.println(("S"));
          Serial.println(F("{TON} T"));  //Also Send TX ON info
          break;
      }
      break;

    case UMesLocator:
      Serial.print(F("{GL4} "));
      Serial.println(GadgetData.WSPRData.MaidenHead4);
      Serial.print(F("{GL6} "));
      Serial.println(GadgetData.WSPRData.MaidenHead6);
      break;

    case UMesTime:
      // ----------------------- RobZ ----------------------------------
      // GPSH = fix.dateTime.hours;
      // GPSM = fix.dateTime.minutes;
      // GPSS = fix.dateTime.seconds;
      GPSH = 0;  //Reset time
      GPSM = 0;
      GPSS = 0;
      if (fix.valid.time) {  //Set time from GPS data
        GPSH = fix.dateTime.hours;
        GPSM = fix.dateTime.minutes;
        GPSS = fix.dateTime.seconds;
      } else if (timeClient.isTimeSet()) {  // If no GPS, set time from NTP data
        GPSH = timeClient.getHours();
        GPSM = timeClient.getMinutes();
        GPSS = timeClient.getSeconds();
      }
      // ----------------------- RobZ--------------------------------------
      Serial.print(F("{GTM} "));
      if (GPSH < 10) SerialPrintZero();
      Serial.print(GPSH);
      Serial.print(":");
      if (GPSM < 10) SerialPrintZero();
      Serial.print(GPSM);
      Serial.print(":");
      if (GPSS < 10) SerialPrintZero();
      Serial.println(GPSS);
      break;

    case UMesGPSLock:
      Serial.println(F("{GLC} T"));
      break;

    case UMesNoGPSLock:
      Serial.println(F("{GLC} F"));
      break;

    case UMesFreq:
      Serial.print(F("{TFQ} "));
      Serial.println(uint64ToStr(freq, false));
      break;

    case UMesTXOn:
      Serial.println(F("{TON} T"));
      break;

    case UMesTXOff:
      Serial.println(F("{TON} F"));
      break;

    case UMesWSPRBandCycleComplete:
      Serial.println(F("{TCC}"));
      break;

    case UMesVCC:
      // Serial.print (F("{MVC} "));
      // Serial.println (GetVCC());
      break;

    case UMesLPF:
      Serial.print(F("{LPI} "));
      if (CurrentLP == LP_A) Serial.println("A");
      if (CurrentLP == LP_B) Serial.println("B");
      if (CurrentLP == LP_C) Serial.println("C");
      if (CurrentLP == LP_D) Serial.println("D");
  }
}


//Brief flash on the Status LED 'Blinks'" number of time
void LEDBlink(int Blinks) {
  for (int i = 0; i < Blinks; i++) {
    digitalWrite(StatusLED, LOW);
    smartdelay(50);
    digitalWrite(StatusLED, HIGH);
    smartdelay(50);
  }
}

//Pulls the correct relays to choose LP filter A,B,C or D
void DriveLPFilters() {
  switch (Product_Model) {
    case 1011:
      switch (CurrentLP) {
        case LP_A:
          //all relays are at rest
          digitalWrite(Relay1, LOW);
          digitalWrite(Relay2, LOW);
          break;

        case LP_B:
          digitalWrite(Relay1, HIGH);
          digitalWrite(Relay2, LOW);
          break;

        case LP_C:
          digitalWrite(Relay1, LOW);
          digitalWrite(Relay2, HIGH);
          break;

        case LP_D:
          digitalWrite(Relay1, HIGH);
          digitalWrite(Relay2, HIGH);
          break;
      }  //switch (CurrentLP)
      break;

    case 1012:                           //Desktop
      if (FactoryData.HW_Revision > 35)  //On hardware revsion 36 and higher the Relays are driven differently and from different pins on the MCU.
      {
        switch (CurrentLP) {
          case LP_A:
            //all relays are at rest
            digitalWrite(Relay1, LOW);
            digitalWrite(Relay2, LOW);
            break;

          case LP_B:
            digitalWrite(Relay1, HIGH);
            digitalWrite(Relay2, LOW);
            break;

          case LP_C:
            digitalWrite(Relay1, LOW);
            digitalWrite(Relay2, HIGH);
            break;

          case LP_D:
            digitalWrite(Relay1, HIGH);
            digitalWrite(Relay2, HIGH);
            break;
        }  //switch (CurrentLP)
      }    //if
      else {
        //Must be desktop model older then V2R36
        switch (CurrentLP) {
          case LP_A:
            //all relays are at rest
            digitalWrite(Relay1, LOW);
            digitalWrite(Relay2, LOW);
            digitalWrite(Relay3, LOW);
            break;

          case LP_B:
            digitalWrite(Relay1, HIGH);
            digitalWrite(Relay2, LOW);
            digitalWrite(Relay3, LOW);
            break;

          case LP_C:
            digitalWrite(Relay1, LOW);
            digitalWrite(Relay2, LOW);
            digitalWrite(Relay3, HIGH);
            break;

          case LP_D:
            digitalWrite(Relay1, LOW);
            digitalWrite(Relay2, HIGH);
            digitalWrite(Relay3, HIGH);
            break;
        }  //switch (CurrentLP)
      }    //if
      break;

    case 1017:
      //The WSPR-TX Mini dont have any relays
      break;

    case 1020:  //The LP1 with Mezzanine board
      switch (CurrentLP) {
        case LP_A:
          //all relays are at rest
          digitalWrite(Relay2, LOW);
          digitalWrite(Relay3, LOW);
          break;

        case LP_B:
          digitalWrite(Relay2, HIGH);
          digitalWrite(Relay3, LOW);
          break;

        case LP_C:
          digitalWrite(Relay2, LOW);
          digitalWrite(Relay3, HIGH);
          break;

        case LP_D:
          digitalWrite(Relay2, HIGH);
          digitalWrite(Relay3, HIGH);
          break;
      }  // switch (CurrentLP)
      break;

    case 1028:
      //The WSPR-TX Pico dont have any relays
      break;

    case 1048:  //WSPR-TX XP Plus
      switch (CurrentLP) {
        case LP_A:
          //all relays are at rest
          digitalWrite(Relay1, LOW);
          digitalWrite(Relay2, LOW);
          break;

        case LP_B:
          digitalWrite(Relay1, HIGH);
          digitalWrite(Relay2, LOW);
          break;

        case LP_C:
          digitalWrite(Relay1, LOW);
          digitalWrite(Relay2, HIGH);
          break;

        case LP_D:
          digitalWrite(Relay1, HIGH);
          digitalWrite(Relay2, HIGH);
          break;
      }  //switch (CurrentLP)
      break;


      SendAPIUpdate(UMesLPF);
  }
}


// Convert a frequency to a Ham band. Frequency is stored in global variable freq
uint8_t FreqToBand() {
  uint8_t BandReturn = 15;

  if (freq < (WSPR_FREQ70cm * 1.2)) BandReturn = 14;
  if (freq < (WSPR_FREQ2m * 1.2)) BandReturn = 13;
  if (freq < (WSPR_FREQ4m * 1.2)) BandReturn = 12;
  if (freq < (WSPR_FREQ6m * 1.2)) BandReturn = 11;
  if (freq < (WSPR_FREQ10m * 1.2)) BandReturn = 10;
  if (freq < (WSPR_FREQ12m * 1.2)) BandReturn = 9;
  if (freq < (WSPR_FREQ15m * 1.2)) BandReturn = 8;
  if (freq < (WSPR_FREQ17m * 1.1)) BandReturn = 7;
  if (freq < (WSPR_FREQ20m * 1.2)) BandReturn = 6;
  if (freq < (WSPR_FREQ30m * 1.2)) BandReturn = 5;
  if (freq < (WSPR_FREQ40m * 1.2)) BandReturn = 4;
  if (freq < (WSPR_FREQ80m * 1.2)) BandReturn = 3;
  if (freq < (WSPR_FREQ160m * 1.2)) BandReturn = 2;
  if (freq < (WSPR_FREQ630m * 1.2)) BandReturn = 1;
  if (freq < (WSPR_FREQ2190m * 1.2)) BandReturn = 0;

  return BandReturn;
}

//Out of the four possible LP filters fitted - find the one that is best for Transmission on TXBand
void PickLP(uint8_t TXBand) {
  boolean ExactMatch = false;
  uint8_t BandLoop;

  //Check if some of the four low pass filters is an exact match for the TXBand
  if (FactoryData.LP_A_BandNum == TXBand) {
    ExactMatch = true;
    CurrentLP = LP_A;
  }
  if (FactoryData.LP_B_BandNum == TXBand) {
    ExactMatch = true;
    CurrentLP = LP_B;
  }
  if (FactoryData.LP_C_BandNum == TXBand) {
    ExactMatch = true;
    CurrentLP = LP_C;
  }
  if (FactoryData.LP_D_BandNum == TXBand) {
    ExactMatch = true;
    CurrentLP = LP_D;
  }

  //If we did not find a perfect match then use a low pass filter that is higher in frequency.
  if (!ExactMatch) {
    for (BandLoop = TXBand; BandLoop < 99; BandLoop++)  //Test all higher bands to find a a possible LP filter in one of the four LP banks
    {
      if (FactoryData.LP_A_BandNum == BandLoop)  //The LP filter in Bank A is a match for this band
      {
        CurrentLP = LP_A;
        break;
      }
      if (FactoryData.LP_B_BandNum == BandLoop)  //The LP filter in Bank B is a match for this band
      {
        CurrentLP = LP_B;
        break;
      }
      if (FactoryData.LP_C_BandNum == BandLoop)  //The LP filter in Bank C is a match for this band
      {
        CurrentLP = LP_C;
        break;
      }
      if (FactoryData.LP_D_BandNum == BandLoop)  //The LP filter in Bank D is a match for this band
      {
        CurrentLP = LP_D;
        break;
      }
    }
    //If there is no LP that is higher than TXBand then use the highest one, (not ideal as output will be attenuated but best we can do)
    if (BandLoop == 99) {
      TXBand = BandNumOfHigestLP();
      if (FactoryData.LP_A_BandNum == TXBand) {
        CurrentLP = LP_A;
      }
      if (FactoryData.LP_B_BandNum == TXBand) {
        CurrentLP = LP_B;
      }
      if (FactoryData.LP_C_BandNum == TXBand) {
        CurrentLP = LP_C;
      }
      if (FactoryData.LP_D_BandNum == TXBand) {
        CurrentLP = LP_D;
      }
    }
  }
  DriveLPFilters();
}

//Returns a band that is the highest band that has a LP filter fitted onboard.
//Low pass filter numbering corresponds to Bands or two special cases
//The special cases are: 98=just a link between input and output, 99=Nothing fitted (open circut) the firmware will never use this
//These numbers are set by the factory Configuration program and stored in EEPROM
uint8_t BandNumOfHigestLP() {
  uint8_t BandLoop, Result;
  Result = FactoryData.LP_A_BandNum;  //Use this filter if nothing else is a match.
  //Find the highest band that has a Low Pass filter fitted in one of the four LP banks
  for (BandLoop = 98; BandLoop > 0; BandLoop--) {
    if (FactoryData.LP_A_BandNum == BandLoop)  //The LP filter in Bank A is a match for this band
    {
      Result = FactoryData.LP_A_BandNum;
      break;
    }
    if (FactoryData.LP_B_BandNum == BandLoop)  //The LP filter in Bank B is a match for this band
    {
      Result = FactoryData.LP_B_BandNum;
      break;
    }
    if (FactoryData.LP_C_BandNum == BandLoop)  //The LP filter in Bank C is a match for this band
    {
      Result = FactoryData.LP_C_BandNum;
      break;
    }
    if (FactoryData.LP_D_BandNum == BandLoop)  //The LP filter in Bank D is a match for this band
    {
      Result = FactoryData.LP_D_BandNum;
      break;
    }
  }
  return Result;
}


void PowerSaveOFF() {
  GPSWakeUp();
}

void PowerSaveON() {
  GPSGoToSleep();
}

void GPSGoToSleep() {
  switch (Product_Model) {
    case 1017:  //Mini
      //If its the WSPR-TX Mini, send the Sleep string to it
      GPSSerial.println(F("$PMTK161,0*28"));
      //GPSSleep = true;
      break;

    case 1028:  //Pico
      //If it is the WSPR-TX Pico it has a hardware line for sleep/wake
      // pinMode(GPSPower, OUTPUT);
      // digitalWrite(GPSPower, LOW);
      break;
  }
}

void GPSWakeUp() {
  switch (Product_Model) {
    case 1017:  //Mini
      //Send anything on the GPS serial line to wake it up
      GPSSerial.println(" ");
      //GPSSleep = false;
      delay(100);  //Give the GPS some time to wake up and send its serial data back to us
      break;

    case 1028:  //Pico
      //If it is the WSPR-TX Pico it has a hardware line for sleep/wake
      //pinMode(GPSPower, OUTPUT);
      //digitalWrite(GPSPower, HIGH);
      delay(200);
      //pinMode(GPSPower, INPUT);
      delay(200);
      //Send GPS reset string
      //GPSSerial.println(F("$PCAS10,3*1F"));
      //Airborne Mode
      //GPSSerial.println(F("$PCAS11,5*18"));
      break;
  }
}

void GPSReset() {
  GPSWakeUp();
  //Send GPS reset string
  GPSSerial.println(F("$PCAS10,3*1F"));
  GPSSetConstellation(GadgetData.WSPRData.GPSConstellations);
}

void GPSSetConstellation(E_GPSConstellations Constellations) {
  switch (Constellations) {
    case Cons_GPS:
      //Send GPS string
      GPSSerial.println(F("$PCAS04,1*18"));
      break;

    case Cons_BeiDou:
      //Send GPS string
      GPSSerial.println(F("$PCAS04,2*1B"));
      break;

    case Cons_GPSAndBeiDou:
      //Send GPS string
      GPSSerial.println(F("$PCAS04,3*1A"));
      break;
  }
}


void SerialPrintZero() {
  Serial.print("0");
}


//Sends the Sattelite data like Elevation, Azimuth SNR and ID using the Serial API {GSI} format
void SendSatData() {
  uint8_t SNR;
  for (uint8_t i = 0; i < gps.sat_count; i++) {
    Serial.print(F("{GSI} "));
    if (gps.satellites[i].id < 10) SerialPrintZero();
    Serial.print(gps.satellites[i].id);
    Serial.print(" ");
    if (gps.satellites[i].azimuth < 100) SerialPrintZero();
    if (gps.satellites[i].azimuth < 10) SerialPrintZero();
    Serial.print(gps.satellites[i].azimuth);
    Serial.print(" ");
    if (gps.satellites[i].elevation < 10) SerialPrintZero();
    Serial.print(gps.satellites[i].elevation);
    Serial.print((" "));
    SNR = 0;
    if (gps.satellites[i].tracked) {
      SNR = gps.satellites[i].snr;
    } else {
      SNR = 0;
    }
    if (SNR < 10) SerialPrintZero();
    Serial.println(SNR);
  }
  Serial.println();
}  // displaySatellitesInView


//Original WSPR code by NT7S - Jason Milldrum https://github.com/etherkit/JTEncode and Bo Hansen - OZ2M RFZero https://rfzero.net
//Modifed for Type2 and Type3 messages by SM7PNV Harry Zachrisson https://github.com/HarrydeBug

/*
   wspr_encode(const char * call, const char * loc, const uint8_t dbm, uint8_t * symbols)

   Takes an arbitrary message of up to 13 allowable characters and returns

   call - Callsign (6 characters maximum).
   loc - Maidenhead grid locator (4 charcters maximum).
   dbm - Output power in dBm.
   symbols - Array of channel symbols to transmit retunred by the method.
   Ensure that you pass a uint8_t array of size WSPR_SYMBOL_COUNT to the method.

*/

//Converts a letter (A-Z) or digit (0-9)to a special format used in the encoding of WSPR messages
uint8_t EncodeChar(char Character) {
  uint8_t ConvertedNumber;
  if (Character == ' ') {
    ConvertedNumber = 36;
  } else {
    if (isdigit(Character)) {
      ConvertedNumber = Character - '0';
    } else {
      ConvertedNumber = 10 + (Character - 'A');
    }
  }
  return ConvertedNumber;
}

void wspr_encode(const char *call, const char *loc, const uint8_t dbm, uint8_t *symbols, uint8_t WSPRMessageType) {
  char call_[7];
  char loc_[5];
  uint8_t dbm_ = dbm;
  strcpy(call_, call);
  strcpy(loc_, loc);
  uint32_t n, m;

  // Ensure that the message text conforms to standards
  // --------------------------------------------------
  wspr_message_prep(call_, loc_, dbm_);

  // Bit packing
  // -----------
  uint8_t c[11];

  switch (WSPRMessageType) {
    case 1:  //Normal coding with callsign, 4letter Maidenhead postion and power
      n = wspr_code(callsign[0]);
      n = n * 36 + wspr_code(callsign[1]);
      n = n * 10 + wspr_code(callsign[2]);
      n = n * 27 + (wspr_code(callsign[3]) - 10);
      n = n * 27 + (wspr_code(callsign[4]) - 10);
      n = n * 27 + (wspr_code(callsign[5]) - 10);

      m = ((179 - 10 * (locator[0] - 'A') - (locator[2] - '0')) * 180) + (10 * (locator[1] - 'A')) + (locator[3] - '0');
      m = (m * 128) + power + 64;
      break;

    case 2:  //Call sign and Prefix or suffix for it and power, no Maidenhead position
      n = wspr_code(callsign[0]);
      n = n * 36 + wspr_code(callsign[1]);
      n = n * 10 + wspr_code(callsign[2]);
      n = n * 27 + (wspr_code(callsign[3]) - 10);
      n = n * 27 + (wspr_code(callsign[4]) - 10);
      n = n * 27 + (wspr_code(callsign[5]) - 10);

      if (GadgetData.WSPRData.SuPreFixOption == Sufix) {
        // Single number or letter suffix from 0 to 35, 0-9= 0-9. 10-35=A-Z.
        // Or double number suffix from 36 to 125, 36-125=10-99
        m = (27232 + GadgetData.WSPRData.Sufix);
        m = (m * 128) + power + 2 + 64;
      } else {
        //Three character prefix. Numbers, letters or space
        //0 to 9=0-9, A to Z=10-35, space=36
        m = EncodeChar(GadgetData.WSPRData.Prefix[0]);           //Left Character
        m = 37 * m + EncodeChar(GadgetData.WSPRData.Prefix[1]);  //Mid character
        m = 37 * m + EncodeChar(GadgetData.WSPRData.Prefix[2]);  //Right character
        //m = (m * 128) + power +1+ 64;

        if (m > 32767) {
          m = m - 32768;
          m = (m * 128) + power + 66;
        } else {
          m = (m * 128) + power + 65;
        }
      }
      break;

    case 3:  //Hashed Callsign, six letter maidenhead position and power
      //encode the six letter Maidenhear postion in to n that is usually used for callsign coding, reshuffle the character order to conform to the callsign rules
      n = wspr_code(GadgetData.WSPRData.MaidenHead6[1]);
      n = n * 36 + wspr_code(GadgetData.WSPRData.MaidenHead6[2]);
      n = n * 10 + wspr_code(GadgetData.WSPRData.MaidenHead6[3]);
      n = n * 27 + (wspr_code(GadgetData.WSPRData.MaidenHead6[4]) - 10);
      n = n * 27 + (wspr_code(GadgetData.WSPRData.MaidenHead6[5]) - 10);
      n = n * 27 + (wspr_code(GadgetData.WSPRData.MaidenHead6[0]) - 10);
      m = 128 * WSPRCallHash(call) - power - 1 + 64;
      break;

  }  //switch

  // Callsign is 28 bits, locator/power is 22 bits.
  // A little less work to start with the least-significant bits
  c[3] = (uint8_t)((n & 0x0f) << 4);
  n = n >> 4;
  c[2] = (uint8_t)(n & 0xff);
  n = n >> 8;
  c[1] = (uint8_t)(n & 0xff);
  n = n >> 8;
  c[0] = (uint8_t)(n & 0xff);

  c[6] = (uint8_t)((m & 0x03) << 6);
  m = m >> 2;
  c[5] = (uint8_t)(m & 0xff);
  m = m >> 8;
  c[4] = (uint8_t)(m & 0xff);
  m = m >> 8;
  c[3] |= (uint8_t)(m & 0x0f);
  c[7] = 0;
  c[8] = 0;
  c[9] = 0;
  c[10] = 0;

  // Convolutional Encoding
  // ---------------------
  uint8_t s[WSPR_SYMBOL_COUNT];
  convolve(c, s, 11, WSPR_SYMBOL_COUNT);

  // Interleaving
  // ------------
  wspr_interleave(s);

  // Merge with sync vector
  // ----------------------
  wspr_merge_sync_vector(s, symbols);
}

void wspr_message_prep(char *call, char *loc, uint8_t dbm) {
  // PrintCallSign ('2');
  // If only the 2nd character is a digit, then pad with a space.
  // If this happens, then the callsign will be truncated if it is
  // longer than 6 characters.
  if (isdigit(call[1]) && isupper(call[2])) {
    call[5] = call[4];
    call[4] = call[3];
    call[3] = call[2];
    call[2] = call[1];
    call[1] = call[0];
    call[0] = ' ';
  }

  // Ensure that the only allowed characters are digits and uppercase letters
  uint8_t i;
  for (i = 0; i < 6; i++) {
    call[i] = toupper(call[i]);
    if (!(isdigit(call[i]) || isupper(call[i]))) {
      call[i] = ' ';
      if (i == 4) {
        call[5] = ' ';  //If char 4 is a space then also set the last character to a space
      }
    }
  }

  memcpy(callsign, call, 6);

  // Grid locator validation
  for (i = 0; i < 4; i++) {
    loc[i] = toupper(loc[i]);
    if (!(isdigit(loc[i]) || (loc[i] >= 'A' && loc[i] <= 'R'))) {
      memcpy(loc, "AA00", 5);  //loc = "AA00";
    }
  }
  memcpy(locator, loc, 4);
  power = ValiddBmValue(dbm);
}

// Power level validation
uint8_t ValiddBmValue(uint8_t dBmIn) {
  uint8_t i;
  uint8_t validateddBmValue;
  const uint8_t valid_dbm[19] = { 0, 3, 7, 10, 13, 17, 20, 23, 27, 30, 33, 37, 40,
                                  43, 47, 50, 53, 57, 60 };
  validateddBmValue = dBmIn;
  if (validateddBmValue > 60) {
    validateddBmValue = 60;
  }

  for (i = 0; i < 19; i++) {
    if (dBmIn >= valid_dbm[i]) {
      validateddBmValue = valid_dbm[i];
    }
  }
  return validateddBmValue;
}

void convolve(uint8_t *c, uint8_t *s, uint8_t message_size, uint8_t bit_size) {
  uint32_t reg_0 = 0;
  uint32_t reg_1 = 0;
  uint32_t reg_temp = 0;
  uint8_t input_bit, parity_bit;
  uint8_t bit_count = 0;
  uint8_t i, j, k;

  for (i = 0; i < message_size; i++) {
    for (j = 0; j < 8; j++) {
      // Set input bit according the MSB of current element
      input_bit = (((c[i] << j) & 0x80) == 0x80) ? 1 : 0;

      // Shift both registers and put in the new input bit
      reg_0 = reg_0 << 1;
      reg_1 = reg_1 << 1;
      reg_0 |= (uint32_t)input_bit;
      reg_1 |= (uint32_t)input_bit;

      // AND Register 0 with feedback taps, calculate parity
      reg_temp = reg_0 & 0xf2d05351;
      parity_bit = 0;
      for (k = 0; k < 32; k++) {
        parity_bit = parity_bit ^ (reg_temp & 0x01);
        reg_temp = reg_temp >> 1;
      }
      s[bit_count] = parity_bit;
      bit_count++;

      // AND Register 1 with feedback taps, calculate parity
      reg_temp = reg_1 & 0xe4613c47;
      parity_bit = 0;
      for (k = 0; k < 32; k++) {
        parity_bit = parity_bit ^ (reg_temp & 0x01);
        reg_temp = reg_temp >> 1;
      }
      s[bit_count] = parity_bit;
      bit_count++;
      if (bit_count >= bit_size) {
        break;
      }
    }
  }
}

void wspr_interleave(uint8_t *s) {
  uint8_t d[WSPR_SYMBOL_COUNT];
  uint8_t rev, index_temp, i, j, k;

  i = 0;

  for (j = 0; j < 255; j++) {
    // Bit reverse the index
    index_temp = j;
    rev = 0;

    for (k = 0; k < 8; k++) {
      if (index_temp & 0x01) {
        rev = rev | (1 << (7 - k));
      }
      index_temp = index_temp >> 1;
    }

    if (rev < WSPR_SYMBOL_COUNT) {
      d[rev] = s[i];
      i++;
    }

    if (i >= WSPR_SYMBOL_COUNT) {
      break;
    }
  }

  memcpy(s, d, WSPR_SYMBOL_COUNT);
}

void wspr_merge_sync_vector(uint8_t *g, uint8_t *symbols) {
  uint8_t i;
  const uint8_t sync_vector[WSPR_SYMBOL_COUNT] = { 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0,
                                                   1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0,
                                                   0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1,
                                                   0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0,
                                                   1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
                                                   0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1,
                                                   1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                                                   1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 };

  for (i = 0; i < WSPR_SYMBOL_COUNT; i++) {
    symbols[i] = sync_vector[i] + (2 * g[i]);
  }
}

uint8_t wspr_code(char c) {
  // Validate the input then return the proper integer code.
  // Return 255 as an error code if the char is not allowed.

  if (isdigit(c)) {
    return (uint8_t)(c - 48);
  } else if (c == ' ') {
    return 36;
  } else if (c >= 'A' && c <= 'Z') {
    return (uint8_t)(c - 55);
  } else {
    return 255;
  }
}

//GeoFence, do not transmit over Yemen, North Korea and the UK
//GeoFence code by Matt Downs - 2E1GYP and Harry Zachrisson - SM7PNV
//Defined by the NoTXGrids that holds all the Maidehead grids for these locations
boolean OutsideGeoFence() {
  char TestGrid[4];
  boolean Outside;

  Outside = true;
  for (int GridLoop = 0; GridLoop < strlen_P(NoTXGrids); GridLoop = GridLoop + 5) {  //Itterate between Geo-Fenced grids
    for (int CharLoop = 0; CharLoop < 4; CharLoop++) {
      TestGrid[CharLoop] = pgm_read_byte_near(NoTXGrids + CharLoop + GridLoop);  //Copy a Grid string from program memory to RAM variable.
    }
    if ((GadgetData.WSPRData.MaidenHead4[0] == TestGrid[0]) && (GadgetData.WSPRData.MaidenHead4[1] == TestGrid[1]) && (GadgetData.WSPRData.MaidenHead4[2] == TestGrid[2]) && (GadgetData.WSPRData.MaidenHead4[3] == TestGrid[3])) {
      Outside = false;  //We found a match between the current location and a Geo-Fenced Grid
    }
  }

  return Outside;
}



//Type 3 call sign hash by RFZero www.rfzero.net modified by SM7PNV
uint32_t WSPRCallHash(const char *call) {
#define rot(x, k) ((x << k) | (x >> (32 - k)))

  uint32_t a, b, c;
  char CallWithSuPrefix[11];
  uint8_t Length = strlen(call);
  uint8_t TenDigit = 0;
  uint8_t Number;
  uint8_t CharLoop;
  Serial.print("Length ");
  Serial.print(Length);
  strcpy(CallWithSuPrefix, call);
  if (GadgetData.WSPRData.SuPreFixOption == Sufix) {
    CallWithSuPrefix[Length] = '/';      //Add slash at the end
    if (GadgetData.WSPRData.Sufix < 36)  //Single digit or letter
    {
      CallWithSuPrefix[Length + 2] = 0;  //Zero terminate
      if (GadgetData.WSPRData.Sufix < 10) {
        CallWithSuPrefix[Length + 1] = '0' + GadgetData.WSPRData.Sufix;  //Add a single digit
      } else {
        CallWithSuPrefix[Length + 1] = 'A' + (GadgetData.WSPRData.Sufix - 10);  //Add a single letter
      }
    } else  //Suffix is double digits
    {
      /* Seems the Type 3 decodes are not correct in case of two suffix numbers so this code is commented out for now as it will not be used by the Configurtion software
        Number=GadgetData.WSPRData.Sufix-36;
        while (Number>9)
        {
        ++TenDigit;
        Number -= 10;
        }
        CallWithSuPrefix[Length+1]='0'+TenDigit; //Add the Ten Digit
        CallWithSuPrefix[Length+2]='0'+Number; //Add the One Digit
        CallWithSuPrefix[Length+3]=0; //Zero terminate
      */
    }
  }  //if Sufix
  else if (GadgetData.WSPRData.SuPreFixOption == Prefix) {
    CallWithSuPrefix[0] = GadgetData.WSPRData.Prefix[0];
    CallWithSuPrefix[1] = GadgetData.WSPRData.Prefix[1];
    CallWithSuPrefix[2] = GadgetData.WSPRData.Prefix[2];
    CallWithSuPrefix[3] = '/';

    for (CharLoop = 0; CharLoop < Length; CharLoop++) {
      CallWithSuPrefix[CharLoop + 4] = call[CharLoop];
    }
  }  //else if Prefix

  Length = strlen(CallWithSuPrefix);
  // Serial.print(" : ");
  //Serial.println(Length);
  //Serial.print("{MIN} Call with Sufix=");
  //Serial.println(CallWithSuPrefix);

  a = b = c = 0xdeadbeef + Length + 146;

  const uint32_t *k = (const uint32_t *)CallWithSuPrefix;

  switch (Length)  // Length 3-10 chars, thus 0, 1, 2, 11 and 12 omitted
  {
    case 10:
      c += k[2] & 0xffff;
      b += k[1];
      a += k[0];
      break;
    case 9:
      c += k[2] & 0xff;
      b += k[1];
      a += k[0];
      break;
    case 8:
      b += k[1];
      a += k[0];
      break;
    case 7:
      b += k[1] & 0xffffff;
      a += k[0];
      break;
    case 6:
      b += k[1] & 0xffff;
      a += k[0];
      break;
    case 5:
      b += k[1] & 0xff;
      a += k[0];
      break;
    case 4: a += k[0]; break;
    case 3: a += k[0] & 0xffffff; break;
  }

  c ^= b;
  c -= rot(b, 14);
  a ^= c;
  a -= rot(c, 11);
  b ^= a;
  b -= rot(a, 25);
  c ^= b;
  c -= rot(b, 16);
  a ^= c;
  a -= rot(c, 4);
  b ^= a;
  b -= rot(a, 14);
  c ^= b;
  c -= rot(b, 24);

  c &= 0xFFFF;  // 15 bits mask

  return c;
}

//Only transmit on specific times
boolean CorrectTimeslot() {
  boolean CorrectSlot = false;
  uint8_t TestMinute;
  uint8_t ScheduleLenght;
  uint8_t SlotCode;
  TestMinute = GPSM;          //Test the Minute variable from the GPS time
  if ((TestMinute % 2) == 0)  //First check that it an even minute as WSPR transmissions only start on even minute
  {
    if (GadgetData.WSPRData.TimeSlotCode == 17)  //Tracker mode, only transmit when the transmitter is moving
    {
      CorrectSlot = (NewPosition() || (TestMinute == 0));  //Transmit only if the tracker has moved since last transmisson or at top of an Hour
    } else if (GadgetData.WSPRData.TimeSlotCode == 16)     //No scheduling
    {
      CorrectSlot = true;
    } else if (GadgetData.WSPRData.TimeSlotCode == 15)  //Band coordinated scheduling
    {
      switch (CurrentBand) {
        case 2:  //160m band
          CorrectSlot = (TestMinute == 0 || TestMinute == 20 || TestMinute == 40);
          break;
        case 3:  //80m band
          CorrectSlot = (TestMinute == 2 || TestMinute == 22 || TestMinute == 42);
          break;
        case 4:  //40m band
          CorrectSlot = (TestMinute == 6 || TestMinute == 26 || TestMinute == 46);
          break;
        case 5:  //30m band
          CorrectSlot = (TestMinute == 8 || TestMinute == 28 || TestMinute == 48);
          break;
        case 6:  //20m band
          CorrectSlot = (TestMinute == 10 || TestMinute == 30 || TestMinute == 50);
          break;
        case 7:  //17m band
          CorrectSlot = (TestMinute == 12 || TestMinute == 32 || TestMinute == 52);
          break;
        case 8:  //15m band
          CorrectSlot = (TestMinute == 14 || TestMinute == 34 || TestMinute == 54);
          break;
        case 9:  //12m band
          CorrectSlot = (TestMinute == 16 || TestMinute == 36 || TestMinute == 56);
          break;
        case 10:  //10m band
          CorrectSlot = (TestMinute == 18 || TestMinute == 38 || TestMinute == 58);
          break;
        default:
          CorrectSlot = true;  // band does not have schedule, allow it to transmit right now, this applies to 1290m,630m and all bands above 10m
          break;
      }                                              //switch
    }                                                //else if
    else if (GadgetData.WSPRData.TimeSlotCode < 15)  //Schedule is on the minute set on Timeslotcode * 2  E.g if Timeslotcode is 3 then minute 06,16,26,36,46 and 56 is used for transmissions.
    {
      if (GadgetData.WSPRData.TimeSlotCode < 5) {
        ScheduleLenght = 10;
        SlotCode = GadgetData.WSPRData.TimeSlotCode;
      } else {
        ScheduleLenght = 20;
        SlotCode = GadgetData.WSPRData.TimeSlotCode - 5;
      }   //if TimeSlotCode <5
      do  //Remove the ten minute digit and just leave the minute digit
      {
        if (TestMinute > ScheduleLenght - 1) TestMinute = TestMinute - ScheduleLenght;
      } while (TestMinute > ScheduleLenght - 1);
      CorrectSlot = (TestMinute == (SlotCode * 2));  //if the TimeSlotcode multiplied with 2 (only even minutes) is matching the current minute digit then transmit
    }                                                //else if
  }
  return CorrectSlot;
}
