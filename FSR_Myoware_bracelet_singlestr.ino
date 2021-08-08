/*
 * Final code for FSR Myoware sensor bracelet
 * This code reads 2 myoware sensors and 8 FSR sensors and records the values using 
 * Serial Interface. 
 * Author : Ankur Deshmukh
*/

#include<ADC.h>
#include<ADC_util.h>
#include <TimerOne.h>

/*
 * Pin Declarations for ADC0 and ADC1
 * ADC0: A9 - A6 : FSR sensors
 * ADC0: A5 : Myoware sensor
 * ADC1: A20 - A17 : FSR sensors
 * ADC1: A16 : Myoware sensor
 */

uint8_t adc0_pins[] = {A9,A8,A7,A6,A5};
uint8_t adc1_pins[] = {A20,A19,A18,A17,A16};


ADC *adc = new ADC();           //ADC object for initialisation

void setup()
{
/*
 * Declare all ADC pins described above as Input
 */
  pinMode(LED_BUILTIN, OUTPUT);

  for(int i = 0; i < 5; i++)
  {
    pinMode(adc0_pins[i], INPUT);
  }

  for(int i = 0; i < 5; i++)
  {
    pinMode(adc1_pins[i], INPUT);
  }
/*
 * Initialise timer 1 interrupt for maintaining sampling rate of 1KHz
 */
  Timer1.initialize(1000);
  Timer1.attachInterrupt(ISR);
                  
  Serial.begin(9600);

/*
 * Setting ADC configuration parameters
 * ADC averaging to 4 
 * ADC resolution to 10
 * Conversion speed to maximum possible
 * Both ADCs have a similar configuration
 */
 
  adc->adc0->setAveraging(4);
  adc->adc0->setResolution(10);
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

  adc->adc1->setAveraging(4);
  adc->adc1->setResolution(10);
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS);
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  
/*
 * Initiating the conversion on the first channel of both ADCs
 */
 
  adc->adc0->startSingleRead(adc0_pins[0]);
  adc->adc1->startSingleRead(adc1_pins[0]);

  Serial.println("End Setup");
}

/*
 * Variable explainations
 * adc0_val[5] : Storing ADC0 values (Volatile as the variable is also used in ISR)
 * adc1_val[5] : Storing ADC1 values
 * ctr0 and ctr1 : counter variables for keeping track of which channel is being converted
 * flag0 and flag1 : flags to indiccate that all ADC channels has been converted
 * flag : To transfer control back to main flow from ISR
 */

volatile int adc0_val[5];
volatile int adc1_val[5];
int ctr0 = 0;
int ctr1 = 0;
int flag0 = 0;
int flag1 = 0;
volatile int flag = 0;

void ISR(void)
{
  adc_convert();
}

/*
 * Function for conversion at all the 10 channels
 */
void adc_convert()
{
  flag = 0;
  
  while(flag == 0)
  {
    if(adc->adc0->isComplete())
    {
      adc0_val[ctr0] = adc->adc0->readSingle();
      ctr0++;
      if(ctr0 > 4)
      {
        flag0 = 1;
        ctr0 = 0;
      }
      adc->adc0->startSingleRead(adc0_pins[ctr0]);
    }
    
    if(adc->adc1->isComplete())
    {
      adc1_val[ctr1] = adc->adc1->readSingle();
      ctr1++;
      if(ctr1 > 4)
      {
        flag1 = 1;
        ctr1 = 0;
      }
      adc->adc1->startSingleRead(adc1_pins[ctr1]);
    }

    if(flag0 == 1 && flag1 == 1)
    {
      flag0 = 0;
      flag1 = 0;
      flag = 1;
    }
  }
}
String data = "";
/*
 * Loop to record all the sensor values using Serial interfafce
 */
void loop()                     
{
  if(flag == 1)
  {
    data+="S1 = ";
    data+=(adc0_val[0]*3.3/adc->adc0->getMaxValue());
    data+="S2 = ";
    data+=(adc0_val[1]*3.3/adc->adc0->getMaxValue());
    data+="S3 = ";
    data+=(adc0_val[2]*3.3/adc->adc0->getMaxValue());
    data+="S4 = ";
    data+=(adc0_val[3]*3.3/adc->adc0->getMaxValue());
    data+="S5 = ";
    data+=(adc0_val[4]*3.3/adc->adc0->getMaxValue());
    data+="S6 = ";
    data+=(adc1_val[0]*3.3/adc->adc0->getMaxValue());
    data+="S7 = ";
    data+=(adc1_val[1]*3.3/adc->adc0->getMaxValue());
    data+="S8 = ";
    data+=(adc1_val[2]*3.3/adc->adc0->getMaxValue());
    data+="S9 = ";
    data+=(adc1_val[3]*3.3/adc->adc0->getMaxValue());
    data+="S10 = ";
    data+=(adc1_val[4]*3.3/adc->adc0->getMaxValue());
    Serial.print(data);
    flag = 0;
  }

  if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
    Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
  }
  
  if(adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
    Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
  }
}
