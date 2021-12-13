#include <Arduino.h>
#include "LowEnergyBluetooth.h"
#include "LocationAndTime.h"
#include "ClassicEnergyBluetooth.h"
#include "JSONING.h"
#include "Internet.h"
#include <ArduinoJson.h>

//SHIFT + ALT + F INDENT HOT KEYS
#define LED_SNIFF_PIN 16
#define LED_Internet_pin 17

volatile bool gps_function_on_start_flagg;
extern volatile bool low_energy_initialize_flag;
int sniffed_macs_global_counter;

//int * pointer;
String Sniffed_Mac_Addresses[150];
//pointer = malloc(300*sizeof(String));
//char mac_ad[18]
//String Sniffed_Mac_Addresses = (String*)malloc(sizeof(String)*300);
//stringArray = malloc(sizeof(char*)*300);
//stringArray[i] = malloc(256*sizeof(char));

//char * Sniffed_Mac_Addresses[300];
//const int size_of_addres = 19;

unsigned long currentMillis;
unsigned long previousMillis;
const unsigned long time_to_sniff_low_Energy = 1000 * 5;
const unsigned long time_to_sniff_Classic_Energy = ((time_to_sniff_low_Energy) + (1000 * 21));
const unsigned long time_to_reset = 1000 * 60 * 20; //After 10 minutes I want to reset everything
unsigned long reset_previousMillis;

volatile bool lowenergy_duplicate_printed_flagg;
volatile bool classic_energy_duplicate_printed_flag;
extern int TotalClassicMacs;
extern int TotalLowEnergyMACS;

extern float latitude;
extern float longitude;
extern String Tym;

extern bool Internet_setup_flag;
extern bool internet_sending_flagg;

volatile bool done_jsondocumenting_flagg;
volatile bool check_location_change_flagg;
extern volatile bool low_energy_initialize_flag;
extern volatile bool ble_end_flag;
extern bool first_time_posting_flagg;

uint8_t number_b4_upload = 0;
int number_of_trips; //this increases everytime I press the button
extern volatile bool start_inq_flag;

bool finished_recursion_flagg;
bool allocate_flagg;

//the following variables are for use with the button
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 15;   // the number of the pushbutton pin

// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
//end of button variables
extern volatile bool scan_is_on;

String trip_number= "1";
extern volatile bool end_classic_scan_flagg;

//the following variables are for additional data on the distance by speed of BLE experiment
String b_tym; //begin scanning
String e_tym; // end scanning do we need this otherwise we just add 20 seconds to the start time

float b_lat; //begin latitude
float e_lat; //end latitude

float b_lon; //begin longitude
float e_lon; //end scanning longitude

double b_speed; //begin scan speed
double e_speed;  //end scan speed

int rssi[150]; //collect the rssi data on each peripheral

void free_ram();
void mark_trip();
void trip_init();

