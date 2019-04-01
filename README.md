# HC05_HC06_Pair_Link_Bind_Tutorial
Code to automatically, pair, link, and bind an HC-05 with an HC-06 bluetooth module. This sketch will work with ANY Arduino as long as the modules/Arduino are wired correctly.


Wiring for Arduino with multiple hardware Serial ports (Mega used as an example):
![Mega](https://user-images.githubusercontent.com/20977405/55363612-9fe16380-54ab-11e9-859e-104b55b5ee62.png)


Wiring for Arduino with no hardware Serial ports (Nano used as an example):
![Nano](https://user-images.githubusercontent.com/20977405/55363613-9fe16380-54ab-11e9-8922-9764f44430b0.png)

Example Output (Serial Monitor):

<<<=====================================================================>>>
Put HC-05 into AT mode
Ensure Serial Montior line-ending is 'Newline'
Enter the desired name of the HC-06
<<<=====================================================================>>>

Entered name is: myBT

AT
OK
AT+ROLE=0
OK
AT+UART=9600,0,0
OK
AT+CMODE=0
OK
AT+PSWD=1234
OK

AT
OK
AT+NAMEmyBT
OKsetname
AT+BAUD4
OK9600
AT+PIN1234
OKsetPIN

AT+RMAAD
OK
AT+ROLE=1
OK
AT+RESET
OK

<<<=====================================================================>>>
Put HC-05 back into AT mode, press and hold the HC-05 button, and send 'OK' through Serial Monitor
   MAKE SURE THE HC-05's BUTTON IS PRESSED UNTIL TOLD OTHERWISE!!!
<<<=====================================================================>>>

AT
OK
AT+CMODE=0
OK
AT+INQM=0,5,9
OK
AT+INIT
OK
AT+INQ
+INQ:A402:B9:29F454,A010C,7FFF
+INQ:98D3:31:FB8013,1F00,7FFF

<<<=====================================================================>>>
HC-05's button no longer needs to be pressed

Put HC-05 back into AT mode, press and hold the HC-05 button, and send 'OK' through Serial Monitor
   MAKE SURE THE HC-05's BUTTON IS PRESSED UNTIL TOLD OTHERWISE!!!
<<<=====================================================================>>>

AT
OK
AT+INIT
OK
AT+RNAME?A402,B9,29F454
DESKTOP-3MD8VVS
AT+RNAME?98D3,31,FB8013
myBT
   HC-06 found at address: 98D3:31:FB8013
AT+PAIR=98D3,31,FB8013,20
OK
AT+BIND=98D3,31,FB8013
OK
AT+CMODE=1
OK
AT+LINK=98D3,31,FB8013
OK

<<<=====================================================================>>>
HC-05's button no longer needs to be pressed - Modules shoud be paired! :D
<<<=====================================================================>>>
