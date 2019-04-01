/*
 * Tutorial sketch to automatically pair, bind, and link
 * an HC-05 with an HC-06
 * 
 * 
 * ENSURE YOUR WIRING IS CORRECT:
 * 
 * HC-05:
 * ------------------------------------------------------------
 * BT VCC  <-->  Arduino 5V
 * BT GND  <-->  Arduino GND
 * BT TX   <-->  Arduino RX (Serial1 RX or D4)
 * BT RX   <-->  Arduino TX (Serial1 TX or D5)
 * 
 * HC-06:
 * ------------------------------------------------------------
 * BT VCC  <-->  Arduino 5V
 * BT GND  <-->  Arduino GND
 * BT TX   <-->  Arduino RX (Serial2 RX or D2)
 * BT RX   <-->  Arduino TX (Serial2 TX or D3)
*/

//here we kindly ask precompiler if the board the sketch is being uploaded to
//has multiple hardware serial ports - if so compile the bluetooth modules'
//ports as the hardware ones (i.e. Serial1) and if not, compile the bluetooth 
//ports as SoftwareSerial ports
#if defined(Serial1) && defined(Serial2)

  #define MASTER_PORT Serial1         //HC-05 Port
  #define SLAVE_PORT  Serial2         //HC-06 Port
  
#else

  #include <SoftwareSerial.h>

  SoftwareSerial MASTER_PORT(4, 5);   // RX | TX - HC-05 Port
  SoftwareSerial SLAVE_PORT(2, 3);    // RX | TX - HC-06 Port
  
#endif




#define MAX_NUM_BT_DEVICES  10        //max number of bluetooth devices allowed to be discovered 
                                      //(used for when the HC-05 tries to detect the HC-06)



//name of the HC-06 as defined by the user
String rname = "";

//name of the discovered bluetooth device
String discovered_rname = "";

//list of all bluetooth device addresses discovered by the HC-05
String addressList[MAX_NUM_BT_DEVICES] = {""};

//address ID of the HC-06
String ID = "";

//address of a discovered bluetooth device
String discovered_ID = "";




void setup()
{
  //initialize serial ports
  Serial.begin(115200);
  MASTER_PORT.begin(38400);
  SLAVE_PORT.begin(9600);
  
  //wait for the debugging port to come online
  while (!Serial);

  //set the reading timeouts for each serial port to 100ms
  Serial.setTimeout(100);
  MASTER_PORT.setTimeout(100);
  SLAVE_PORT.setTimeout(100);




  //instruction prints
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println(F("Put HC-05 into AT mode"));
  Serial.println(F("Ensure Serial Montior line-ending is 'Newline'"));
  Serial.println(F("Enter the desired name of the HC-06"));
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println();

  //wait for user input
  while (!Serial.available());

  //read in the user's desired name for the HC-06 and get rid of the newline
  rname = Serial.readString();
  rname = rname.substring(0, rname.length() - 1);

  //HC-06's name is only allowed up to 20 characters - truncate the name
  //if the user tries to put in something longer than 20 characters
  if (rname.length() > 20)
  {
    //truncate the name
    rname = rname.substring(0, 20);

    //let the user know the name was truncated
    Serial.println("\tEntered name is too long - truncated to 20 characters");
  }

  //debugging prints
  Serial.println("Entered name is: " + rname);
  Serial.println();




  //send required AT commands to the HC-05
  sendAT_master(F("AT"));
  sendAT_master(F("AT+ROLE=0"));
  sendAT_master(F("AT+UART=9600,0,0"));
  sendAT_master(F("AT+CMODE=0"));
  sendAT_master(F("AT+PSWD=1234"));
  Serial.println();





  //send required AT commands to the HC-06
  sendAT_slave(F("AT"));
  sendAT_slave("AT+NAME" + rname);
  sendAT_slave(F("AT+BAUD4"));
  sendAT_slave(F("AT+PIN1234"));
  Serial.println();




  //send required AT commands to the HC-05
  sendAT_master(F("AT+RMAAD"));
  sendAT_master(F("AT+ROLE=1"));
  sendAT_master(F("AT+RESET"));
  Serial.println();




  //instruction prints
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println(F("Put HC-05 back into AT mode, press and hold the HC-05 button, and send 'OK' through Serial Monitor"));
  Serial.println(F("\tMAKE SURE THE HC-05's BUTTON IS PRESSED UNTIL TOLD OTHERWISE!!!"));
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println();

  //wait for user input
  while (Serial.read() != 'O');




  //send required AT commands to HC-05
  sendAT_master(F("AT"));
  sendAT_master(F("AT+CMODE=0"));
  sendAT_master(F("AT+INQM=0,5,9"));
  sendAT_master(F("AT+INIT"));
  sendAT_master(F("AT+INQ"));
  Serial.println();



  //instruction prints
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println(F("HC-05's button no longer needs to be pressed"));
  Serial.println();
  Serial.println(F("Put HC-05 back into AT mode, press and hold the HC-05 button, and send 'OK' through Serial Monitor"));
  Serial.println(F("\tMAKE SURE THE HC-05's BUTTON IS PRESSED UNTIL TOLD OTHERWISE!!!"));
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println();

  //wait for user input
  while (Serial.read() != 'O');




  //send required AT commands to HC-05
  sendAT_master(F("AT"));
  sendAT_master(F("AT+INIT"));

  //solicit the name of each bluetooth device discovered
  //until one of the devices has the same name as the HC-06
  for (byte i = 0; i < MAX_NUM_BT_DEVICES; i++)
  {
    //determine if we found any bluetooth devices
    if (addressList[i].length() != 0)
    {
      //request the name of the current device
      sendAT_master("AT+RNAME?" + addressList[i]);

      //spacer print for readability
      Serial.println();

      //test if the current device's name is the same as the
      //HC-06's name
      if (discovered_rname.equals(rname))
      {
        //save the HC-06's address
        discovered_ID = addressList[i];

        //reformat the HC-06's address for future AT commands
        addressList[i].replace(",", ":");

        //debugging print
        Serial.println("\tHC-06 found at address: " + addressList[i]);

        //break the for loop - no need to continue searching
        //for something we have already found
        break;
      }
    }
    else
    {
      //debugging print
      Serial.println(F("\tERROR - NO BT DEVICES FOUND"));

      //halt program
      while (1);
    }

    //determine if we have tested every device and still haven't found
    //the HC-06 we're looking for
    if (i == (MAX_NUM_BT_DEVICES - 1))
    {
      //debugging print
      Serial.println(F("\tERROR - HC-06 NOT FOUND"));

      //halt program
      while (1);
    }
  }

  //send required AT commands to the HC-05
  sendAT_master("AT+PAIR=" + discovered_ID + ",20");
  sendAT_master("AT+BIND=" + discovered_ID);
  sendAT_master(F("AT+CMODE=1"));
  sendAT_master("AT+LINK=" + discovered_ID);
  Serial.println();




  //instruction prints
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println(F("HC-05's button no longer needs to be pressed - Modules shoud be paired! :D"));
  Serial.println(F("<<<=====================================================================>>>"));
  Serial.println();
}




