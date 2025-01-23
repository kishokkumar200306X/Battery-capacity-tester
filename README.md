# Battery Capacity Tester

## Overview  
The Battery Capacity Tester is a digital device that allows users to measure the capacity of various battery types. It is designed for household use and works with nearly all batteries rated below 5V. The device features an OLED display for real-time monitoring and provides audible signals to indicate the start and end of the testing process.

## Main Functionality  
1. **Setup**  
   - Connect the battery to the battery terminal.  
   - Set the desired discharge current using the push buttons.  

2. **Start Test**  
   - Long-press the "UP" button until a beep is heard to start the test process.  

3. **Testing Process**  
   - The battery will discharge until its voltage reaches the low-level threshold of 3.2V.  
   - All relevant parameters (voltage, discharge current, and capacity) are displayed on the OLED during the process.  

4. **End of Test**  
   - Two prolonged beeps indicate the end of the test when the battery voltage drops to 3.2V.


![Battrey capacity tester](image/implemeted.png)   

## Features  
- Supports a wide range of battery types:  
  - AA/AAA/NiMh/NiCd  
  - 18650 Li-ion  
  - Li-Polymer  
  - LiFePO4  
- Adjustable discharge current via push buttons.  
- Functions as an electronic load.  
- Real-time monitoring on a 0.96’’ I2C OLED display.  
- Audible alerts for the start and end of the test process.  

## Specifications  
- **Type**: Household battery tester  
- **Style**: Digital  
- **Power Supply**: 7–9V DC  
- **Display**: 0.96’’ I2C OLED  
- **Monitoring Parameters**:  
  - Battery voltage  
  - Discharge current  
  - Battery capacity  
- **Controls**:  
  - Three push buttons:  
    - One for resetting the Arduino  
    - Two for adjusting discharge current  
- **Buzzer**: Provides audible signals for key events.  
- **Operating Temperature**: 0°C – 40°C  

## How to Use  
1. Power the tester using a 7–9V DC power source.  
2. Connect the battery securely to the terminal.  
3. Use the push buttons to set the desired discharge current.  
4. Long-press the "UP" button to start the test.  
5. Monitor the OLED display for real-time updates on voltage, current, and capacity.  
6. Wait until the battery voltage reaches 3.2V, at which point the device will emit two prolonged beeps.  

## Notes  
- Ensure the power supply is stable and within the specified range (7–9V).  
- Operate the tester within the recommended temperature range (0°C – 40°C).  
- Handle batteries carefully to prevent short circuits or damage.  



---

**Enjoy accurate and efficient battery capacity testing with this versatile device!**

