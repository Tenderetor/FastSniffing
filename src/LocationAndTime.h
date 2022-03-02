#ifndef LOCATIONANDTIME_H
#define LOCATIONANDTIME_H

#include <string.h>
#include "TinyGPSplus.h"

TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
float latitude;
float longitude;
String Tym;
extern unsigned long previousMillis;

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

void gps_function_on_start() {

  Serial.println(F("location"));
  // while ((Serial1.available() > 0) && (Serial1.read() != '\r')) 
  // {
  //   smartDelay(100);
  // }

    // while (Serial1.available() > 0)
    // gps.encode(Serial1.read());

    Serial.print(F("Latitude: "));
    Serial.println(gps.location.lat(), 6);
    latitude = gps.location.lat();
    Serial.print(F("Longitude: "));
    Serial.println(gps.location.lng(), 6);
    longitude = gps.location.lng();
    Serial.println(F(""));

  String _tym_;

  //Serial.print(F("Time: "));
  if (gps.date.isValid())
  {
    _tym_ += gps.date.month();
    _tym_ += "/";
    _tym_ += gps.date.day();
    _tym_ += "/";
    _tym_ += gps.date.year();
  }
  else
  {
    _tym_ = "DATE_INVALID";
  }

  _tym_ += " ";
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.hour();
    _tym_ += ":";
    if (gps.time.minute() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.minute();
    _tym_ += ":";
    if (gps.time.second() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.second();
  }
  else
  {
    _tym_ = "TIME INVALID";
  }
 
  Serial.print("Inside gps functon the returned tym ->");
  Serial.println(_tym_);
  Tym =_tym_;
  _tym_ ='\0';
  //Serial.println();    


  //previousMillis = millis();
  return;
//next clear the buffers and store the location values
}

void current_time()
{
  String _tym_;

    while ((Serial1.available() > 0) && (Serial1.read() != '\r')) 
  {
    smartDelay(1000);
  }

  //Serial.print(F("Time: "));
  if (gps.date.isValid())
  {
    _tym_ += gps.date.month();
    _tym_ += "/";
    _tym_ += gps.date.day();
    _tym_ += "/";
    _tym_ += gps.date.year();
  }
  else
  {
    _tym_ = "DATE INVALID";
  }

  _tym_ += " ";
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.hour();
    _tym_ += ":";
    if (gps.time.minute() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.minute();
    _tym_ += ":";
    if (gps.time.second() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.second();
  }
  else
  {
    _tym_ = "TIME INVALID";
  }
 
 // Serial.print("Inside gps functon the returned tym ->");
  //Serial.println(_tym_);
  Tym =_tym_;
  _tym_ ='\0';
  //Serial.println();    

 //previousMillis = millis();
 return;
}

String printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  String tyming;

  if (!d.isValid())
  {
    //Serial.print(F("********** "));
    tyming += "DATE INVALID";
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    //Serial.print(sz);
    tyming += sz;
  }
  
  if (!t.isValid())
  {
    //Serial.print(F("******** "));
    tyming += "TIME INVALID";
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    //Serial.print(sz);
    tyming += sz;
  } 

  smartDelay(0);

  return tyming;
}

bool has_location_changed()
{
    bool location_changed_status = false;

    //Serial.println("Location change function:");
    if (gps.location.isValid())
    {
      double travelled =
        TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          latitude,
          longitude);
 

      //Serial.print(F("Distance moved since lasttime:     "));
      Serial.print(travelled);
      //Serial.print(F(" m Since last time"));

      if(travelled >= 200) //we change location after every 200meters
      {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        location_changed_status = true;
      }

      else{
          location_changed_status = false;
      }

    } 
    else{
        //Serial.println("LOCATION INVALID");
        //location_changed_status = false;
    }

    return location_changed_status;
}
#endif