void loop()
{
  //do nothing
}




//send AT command to HC-05 - Baud: 38400, Line ending: Newline
void sendAT_master(String command)
{
  String temp = "";
  String response = "";

  //variables used for the millis() timer - set at 5sec by default
  unsigned long timeBench;
  unsigned long currentTime = timeBench;
  unsigned int duration = 5000;

  //array index counter
  byte index = 0;

  //debugging print
  Serial.println(command);

  //make sure port is active (needed when using multiple software serial ports)
  MASTER_PORT.listen();

  //send AT command to the HC-05
  MASTER_PORT.println(command);

  //wait for response
  while (!MASTER_PORT.available());

  //extra processing for responses to specific AT commands
  if (command.equals(F("AT+INQ")))
  {
    //use a millis() timer to allow the HC-05 to discover as many bluetooth devices as it can
    //timer is set to 5sec by default
    timeBench = millis();
    currentTime = timeBench;
    while ((currentTime - timeBench) <= duration)
    {
      currentTime = millis();

      //read the address of a newly discovered bluetooth device
      temp = MASTER_PORT.readString();

      //temp == -1 means there was a timeout trying to read the response from the HC-05
      if ((temp != -1) && (index < MAX_NUM_BT_DEVICES))
      {
        //reformat the new address and save it to the list of bluetooth device adresses
        addressList[index] = temp.substring(temp.indexOf(":") + 1, temp.indexOf(":") + 15);
        addressList[index].replace(":", ",");

        index++;
      }

      //tack on the device address to response - this allows us to see all device
      //addresses when printing "response" for debugging
      response = response + temp;
    }
  }
  else if (command.indexOf(F("RNAME?")) == 3)
  {
    //read response from the HC-05
    discovered_rname = MASTER_PORT.readString();

    //reformat the response by getting rid of unecessary characters, leaving only the device name
    discovered_rname = discovered_rname.substring(discovered_rname.indexOf(':') + 1, discovered_rname.indexOf('\n') - 1);

    //set the response eaqual to the discovered name - this allows us to see the
    //current device name when printing "response" for debugging
    response = discovered_rname;
  }
  else
  {
    //read response from the HC-05
    response = MASTER_PORT.readString();
  }

  //print HC-05's response for debugging
  Serial.print(response);

  return;
}




//send AT command to HC-06 - Baud: 9600, Line ending: None
void sendAT_slave(String command)
{
  String response = "";

  //debugging print
  Serial.println(command);

  //make sure port is active (needed when using multiple software serial ports)
  SLAVE_PORT.listen();

  //send AT command to HC-06
  SLAVE_PORT.print(command);

  //wait for response
  while (!SLAVE_PORT.available());

  //read response
  response = SLAVE_PORT.readString();

  //debugging print
  Serial.println(response);

  return;
}
