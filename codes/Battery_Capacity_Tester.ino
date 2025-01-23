#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>





    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // ARDUINO BATTERY CAPACITY TESTER
    
    
    #include<JC_Button.h>
    #include <Adafruit_SSD1306.h>
    
    #define DEBUG 0 // Binary Treatment 0=Nothing, 1=Early, 2=Running Voltages, 4=Finish
    #define SCREEN_WIDTH 128 // OLED display width, in pixels
    #define SCREEN_HEIGHT 64 // OLED display height, in pixels
    
    // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
    #define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    
    void timerInterrupt();
    void Display_UP_DOWN();//Debugging support
    void Print_DEBUG_4();//Debugging support
    
    const float Low_BAT_level = 2.50;// Threshold for stopping test
    //Desired Current steps with a 3R load (R7)
    int Current [] = {0,50,100,200,300,400,500,600,700,800,900,1000};
    int PWM [] = {0, 2, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    int Array_Size;// calculated during setup
    const byte PWM_Pin = 10;
    const byte Buzzer = 9;
    const int BAT_Pin = A0;
    const int Vref_Pin = A1;//Pin to which Band Gap Reference is attached
    int Current_Value = 0; // Selected Current value for test
    int PWM_Value = 0;// Value of PWM during test
    int PWM_Index = 0;// Index into PWM array during test
    unsigned long Capacity;
    float Capacity_f;
    int ADC_Value = 0;
    float Vref_Voltage = 1.215; // LM385BLP-1.2 Band Gap Reference voltage
    float Vcc = 4.94; // Voltage of Arduino 5V pin ( Measured during setup using Band Gap Reference )
    float BAT_Voltage = 0;
    float Resistance = 1.0; // Value of R3 Power Resistor
    float sample = 0;
    byte Hour = 0, Minute = 0, Second = 0;
    bool calc = false, Done = false, Report_Info = true;
    Button UP_Button(2, 25, false, true);
    Button Down_Button(3, 25, false, true);
    
    // string values for reporting intermediate information
    const int VAL_MAX = 10;
    char val_0[VAL_MAX]={""};
    char val_2[VAL_MAX]={""};
    
    void setup () {//Setup
    
    Serial.begin(38400);
    pinMode(PWM_Pin, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    analogWrite(PWM_Pin, PWM_Value);
    UP_Button.begin();
    Down_Button.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(12,25);
    display.print("BC TESTER");
    display.display();
    delay(3000);
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(2,15);
    display.print("Adj Curr:");
    display.setCursor(2,40);
    display.print("UP/Down:");
    display.print("0");
    display.setCursor(2,55);
    display.setTextSize(1);
    
    #if (DEBUG == 1 || DEBUG == 5)
    Serial.println("\nStart of calculations");
    #endif
    
    Array_Size = sizeof(Current)/2;
    //Read voltages of Band Gap Reference and use it to calculate actual Vcc
    float fTemp=0.0;
    for(int i=0;i< 100;i++)
    {
    fTemp=fTemp+analogRead(Vref_Pin); //read the Band Gap Reference voltage
    delay (2);
    }
    fTemp=fTemp/100.0;
    Vcc = Vref_Voltage * 1024.0 / fTemp;
    
    // Convert desired current levels to PWM using actual Vcc
    // Convert PWM values back to actual current levels
    // While measuring actual current I discovered that the actual draw is
    // (PWM + 1)/256 rather than PWM/255 as indicated in Arduino documentation
    int iTemp;
    for (int i=0;i<Array_Size;i++)
    {
    iTemp = int( Resistance * Current[i] * 256.0 / Vcc / 1000.0 - 1.0 + 0.5 ); // desired current to nearest PWM
    iTemp = min(iTemp,255);
    iTemp = max(iTemp,0);
    Current[i] = int( (iTemp+1) * Vcc / 256.0 / Resistance * 1000.0); // actual current for PWM
    PWM[i] = iTemp;//Save PWM in array
    }
    
    //Include Threshold and Vcc in startup display
    dtostrf(Low_BAT_level, 5, 3, val_0);
    dtostrf(Vcc, 5, 3, val_2);
    
    display.print("Thr=");
    display.print(val_0);
    display.print("v, Vcc=");
    display.print(val_2);
    
    display.display();
    display.setTextSize(2);
    
    }//Setup
    
    //************************* End of Setup function *******************************
    
    void loop() {
    
    if (Report_Info)
    {//Report_Info
    Serial.flush();
    #if (DEBUG == 1 || DEBUG == 5)
    // Serial.println("Measured Vcc Voltage: " + String(Vcc) + " volts");
    Serial.print("Threshold: ");
    Serial.print(Low_BAT_level,3);
    Serial.println(" volts");
    Serial.print("R3 Resistance: ");
    Serial.print(Resistance,3);
    Serial.println(" ohms");
    Serial.print("Measured Vcc Voltage: ");
    Serial.print(Vcc,4);
    Serial.println(" volts");
    sample = 0.0;
    
    Serial.println("Index Actual(mA) PWM");
    for (int i=0;i<Array_Size;i++)
    {
    // Serial.println( "["+String(i)+"]="+String(Current[i])+" mA PWM["+String(i)+"]="+String(PWM[i]) );
    Serial.print("[");
    Serial.print(i);
    Serial.print("]");
    Serial.print(" ");
    Serial.print(Current[i],DEC);
    Serial.print(" ");
    Serial.print(PWM[i],DEC);
    Serial.println(" ");
    }
    #endif
    Report_Info = false;
    }//Report_Info
    
    UP_Button.read();
    Down_Button.read();
    if (UP_Button.wasReleased() && PWM_Index < (Array_Size-1) && calc == false)
    {
    PWM_Value = PWM[++PWM_Index];
    analogWrite(PWM_Pin,PWM_Value);
    
    Display_UP_DOWN();
    }
    
    if (Down_Button.wasReleased() && PWM_Index > 0 && calc == false)
    {
    PWM_Value = PWM[--PWM_Index];
    analogWrite(PWM_Pin,PWM_Value);
    
    Display_UP_DOWN();
    }
    
    if (UP_Button.pressedFor (1000) && calc == false)
    {
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    display.clearDisplay();
    timerInterrupt();
    }
    }
    
    //************************* End of Loop function *******************************
    
    void timerInterrupt(){
    calc = true;
    while (Done == false) {//(Done == false)
    Second ++;
    if (Second == 60) {
    Second = 0;
    Minute ++;
    }
    if (Minute == 60) {
    Minute = 0;
    Hour ++;
    }
    
    //************ Measuring Battery Voltage ***********
    
    for(int i=0;i< 100;i++)
    {
    sample=sample+analogRead(BAT_Pin); //read the Battery voltage
    delay (2);
    }
    sample=sample/100;
    BAT_Voltage = sample * Vcc / 1024.0;
    //*********************************************
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20,5);
    // display.print(String(Hour) + ":" + String(Minute) + ":" + String(Second));
    display.print(Hour);
    display.print(":");
    display.print(Minute);
    display.print(":");
    display.print(Second);
    
    display.setTextSize(1);
    display.setCursor(0,25);
    display.print("Disch Curr: ");
    // display.print(String(Current[PWM_Index])+"mA");
    display.print(Current_Value);
    display.print("mA");
    
    display.setCursor(2,40);
    // display.print("Bat Volt:" + String(BAT_Voltage)+"V" );
    display.print("Bat Volt:");
    display.print(BAT_Voltage,3);
    display.print("V");
    
    // When total seconds is greater than 32767 the statement below did not work until the byte values
    // Hour, Minute and Second were cast to an unsigned long. Apparently the compiler cast the byte
    // values to an "int" first which cannot represent 32768 correctly
    Capacity = ((unsigned long)Hour * 3600) + ((unsigned long)Minute * 60) + (unsigned long)Second;
    Capacity_f = ((float)Capacity * Current_Value) / 3600.0;
    
    display.setCursor(2, 55);
    // display.print("Capacity:" + String(Capacity) + "mAh");
    display.print("Capacity:");
    display.print(Capacity_f,1);
    display.print("mAh");
    display.display();
    
    #if (DEBUG == 4 || DEBUG == 2)
    Print_DEBUG_4();
    #endif
    
    if (BAT_Voltage < Low_BAT_level)
    {//BAT_Voltage < Low_BAT_level
    
    #if (DEBUG == 4 || DEBUG == 5)
    Serial.println("\nCurrent_Value PWM_Value");
    Serial.print(Current_Value);
    Serial.print(" ");
    Serial.println(PWM_Value);
    Serial.println("\nHour Minute Second PWM_Index");
    Serial.print(Hour);
    Serial.print(" ");
    Serial.print(Minute);
    Serial.print(" ");
    Serial.print(Second);
    Serial.print(" ");
    Serial.println(PWM_Index);
    #endif
    
    // When total seconds is greater than 32767 the statement below did not work until the byte values
    // Hour, Minute and Second were cast to an unsigned long. Apparently the compiler cast the byte
    // values to an "int" first which cannot represent 32768 correctly
    Capacity = ((unsigned long)Hour * 3600) + ((unsigned long)Minute * 60) + (unsigned long)Second;
    
    #if (DEBUG == 4 || DEBUG == 5)
    Serial.println("Capacity HMS");
    Serial.println(Capacity);
    #endif
    
    Capacity_f = ((float)Capacity * Current_Value) / 3600.0;
    
    #if (DEBUG == 4 || DEBUG == 5)
    Serial.println("Capacity HMS*PWM");
    Serial.println(Capacity_f,1);
    #endif
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(2,15);
    display.print("Capacity:");
    display.setCursor(2,40);
    // display.print(String(Capacity) + "mAh");
    display.print(Capacity_f,1);
    display.print("mAh");
    display.display();
    Done = true;
    PWM_Value = 0;
    analogWrite(PWM_Pin, PWM_Value);
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    delay(100);
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    delay(100);
    }//BAT_Voltage < Low_BAT_level
    delay(1000);
    }//(Done == false)
    
    }// timerInterrupt
    
    void Display_UP_DOWN()
    {//Display_UP_DOWN()
    Current_Value = Current[PWM_Index];
    
    display.clearDisplay();
    display.setCursor(2,25);
    display.print("Curr:");
    display.print(Current_Value);
    display.print("mA ");
    display.setCursor(2,40);
    display.print("PWM=");
    display.print(PWM_Value);
    display.display();
    }//Display_UP_DOWN()
    
    void Print_DEBUG_4()
    {//Print_DEBUG_4()
    Serial.print(Hour);
    Serial.print(":");
    Serial.print(Minute);
    Serial.print(":");
    Serial.print(Second);
    Serial.print(" ");
    Serial.print(BAT_Voltage,3);
    Serial.print("v ");
    Serial.print(Capacity_f,1);
    Serial.println("mAh");
    }//Print_DEBUG_4()