void setup()
{
  pinPeripheral(2, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);

  pinPeripheral(5, PIO_SERCOM_ALT); //gsm sim 800l
  pinPeripheral(6, PIO_SERCOM_ALT); //gsm sim 800l

  pinMode(buttonPin, INPUT);//button
  pinMode(LED_SNIFF_PIN, OUTPUT);
  pinMode(LED_Internet_pin, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
  BTserial.begin(38400);
  gsmSerial.begin(9600);

  gps_function_on_start_flagg = true;
  low_energy_initialize_flag = true;

  lowenergy_duplicate_printed_flagg = false;
  classic_energy_duplicate_printed_flag = false;
  done_jsondocumenting_flagg = false;
  Internet_setup_flag = false;
  check_location_change_flagg = false;
  internet_sending_flagg = false;
  low_energy_initialize_flag = true;
  ble_end_flag = true;
  first_time_posting_flagg = true;
  finished_recursion_flagg = false;
  allocate_flagg = true;
  scan_is_on=false;

  /*  for(int i=0; i<300; i++)
  {
    Sniffed_Mac_Addresses[i] = (char *)malloc(sizeof(char)*size_of_addres);
  }
  */
  pinMode(LED_BUILTIN, OUTPUT);
  sniffed_macs_global_counter = 0;
  number_of_trips = 0;
  delay(10000);
  uint16_t capa = 128*20;
  Serial.print("The capacity created is: ");
  Serial.println(capa);
  delay(10000);
  delay(2000);
  hc_init();
  Internet_initialization();
  //trip_init();
}

void loop()
{
  int reading = digitalRead(buttonPin);
  //Internet_setup_flag=true;
  if (Internet_setup_flag)
  {
    currentMillis = millis();

    if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    }


  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        Serial.println("Button Pressed");
        Serial.println("Ending trip..."); //when ending trip all variables must be reset too like end of a cycle
        mark_trip();
        /*if (atleast_one_post_failed == true) //this means we have a post that failed at some point
          {
          atleast_one_post_failed = false;
          //now we write the data to the SD card
          Serial.println("failed posts to the SD card...");
          SaveToSDCard();
          }
          else {
          Serial.println("There are no failed posts to write to the SD card");
          }
        */
        //show that we are switcing back to BLE mode at top of loop
        Serial.println("==============================================");
        Serial.println("==============================================");
        Serial.println(F("END BUTTON SEQUENCE... "));
        Serial.println("==============================================");
        Serial.println("==============================================");
        //ButtonResetting();
      }
    }
  }


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  //end of button

    //the bigger loop should be checking for speed
    //
    //Serial.println("Hello");
    

    //To do: Check if the location has changed the increase the time_and_location_counter
    /*
    if (check_location_change_flagg)
    {
      if (has_location_changed())
      {
        gps_function_on_start_flagg = true;
      }

      else
      {
        //current_time();
        gps_function_on_start_flagg = false;
        previousMillis = millis(); // this is tryung hard for the sniffing of low energy to have enough time and not have its time eaten away
      }
    }

    if (gps_function_on_start_flagg == true)
    {
      //the first time we have to get location without chacking if it has changed, it is the first time after all
      //check_location_change_flagg = true;

      //In this loop I get the location and time on start up
      //To do: Get the time and location
      gps_function_on_start();
      gps_function_on_start_flagg = false;
      //delay(5000);
    }
    */

    //To do: Check if the location has changed the increase the time_and_location_counter
    //has_location_changed();

    //To do: Sniff Low Energy Bluetooth for about 10 seconds

    if (((currentMillis - previousMillis) < time_to_sniff_low_Energy))
    {
      digitalWrite(LED_Internet_pin,LOW);
      digitalWrite(LED_SNIFF_PIN,HIGH);
      Sniff_Low_Energy_bluetooth();
      //delay(1000);
      //hc_sniff();
    }

    //To do: Check for duplicates
    //LowEnergyCheckForDuplicates();

    //To do: Sniff Classic Energy Bluetooth
    //if (((currentMillis - previousMillis) >= time_to_sniff_low_Energy) && ((currentMillis - previousMillis) < time_to_sniff_Classic_Energy))
    if (((currentMillis - previousMillis) >= time_to_sniff_low_Energy) && (!end_classic_scan_flagg))
    {
      end_classic_scan_flagg=true; //since we are not scanning classic BT

      //Serial.print("latitude "); Serial.println(latitude);
      //Serial.print("longitude "); Serial.println(longitude);
      //Serial.print("Tym "); Serial.println(Tym);
      //delay(10000);
      if (lowenergy_duplicate_printed_flagg == false)
      {
        lowenergy_duplicate_printed_flagg = true;
        LowEnergyCheckForDuplicates();
      }

      if (ble_end_flag)
      {
        Serial.println(F("ending bluetooth scanning first"));
        ble_end_flag = false;
        scan_is_on=false;
        free_ram();      
      }

      //hc_sniff(); //since we are not scanning BT
      internet_sending_flagg = true;
    }

    //Check for duplicates
    //ClassicEnergyCheckForDuplicates();

    //To do: Send to database
    //if (((currentMillis - previousMillis) >= time_to_sniff_low_Energy) && ((currentMillis - previousMillis) >= time_to_sniff_Classic_Energy) && (internet_sending_flagg == true)) //&& ((currentMillis - previousMillis) < time_to_temporary_internet))
    if((end_classic_scan_flagg==true)&&(internet_sending_flagg == true))
    {
      end_classic_scan_flagg=false;
      if (classic_energy_duplicate_printed_flag == false)
      {
        BLE.end();
        classic_energy_duplicate_printed_flag = true;
        ClassicEnergyCheckForDuplicates();
      }

      free_ram();

      Serial.println("Hello");
      //delay(1000);
      if (done_jsondocumenting_flagg == false)
      {
        gps_function_on_start();
        Serial.print("The number of new passengers is ");
        Serial.println(sniffed_macs_global_counter - number_b4_upload);
        Serial.println();
        //if new pass is more than zero we go else we restart sniff
        if ((sniffed_macs_global_counter - number_b4_upload) > 0)
        {
          //recursion_before_calling_domumentingJson(sniffed_macs_global_counter - number_b4_upload);
          AlldomumentingJson();

          if (finished_recursion_flagg == true)
          {
            finished_recursion_flagg = false;
            digitalWrite(LED_Internet_pin,HIGH);
            digitalWrite(LED_SNIFF_PIN,LOW);
            Send_To_The_Internet();
            digitalWrite(LED_Internet_pin,LOW);
          }
        }

        else
        {
          internet_sending_flagg = false;
          Serial.println("No new passengers to upload");
          Serial.println("======================================================================================");

          //memset(Json_output_string, 0, sizeof(Json_output_string));
          doc_how_many_times = 0;
          doc_how_many_times_remainder = 0;
          counter = 0;
          done_jsondocumenting_flagg = false;
          internet_sending_flagg = false;
          start_inq_flag = true;

          //the following resets will remove continuity suc as finding the nw passengers etc
          //memset(Sniffed_Mac_Addresses, 0, sizeof(Sniffed_Mac_Addresses));   
          //for (int e = 0; e < 300; e++)
          //{
            //(Sniffed_Mac_Addresses[e]) = '\0';
          //}       
          //allocate_flagg = true;
          number_b4_upload = 0;
          sniffed_macs_global_counter = 0;
          output_string_counter = 0;
          
          previousMillis = millis();
        }
      }

      //delay(1000);
      number_b4_upload = sniffed_macs_global_counter;
      TotalClassicMacs = 0;
      //memset(Json_output_string, 0, sizeof(Json_output_string));
      gps_function_on_start_flagg = true;
      classic_energy_duplicate_printed_flag = false;
      lowenergy_duplicate_printed_flagg = false;
      TotalLowEnergyMACS = 0;
      ble_end_flag = true;
      low_energy_initialize_flag = true;
      start_inq_flag = true;

      number_b4_upload = 0;
      sniffed_macs_global_counter = 0;
      output_string_counter = 0; 
      //memset(Sniffed_Mac_Addresses, 0, sizeof(Sniffed_Mac_Addresses));  
      //for (int e = 0; e < 300; e++)
      //{
        //(Sniffed_Mac_Addresses[e]) = '\0';
      //}

      previousMillis = millis();
    }
 
  }
}


