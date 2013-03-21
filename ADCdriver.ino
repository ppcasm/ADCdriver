/*
W555AD808 ADC Driver Library by: ppcasm
*/

#define PWR_FALL 0x0//Power Down and Falling Wakeup Setting
#define D_OR_A_IN_SEL 0x1 //Digital or Analog Input Selection
#define WAKEUP_SETTING 0x2 //Wakeup Pin Setting
#define IN_FUNC_SEL 0x3 //Internal Function Selection 
#define READ_ADC 0x4 //Read the Analog to Digital Conversion
#define READ_DI 0x5 //Read the Digital Input
#define PWR_RISE 0x7//Power Down and Rising Wakeup Setting

//ADC Sel Bits
#define SEL_A7 0x7
#define SEL_A6 0x6
#define SEL_A5 0x5
#define SEL_A4 0x4
#define SEL_A3 0x3
#define SEL_A2 0x2
#define SEL_A1 0x1
#define SEL_A0 0x0 

int LED = 13; //LED
int SCLKZ = 21; //SCLK
int DATAZ = 20; //DATA
int ENABLEZ = 37;  //EN

void send_insn(uint8_t insn);
uint8_t get_byte(void);
uint8_t read_ADC(uint8_t SELAX);

// the setup routine runs once when you press reset:
void setup() 
{                
  Serial.begin(115200); //Enable Serial (115200 baud)
  pinMode(LED, OUTPUT);  //LED Debug
  pinMode(SCLKZ, OUTPUT); //SCLK
  pinMode(DATAZ, OUTPUT); //DATA
  pinMode(ENABLEZ, OUTPUT);  //EN
  
  //Set EN HIGH
  digitalWrite(ENABLEZ, HIGH);
  
  //Set SCLK LOW
  digitalWrite(SCLKZ, LOW);
}

void loop() 
{
  Serial.print(read_ADC(SEL_A0), HEX);
  delay(50);
  Serial.print("\n");
  delay(50);
}

void send_insn(uint8_t insn)
{
  //Send 3 bit ADC opcode.
  for(int i=0;i<3;i++)
  {
    digitalWrite(SCLKZ, HIGH);
    digitalWrite(DATAZ, (insn&(1<<2-i)) ? HIGH : LOW);
    digitalWrite(SCLKZ, LOW); 
  }
  
  return;
}

uint8_t get_byte()
{
  uint8_t bytez = 0;
   
  //Now switch to input mode on the data pin.
  pinMode(DATAZ, INPUT);
  
  //Read next 8 bits, make summation, return answer.
  for(int i=0;i<8;i++)
  {
    digitalWrite(SCLKZ, HIGH);
    digitalWrite(SCLKZ, LOW);
    digitalRead(DATAZ) ? bytez+=(0xff&(1<<i)):bytez+=0; 
  }
  
  pinMode(DATAZ, OUTPUT);
  
  return bytez;
}

uint8_t read_ADC(uint8_t SELAX)
{
  
   int ADC_VAL = 0;
   
   //Set DATA pin to OUTPUT.
   pinMode(DATAZ, OUTPUT);
   
   //CMD Enable.
   digitalWrite(ENABLEZ, LOW); 
  
  //Now, we need to clock in the instruction (READ_ADC).
  send_insn(READ_ADC);
  
  //This is a "hack" that sends the 3 SEL bits (for READ_ADC insn.
  send_insn(SELAX); 
  
  //Clock X1.
  digitalWrite(SCLKZ, HIGH);
  digitalWrite(DATAZ, LOW);
  digitalWrite(SCLKZ, LOW);
  
  //Clock X2.
  digitalWrite(SCLKZ, HIGH);
  digitalWrite(DATAZ, LOW);
  digitalWrite(SCLKZ, LOW);
  
  //Get ADC value.
  ADC_VAL = get_byte();
  
  //Last clock.
  digitalWrite(DATAZ, HIGH);
  
  //CMD Done.
  digitalWrite(ENABLEZ, HIGH);
  
  return ADC_VAL;
   
}
