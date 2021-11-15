#ifndef LOWENERGYBLUETOOTH_H
#define LOWENERGYBLUETOOTH_H

#include <ArduinoBLE.h>

BLEDevice peripheral;
extern String Sniffed_Mac_Addresses[];
//extern char * Sniffed_Mac_Addresses[];
volatile bool low_energy_initialize_flag;
extern int sniffed_macs_global_counter;
int TotalLowEnergyMACS;
volatile bool ble_end_flag;
extern const int size_of_addres;

extern const unsigned long time_to_sniff_low_Energy;
const unsigned long time_to_sniff_low_Energy_for_returning = ((time_to_sniff_low_Energy) - (1000 * 1)); // this will terminate earlier than the time allowing it to run
extern unsigned long currentMillis;
extern unsigned long previousMillis;

volatile bool scan_is_on;

//char *sniffed;//[18][300];
//malloc(sizeof(char*)*300);

void LowEnergyCheckForDuplicates();

void Sniff_Low_Energy_bluetooth()
{
    if(low_energy_initialize_flag)
    {
        low_energy_initialize_flag=false;

          /*for(int i=0; i<300; i++)
          {
            Sniffed_Mac_Addresses[i] = (char *)malloc(sizeof(char)*size_of_addres);
          }
            */
           BLE.end();
        // begin initialization
        if (!BLE.begin()) {
            Serial.println(F("starting BLE failed!"));
            while (1);
        }

        Serial.println(F("BLE Central scan"));
        // start scanning for peripheral
        BLE.scan();
        scan_is_on = true;
    }

  //check if a peripheral has been discovered
  peripheral = BLE.available();

  //if ((currentMillis - previousMillis) < time_to_sniff_low_Energy_for_returning) //i now need all functions to return so this will require time since they all depend on time to finish
  //if((sniffed_macs_global_counter<150) || ((currentMillis - previousMillis) <= time_to_sniff_low_Energy_for_returning))
  if(sniffed_macs_global_counter<150)
  {
    //Sniffed_Mac_Addresses[sniffed_macs_global_counter] = (char *)malloc(sizeof(char)*size_of_addres);
    if ((peripheral)) {
        // discovered a peripheral
        Serial.println(F("Discovered a peripheral"));
        Serial.println(F("----------------------------------------"));

        // print address
        Serial.print(F("Address: "));
        Serial.println(peripheral.address());
        Sniffed_Mac_Addresses[sniffed_macs_global_counter] = peripheral.address();
        //strcpy(Sniffed_Mac_Addresses[sniffed_macs_global_counter], peripheral.address().c_str());
        // print the local name, if present
        if (peripheral.hasLocalName()) {
          Serial.print(F("Local Name: "));
          Serial.println(peripheral.localName());
        }

        sniffed_macs_global_counter = sniffed_macs_global_counter + 1;
        TotalLowEnergyMACS = TotalLowEnergyMACS+1;
        Serial.print(TotalLowEnergyMACS);
        Serial.println(" Devices from low energy bluetooth");
        Serial.println();
        Serial.println(F("========================================"));
    }
  }

  else{
      return;
      //LowEnergyCheckForDuplicates();
  }


    //TotalLowEnergyMACS //= sniffed_macs_global_counter;
    /*    Serial.println(F("========================================"));
    Serial.print(TotalLowEnergyMACS); Serial.println(" Final total devices from low energy bluetooth");
    Serial.println(F("========================================"));
    */
}

void LowEnergyCheckForDuplicates()
{
    int i, j, k;
    int old_number_of_macs = sniffed_macs_global_counter;
 /*   
    Serial.println("Function: LowEnergyCheckForDuplicates");
    Serial.println("The following are the LOW ENERGY sniffed Mac Addresses:");
    Serial.println(F("========================================")); 

    for(int m = 0; m< sniffed_macs_global_counter; m++)
    {
        Serial.println(Sniffed_Mac_Addresses[m]);
    }
    Serial.println(F("========================================"));
    Serial.println(F("========================================"));
*/    

    //lets sort them and remove the duplicates

    for( i=0; i<sniffed_macs_global_counter; i++)
    {
        for( j=i+1; j<sniffed_macs_global_counter; j++)
        {
            /* If any duplicate found */
            if ((strcmp(((Sniffed_Mac_Addresses[i].c_str())), ((Sniffed_Mac_Addresses[j].c_str()))) == 0)) 
            //if((strcmp(((Sniffed_Mac_Addresses[i])), ((Sniffed_Mac_Addresses[j]))) == 0))
            {
                /* Delete the current duplicate element */
                for(k=j; k<sniffed_macs_global_counter-1; k++)
                {
                    Sniffed_Mac_Addresses[k] = Sniffed_Mac_Addresses[k+1];
                }
                /* Decrement size after removing duplicate element */
                sniffed_macs_global_counter--;

                /* If shifting of elements occur then don't increment j */
                j--;
            }
        }
    }

    Serial.println("Function: LowEnergyCheckForDuplicates");
    Serial.print("Old Unsorted Array had: "); Serial.print(old_number_of_macs); Serial.println(" Elements");
    Serial.print("New Sorted Array has: "); Serial.print(sniffed_macs_global_counter); Serial.println(" Elements");
    Serial.println(F("========================================")); 

    for(int m = 0; m< sniffed_macs_global_counter; m++)
    {
        Serial.println(Sniffed_Mac_Addresses[m]);
    }
    Serial.println(F("========================================"));
    Serial.println(F("========================================"));
    //delay(100);
    return;
}
#endif