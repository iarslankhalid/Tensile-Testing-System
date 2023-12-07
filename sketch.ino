/*
  => Keypad pins -- 0-7
  => Motor Driver control pins -- 8-13
      (
        DIR =   8
        STEP =  9
        MS1 =   10
        MS2 =   11
        MS3 =   12
        EN =    13
        RESET = 14
        SLEEP = 15
      )
  
  => LCD DISPLAY PINS --
      (
        RS =    53
        E =     52
        D0 - D1 = 22 - 29
      )
  
  => LOAD CELL PINS --
      (
        DT =  33
        SCK = 34
      )

  PUSH BUTTON FOR TARE -- 37
*/


// Libraries
#include <Keypad.h>
#include <HX711.h>
#include <LiquidCrystal.h>


// KEYPAD initialization
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
{'1','2','3', 'A'},
{'4','5','6', 'B'},
{'7','8','9', 'C'},
{'*','0','#', 'D'}
};
byte rowPins[ROWS] = {7, 6, 5, 4}; 
byte colPins[COLS] = {3, 2, 1, 0}; 
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// LCD initialization
const int rs = 53, en = 52, d4 = 26, d5 = 27, d6 = 28, d7 = 29;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Motor Driver initialization
const int STEP =  9;
const int DIR =   8;
const int MS1 =   10;
const int MS2 =   11;
const int MS3 =   12;
const int EN =    13;
const int RESET = 14;
const int SLEEP = 15;

//Load Cell initialization
HX711 scale;
const int DT = 33;
const int CK =34;

// variables
String msg;
boolean inputInProgress;
const float calibrationFactor = 0.42;
float old_load = 0.0;
float old_force = 0.0;


void setup() {

  // Motor driver setup
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(SLEEP, OUTPUT);

  // LCD setup
  lcd.begin(16, 2);
  lcd_center(0, "Press '*' to");
  lcd_center(1, "start");

  // Load cell setup
  scale.begin(DT, CK);
  scale.set_scale(calibrationFactor);
}

void loop() {
  char key = kpd.getKey();
  
  if(key)
  {
    if (key == '*')
    { 
      boolean AdjustmentRequested = false;
      boolean ManualModeRequested = false;
      boolean CompressModeRequested = false;
      boolean NormalModeRequested = false;

      lcd.clear();
      lcd_center(0, "Adjust Motors");
      AdjustmentRequested = check_request(3000);
      if(AdjustmentRequested)
        AdjustMotors();

      lcd.clear();     
      lcd_center(0, "Normal Mode");
      NormalModeRequested = check_request(3000);
      if(NormalModeRequested)
        NormalMode();


      lcd.clear();
      lcd_center(0, "Manual Mode");
      ManualModeRequested = check_request(3000);
      if(ManualModeRequested)
        ManualMode();
      

      lcd.clear();
      lcd_center(0, "Compress Mode");
      CompressModeRequested = check_request(3000);
      if(CompressModeRequested)
        CompressMode();
    }

    // if user does not press * to start the program
    else if(key != '*')
    {
      lcd.clear();
      lcd_center(0, "ERROR!");
      lcd_center(1, "INVALID INPUT");
      delay(2000);
    }

    asm volatile("jmp 0");
  }
} 

// Program ends here...


// Modes
void AdjustMotors()
{
  lcd.clear();
  lcd_center(0, "Use A and B to");
  lcd_center(1, "Adjust Motors");
  
  char key = kpd.getKey();
  while(key != '#')
  {

    key = kpd.getKey();

    if (key == 'A')
    {
      motor_set(1, 1);   // motor set in forward direction
      while (!(key == 'B' || key == '#'))
      {
        key = kpd.getKey();
        motor_motion(400);
      }
    }
        
    else if (key == 'B')
    {
      motor_set(1, -1);   // motor set in reverse direction
      while (!(key == 'A' || key == '#'))
      {
        key = kpd.getKey();
        motor_motion(400);
      }
    } 
  }

  motor_set(0, 1);    // disabling the motor
  lcd_center(0, "Position is");
  lcd_center(1, "set...");
  delay(1000);
}


