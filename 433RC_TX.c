//  This program sends  bits to a simple 433MHz OOK ASK transmitter module
// Used to replace a AC114-01B remote control on Raspberry Pi
// Button UP / STOP  /  DOWN  only
// bits for each command were scanned through RTL-SDR dongle + Universal Radio Hacker
//
// Requires wiringPi library
//
// call :  433RC_TX up/down/stop [GPIO_port] 
//   default GPIO port is 0 (GPIO17)

// compile:  gcc -Wall -o 433RC_TX 433RC_TX.c -lwiringPi


// phfu  -  June 2019  -  Free to use  :)

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <string.h>

#define TIME 202     // bit duration in µs


// this are the bits from URH     Please replace with your codes  !!!
const char Up []    = "11111111111111111111111111100011101000111010001000100011101110100010001....";
const char Down []  = "1111111111111111111111111110001110100011101000100010001110111010001000111....";
const char Stop []  = "111111111111111111111111111000111010001110100010001000111011101000100011101000....";

void sendData(int GPIO_pin,const char * data, long int size)     // sends the 433 MHz message  by togling the Data pin of the TX module
{
  for(long int i = 0; i < size; ++i)
  {
    if (data[i]=='0')
    {
     digitalWrite(GPIO_pin, LOW);
     delayMicroseconds(TIME);
    }
    else if (data[i]=='1')
    {
     digitalWrite(GPIO_pin, HIGH);
     delayMicroseconds(TIME);
    }
  }
  digitalWrite(GPIO_pin, LOW);
  delayMicroseconds(TIME*20);
}



int main(int argc, char *argv[])
{   int GPIO_pin = 0;
    if (argc < 2)
    {
        printf("Usage : %s up/down/stop [GPIO_Pin] \n",argv[0]);
        printf("GPIO is optional, default = 0 (GPIO17) \n");
        exit(1);
    }
    if (argc == 3)
     GPIO_pin = atoi (argv[2]);

    if (wiringPiSetup () == -1)
    {  printf("Could not access GPIO\n");
        return 1 ;
    }

    pinMode (GPIO_pin, OUTPUT) ;

    if (strcmp(argv[1],"up")==0)
    {
        printf("send up\n");
	sendData(GPIO_pin,Up, sizeof(Up));
    }
    else if (strcmp(argv[1],"down")==0)
    {
        printf("send down\n");
	sendData(GPIO_pin,Down, sizeof(Down));
    }
    else if (strcmp(argv[1],"stop")==0)
    {
        printf("send stop\n");
	sendData(GPIO_pin,Stop, sizeof(Stop));
    }
    else
        printf("Bad command ! use  up, down or stop\n");

    return 0;
}