void free_ram()
{
  //Serial.println("CHECKING RESIDUAL NONSENSE FOR RAM RELEASE");
  Serial.println("==========================RELEASE==============RAM====================================");
  //Serial.println("======================================================================================");
  //for(int d=0; d<400; d++)
  //{
  //Serial.println(Sniffed_Mac_Addresses[d]);
  //}
  //Serial.println("=======================================That's done====================================");
  //Serial.println("======================================================================================");

  //Serial.println("HAVE WE CLEARED ?");
  //Serial.println("======================================================================================");
  //Serial.println("======================================================================================");
  for (int e = sniffed_macs_global_counter; e < 150; e++)
  {
    (Sniffed_Mac_Addresses[e]) = '\0';
    rssi[e]=0;
  }

  //for(int f=0; f<400; f++)
  //{
  //Serial.println(Sniffed_Mac_Addresses[f]);
  //}
  Serial.println("==================================DONE================================================");
  Serial.println("======================================================================================");
  return;
}

void mark_trip()
{
      digitalWrite(LED_Internet_pin,HIGH);
      digitalWrite(LED_SNIFF_PIN,LOW);

      Serial.println("Resetting variables");
      number_b4_upload = sniffed_macs_global_counter;
      if(scan_is_on)
      {
        BLE.end();
      }
      TotalClassicMacs = 0;
      //memset(Json_output_string, 0, sizeof(Json_output_string));
      gps_function_on_start_flagg = true;
      classic_energy_duplicate_printed_flag = false;
      lowenergy_duplicate_printed_flagg = false;
      TotalLowEnergyMACS = 0;
      ble_end_flag = true;
      low_energy_initialize_flag = true;
      start_inq_flag = true;

      number_b4_upload = 0;
      sniffed_macs_global_counter = 0;
      output_string_counter = 0; 
      //memset(Sniffed_Mac_Addresses, 0, sizeof(Sniffed_Mac_Addresses));  
      for (int e = 0; e < 300; e++)
      {
        (Sniffed_Mac_Addresses[e]) = '\0';
      }

    previousMillis = millis();
    Serial.println(F("GSM has already been initiated continue..."));
    Serial.print(F("Get IP Address... "));
    Serial.println(http.getIP());
    //delay(1000);

    gps_function_on_start();
    //calculate the capacity of json document
    int capa = JSON_ARRAY_SIZE(1) + 1 * JSON_OBJECT_SIZE(6);
    DynamicJsonDocument doc(capa); //this was the original before trying to fix the output issue

    // create an object
    JsonObject root = doc.createNestedObject();
    root["trip"] = "00:00:00:00:00:00";
    root["lat"] = latitude;
    root["lon"] = longitude;
    root["time"] = Tym;

    number_of_trips++;

    // serialize the object and send the result to Serial
    Serial.println("POST...");
    serializeJson(doc, Serial);
    serializeJson(doc,Json_output_string[0]);
    doc.clear();

    http.post("gjm.yjg.mybluehost.me/kptbot.php", Json_output_string[0], "application/json");

    for(int ok=0; ok<20; ok++)
    {
        Json_output_string[ok] = '\0';
    }

    String get_rx_bffr;
    String rx_bffr_withoutCodes;
    String keys;
    String paid_passengers;
    
    Serial.println(); 
    Serial.print("Get... ");
    Serial.println(http.get("gjm.yjg.mybluehost.me/kptbot.php",true));
    get_rx_bffr = http.get("gjm.yjg.mybluehost.me/kptbot.php",true);
    Serial.println("this is what I have received");
    Serial.println(get_rx_bffr);

    int pos = get_rx_bffr.indexOf("}}");
    //int len = get_rx_bffr.length();
    trip_number = get_rx_bffr.substring(pos-1, pos);
    Serial.print("The next trip number is ");
    Serial.println(trip_number);

    previousMillis = millis();

}

void trip_init()
{

    Serial.println("Lets Get the trip number and start trip");
    Serial.println("=====================================================================");
    String get_rx_bffr;
    /*
    Serial.print(F("First close GPRS... "));
    Serial.println(http.closeConn());
    delay(1000);

    Serial.print(F("Now connect GPRS."));

    while (!http.isConnected())
    {
        Serial.print(F("."));
        Serial.println(http.connect());
        delay(500);
    }
    */

    Serial.println(F("GSM has already been initiated continue..."));
    Serial.print(F("Get IP Address... "));
    Serial.println(http.getIP());
    delay(1000);

    
    Serial.println(); 
    Serial.print("Get... ");
    Serial.println(); 
    //Serial.println(http.get("gjm.yjg.mybluehost.me/kptbot.php",true));
    get_rx_bffr = http.get("gjm.yjg.mybluehost.me/kptbot.php",true);
    //Serial.println("this is what I have received");
    //Serial.println(get_rx_bffr);

    int pos = get_rx_bffr.indexOf("}}");
    //int len = get_rx_bffr.length();
    trip_number = get_rx_bffr.substring(pos-1, pos);
    Serial.print("This is trip number ");
    Serial.println(trip_number);

    previousMillis = millis();
}