void NormalMode()
{ 
  scale.tare();
  float load , load_force = get_load_values();

  lcd.clear();
  lcd_center(0, "Normal Mode");
  lcd_center(1, "activated");
  delay(1500);

  lcd.clear();
  lcd_center(0, "Enter Force (N)");
  String force = readKeypad();

  lcd.clear();
  lcd_center(0, "Enter Iterations");
  String itr = readKeypad();
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Force (N):");
  lcd.setCursor(16-force.length(), 0);
  lcd.print(force);
  lcd.setCursor(0,1);
  lcd.print("Iterations:");
  lcd.setCursor(16-itr.length(), 1);
  lcd.print(itr);
  delay(1000);

  lcd.clear();
  lcd.noBlink();
  for (int i=1 ; i <= itr.toInt() ; i++)
  {
    scale.tare();
    motor_set(1, 1); // set the motor to forward direction
    while (true)
    {
      float load , load_force = get_load_values();
      motor_motion(400);
      show_values();

      if (load_force >= force.toInt())
        break;
    }
    
    lcd.clear();
    lcd_center(0, "Required Force");
    lcd_center(1, "Reached...");
    delay(500);
    lcd.clear();

    motor_set(1, -1); // setting the motor to reverse direction
    while (true)
    {
      float load , load_force = get_load_values();
      motor_motion(400);
      show_values();

      if (load <= 0 && load_force <= 0)
        break;
    }

    lcd.clear();
    lcd_center(0, "Zero Force");
    lcd_center(1, "Reached...");
    delay(500);
    lcd.clear();
  }

  lcd.clear();
  lcd_center(0, "Task Completed");
  lcd_center(1, "Successfully.");
  delay(1000);

  while(1)
  {
    char key = kpd.getKey();
    if(key)
      break;  
  }
} 


void ManualMode()
{
  digitalWrite(EN, LOW);
  digitalWrite(SLEEP, HIGH);
  digitalWrite(RESET, HIGH);

  lcd.clear();
  lcd_center(0, "Manual Mode");
  lcd_center(1, "activated");
  delay(1500);
  lcd_center(0, "Use A and B to");
  lcd_center(1, "Operate Manually");
  delay(1500);

  while(1)
  {
    char key = kpd.getKey();
    if(key)
    {
      if(key == 'A')
      {
        while(!(key == '0'))
        {
          key = kpd.getKey();
          digitalWrite(DIR, HIGH);
          
        }
      }

      else if(key == 'B')
      {
        while(!(key == '0'))
        {
          key = kpd.getKey();
          digitalWrite(DIR, LOW);
          
        } 
      }

      else
      {
        lcd.clear();
        lcd_center(0, "INVALID INPUT");
      }  
    }
  }
}


void CompressMode()
{
  lcd.clear();
  lcd_center(0, "Compress Mode");
  lcd_center(1, "activated");
  delay(3000);
  while(1){}
}


//functions for motor motion
void motor_set(int enable, int direction)
{
  if (enable == 0)
  {
    digitalWrite(SLEEP, LOW);
    digitalWrite(en, HIGH);
    return;
  }

  digitalWrite(SLEEP, HIGH);
  digitalWrite(EN, LOW);

  digitalWrite(RESET, LOW);delay(100);
  digitalWrite(RESET, HIGH);

  if (direction == -1)
    digitalWrite(DIR, LOW);
  else
    digitalWrite(DIR, HIGH);
}

void motor_motion(int pulse)
{
  digitalWrite(STEP, HIGH);delayMicroseconds(pulse);
  digitalWrite(STEP, LOW);delayMicroseconds(pulse);
}


//funtion to display something in the center
void lcd_center(int row, String text)
{
  int text_len = text.length();
  int num_spaces = max(0, (16 - text_len) / 2); 
  String centered_text = "";
  for (int i = 0; i < num_spaces; i++) {
    centered_text += " "; 
  }
  centered_text += text; 
  lcd.setCursor(0, row); 
  lcd.print(centered_text); 
}

// funtion to read from the keypad until # is pressed
String readKeypad()
{
  int i = 0;
  String msg = "";
  while(1)
  {
    char key = kpd.getKey();
    lcd.setCursor(i,1);
    lcd.blink();
    
    if(key)
    {
      if(key == '#')
      break;
    
      lcd.print(key);
      msg += key;
      i++;
      delay(300);
    }
  }

  if(msg.length() == 0)
    return String(0);

  else
    return msg;
}


// function to check request for menu selection
boolean check_request(int time_ms)
{
  unsigned long start_time = millis();

  while (millis() - start_time < time_ms)
  {
    char key = kpd.getKey();
    if (key == 'C')
      break;

    else if (key == 'D')
      return true;
  }
  return false;
}

// To print force and weight values:
void show_values() {
  lcd.setCursor(0, 0);
  lcd.print("Weight:");
  lcd.setCursor(0, 1);
  lcd.print("Force:");

  float new_load = scale.get_units(); 
  float new_force = new_load / 1000.0 * 9.8;

  // Check if the values have changed beyond the specified thresholds
  if (abs(new_load - old_load) > 5 || abs(new_force - old_force) > 0.01) {
    // If values have changed, update the display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Weight:");
    lcd.setCursor(0, 1);
    lcd.print("Force:");
    
    lcd.setCursor(9, 0);
    lcd.print(new_load);

    lcd.setCursor(9, 1);
    lcd.print(new_force);

    // Update old values for the next comparison
    old_load = new_load;
    old_force = new_force;
    return;
  }
}

// to get values from the load cell
float get_load_values()
{
  float load, force = 0;
  load = scale.get_units(); 
  force = load/1000 * 9.8 ;
  return load,force;
}