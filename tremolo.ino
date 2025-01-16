#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <EncoderButton.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET    -1 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SCREEN_ADDRESS 0x3C   ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define LDR_PIN_1 9
#define LDR_PIN_2 10

#define LED_PIN_1 5
#define LED_PIN_2 6

#define LED_PIN_ON 14

#define EXPRESSION_DETECT_PIN 15
#define EXPRESSION_VALUE_PIN 0

#define MAX_SPEED 64
#define MIN_SPEED -32

#define DEPTH_SPEED 5

#define MODE_COUNT 8

const uint8_t SINE_WAVE[] PROGMEM = {
  0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95, 0x98, 0x9b, 0x9e, 0xa2, 0xa5, 0xa7, 0xaa, 0xad, 0xb0, 0xb3, 0xb6, 0xb9, 0xbc, 0xbe, 0xc1, 0xc4, 0xc6, 0xc9, 0xcb, 0xce, 0xd0, 0xd3, 0xd5, 0xd7,
  0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xeb, 0xed, 0xee, 0xf0, 0xf1, 0xf3, 0xf4, 0xf5, 0xf6, 0xf8, 0xf9, 0xfa, 0xfa, 0xfb, 0xfc, 0xfd, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfc, 0xfb, 0xfa, 0xfa, 0xf9, 0xf8, 0xf6, 0xf5, 0xf4, 0xf3, 0xf1, 0xf0, 0xee, 0xed, 0xeb, 0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc,
  0xda, 0xd7, 0xd5, 0xd3, 0xd0, 0xce, 0xcb, 0xc9, 0xc6, 0xc4, 0xc1, 0xbe, 0xbc, 0xb9, 0xb6, 0xb3, 0xb0, 0xad, 0xaa, 0xa7, 0xa5, 0xa2, 0x9e, 0x9b, 0x98, 0x95, 0x92, 0x8f, 0x8c, 0x89, 0x86, 0x83,
  0x80, 0x7c, 0x79, 0x76, 0x73, 0x70, 0x6d, 0x6a, 0x67, 0x64, 0x61, 0x5d, 0x5a, 0x58, 0x55, 0x52, 0x4f, 0x4c, 0x49, 0x46, 0x43, 0x41, 0x3e, 0x3b, 0x39, 0x36, 0x34, 0x31, 0x2f, 0x2c, 0x2a, 0x28,
  0x25, 0x23, 0x21, 0x1f, 0x1d, 0x1b, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x0f, 0x0e, 0x0c, 0x0b, 0x0a, 0x09, 0x07, 0x06, 0x05, 0x05, 0x04, 0x03, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x09, 0x0a, 0x0b, 0x0c, 0x0e, 0x0f, 0x11, 0x12, 0x14, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x23,
  0x25, 0x28, 0x2a, 0x2c, 0x2f, 0x31, 0x34, 0x36, 0x39, 0x3b, 0x3e, 0x41, 0x43, 0x46, 0x49, 0x4c, 0x4f, 0x52, 0x55, 0x58, 0x5a, 0x5d, 0x61, 0x64, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7c
};

const uint8_t TRIANGLE_WAVE[] PROGMEM = {
  0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40,
  0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 0x80,
  0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d, 0x8f, 0x91, 0x93, 0x95, 0x97, 0x99, 0x9b, 0x9d, 0x9f, 0xa1, 0xa3, 0xa5, 0xa7, 0xa9, 0xab, 0xad, 0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb9, 0xbb, 0xbd, 0xbf,
  0xc1, 0xc3, 0xc5, 0xc7, 0xc9, 0xcb, 0xcd, 0xcf, 0xd1, 0xd3, 0xd5, 0xd7, 0xd9, 0xdb, 0xdd, 0xdf, 0xe1, 0xe3, 0xe5, 0xe7, 0xe9, 0xeb, 0xed, 0xef, 0xf1, 0xf3, 0xf5, 0xf7, 0xf9, 0xfb, 0xfd, 0xff,
  0xfd, 0xfb, 0xf9, 0xf7, 0xf5, 0xf3, 0xf1, 0xef, 0xed, 0xeb, 0xe9, 0xe7, 0xe5, 0xe3, 0xe1, 0xdf, 0xdd, 0xdb, 0xd9, 0xd7, 0xd5, 0xd3, 0xd1, 0xcf, 0xcd, 0xcb, 0xc9, 0xc7, 0xc5, 0xc3, 0xc1, 0xbf,
  0xbd, 0xbb, 0xb9, 0xb7, 0xb5, 0xb3, 0xb1, 0xaf, 0xad, 0xab, 0xa9, 0xa7, 0xa5, 0xa3, 0xa1, 0x9f, 0x9d, 0x9b, 0x99, 0x97, 0x95, 0x93, 0x91, 0x8f, 0x8d, 0x8b, 0x89, 0x87, 0x85, 0x83, 0x81, 0x80,
  0x7e, 0x7c, 0x7a, 0x78, 0x76, 0x74, 0x72, 0x70, 0x6e, 0x6c, 0x6a, 0x68, 0x66, 0x64, 0x62, 0x60, 0x5e, 0x5c, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50, 0x4e, 0x4c, 0x4a, 0x48, 0x46, 0x44, 0x42, 0x40,
  0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e, 0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0e, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x00
};


uint8_t WAVE_INDEX = 0;
uint8_t LDR1_VALUE = 0;
uint8_t LDR2_VALUE = 0;

bool enabled = false;

float R;
unsigned long previous_millis = 0;
long NON_BLOCKING_INTERVAL = 0;  // interval at which to blink (milliseconds)
long FADE_SPEED = 1;

unsigned int MODE = 0;
int MIN_BRIGHTNESS = 0;
int MAX_BRIGHTNESS = 255;

uint8_t PHASE = 128;

unsigned long prev_tap_millis = 0;
unsigned long new_tap_millis = 0;

bool expression_connected = false;
unsigned long expression_value = 0;


EncoderButton eb1(0, 1, 4);
EncoderButton eb2(7, 20, 21);
EncoderButton sw1(8);
EncoderButton sw2(16);


void setup_fast_pwm(){
  // Fast PWM AtMega32u4
  // Clear Timer/Counter Control Register A & B
  TCCR1A = 0;
  TCCR1B = 0;

  // Table 14-4. Waveform Generation Mode Bit Description. Page 133
  // Mode:7 - 0 1 1 1 - Fast PWM, 10-bit 0x03FF TOP TOP
  TCCR1B |= (0 << WGM13) | (1 << WGM12);  TCCR1A |= (1 << WGM11) | (1 << WGM10);

  // Table 14-5. Clock Select Bit Description. Page 134
  // 0 0 1 ..   /1 = 15.62 kHz PWM
  // 0 1 0 ..   /8 =  1.95 kHz
  // 0 1 1 ..  /64 =    244 Hz
  // 1 0 0 .. /256 =    61 Hz
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10); // 0 0 1 ... clkIO/1 (No prescaling)

  // Table 15-7. Compare Output Mode, Phase and Frequency Correct PWM Mode. Page 165
  // COM4A1..0 = 0b10
  //   Cleared on Compare Match when up-counting.
  //   Set on Compare Match when down-counting. 
  TCCR1A |= (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1);
  
  // Define Arduino PIN 9 as output. Page 75
  // (PCINT5/OC1A/#OC4B/ADC12) PB5 --> Port B Bit 5 --> Package Pin 29 --> Arduino Pin 9
  /* 
    OC.4B: Timer 4 Output Compare B. This pin can be used to generate a high-speed PWM signal from Timer 4
    module, complementary to OC.4B (PB5) signal. The pin has to be configured as an output (DDB5 set (one)) to
    serve this function. 
  */
  //DDRB |= (1 << DDB5);
  //DDRB |= (1 << DDB6);
  //PORTB |= (0<<PB5);
}



void sw1Pressed(EncoderButton& eb) {
  Serial.println("foo");
}

void sw2Pressed(EncoderButton& eb) {

  enabled = !enabled;

  if(!enabled){
    disableLDR();
  }
  else{
    enableLDR();
  }

  Serial.print("ENABLED: ");
  Serial.println(enabled);
  update_display();
}

void enableLDR(){
    setup_fast_pwm();
    DDRB |= (1 << DDB5);
    DDRB |= (1 << DDB6);
    enabled = true;
    digitalWrite(LED_PIN_ON, HIGH);
    display.invertDisplay(true);
}

void disableLDR(){
    digitalWrite(LDR_PIN_1,LOW);
    digitalWrite(LDR_PIN_2,LOW);
    DDRB |= (0 << DDB5);
    DDRB |= (0 << DDB6);
    enabled = false;
    digitalWrite(LED_PIN_ON, LOW);
    display.invertDisplay(false);
    
}

void onEb1Pressed(EncoderButton& eb) {

  if(++MODE >= MODE_COUNT)
    MODE = 0;
  Serial.print("MODE: ");
  Serial.println(MODE);
  update_display();
}

void onEb2Pressed(EncoderButton& eb) {
  Serial.println("foo");
/*
  unsigned long current_tap_millis = millis();
  
  if(MODE == 5) // Fading
  {
    if(direction == -1){
      direction = 1;
      LDR_VALUE = 0;
    }
    else{
      direction = -1;
      LDR_VALUE = 255;
    }
    is_fading = true;
  }
  else{
    is_fading = false;
    if(prev_tap_millis == 0){
      Serial.println("First Tap!");
      prev_tap_millis = current_tap_millis;
    }
    else {
      new_tap_millis = current_tap_millis - prev_tap_millis;
    }
    interval = new_tap_millis / fade_factor;

    if(interval >= MAX_INTERVAL)
      interval = MAX_INTERVAL;
    else if(interval <= 0)
      interval = 0;
      
  }
  prev_tap_millis = millis();
*/
}


void onEb1Encoder(EncoderButton& eb) {
  /*
  Serial.print("eb1 incremented by: ");
  Serial.println(eb.increment());
  Serial.print("eb1 position is: ");
  Serial.println(eb.position());
  */
  FADE_SPEED += eb.increment();
  
  if(FADE_SPEED >= MAX_SPEED)
    FADE_SPEED = MAX_SPEED;
  else if(FADE_SPEED <= MIN_SPEED)
    FADE_SPEED = MIN_SPEED;

 
  Serial.print("SPEED");
  Serial.print(" = ");
  
  Serial.println(FADE_SPEED);
  //EEPROM.put(0, SPEED);
  update_display();
}

void onEb2Encoder(EncoderButton& eb) {


  MAX_BRIGHTNESS += eb.increment() * DEPTH_SPEED;

  Serial.print(MAX_BRIGHTNESS);
  Serial.print(" => ");
  
  if(MAX_BRIGHTNESS >= 255)
    MAX_BRIGHTNESS = 255;
  else if(MAX_BRIGHTNESS <= 0)
    MAX_BRIGHTNESS = 0;
  
  Serial.println(MAX_BRIGHTNESS);
  //EEPROM.put(2, MAX_BRIGHTNESS);

  update_display();
}

void update_display(){
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  //display.println(enabled ? "ON" : "OFF");
  //display.setTextSize(1); // Draw 2X-scale text
  display.setCursor(0, 0);

  switch(MODE){
    case 0:
      display.println("SINE");
      break;
    case 1:
      display.println("TRIANGLE");
      break;
    default:
      display.println("WTF?");
      break;
  }
  display.setTextSize(1); // Draw 2X-scale text
  display.setCursor(0, 32);
  drawProgressbar(2,32,124,8, map(FADE_SPEED,MIN_SPEED,MAX_SPEED,0,99) ); // int x, int y, int width, int height, value
  drawProgressbar(2,42,124,8, map(MAX_BRIGHTNESS,0,255,0,98) ); // int x, int y, int width, int height, value
  display.setCursor(0, 56);
  display.print("S: "); display.print(FADE_SPEED); display.print(", D: "); display.print(MAX_BRIGHTNESS); display.println("");
  display.display();      // Show initial text
}


void setup() {
  setup_fast_pwm();

  Serial.begin(115200);

  eb1.setPressedHandler(onEb1Pressed);
  eb2.setPressedHandler(onEb2Pressed);

  sw1.setPressedHandler(sw1Pressed);
  sw2.setPressedHandler(sw2Pressed);

  eb1.setEncoderHandler(onEb1Encoder);
  eb2.setEncoderHandler(onEb2Encoder);

  // Calculate linear factor
  R = (255 * log10(2))/(log10(255));

  pinMode(0,INPUT_PULLUP);
  pinMode(1,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(LED_PIN_ON,OUTPUT);
  pinMode(EXPRESSION_DETECT_PIN,INPUT_PULLUP);


  
  

  //EEPROM.get(0, interval);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  update_display();

}


void write_output(){


  int led_brightness_1 = pow (2, (LDR1_VALUE / R)) - 1; // helligkeitsanpassung
  int led_brightness_2 = pow (2, (LDR2_VALUE / R)) - 1; // helligkeitsanpassung

  led_brightness_1 = map(led_brightness_1, 0, 255, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  led_brightness_2 = map(led_brightness_2, 0, 255, MIN_BRIGHTNESS, MAX_BRIGHTNESS);

  if(enabled){
    OCR1A = led_brightness_1;// Pin 9   LDR 1
    OCR1B = led_brightness_2;// Pin 10  LDR 2
    }
  else{
    OCR1A = 0; // Pin 9 LDR
    OCR1B = 0; // Pin 10 LDR
  }
 
  analogWrite(LED_PIN_1, led_brightness_1); // kontroll led 1
  analogWrite(LED_PIN_2, led_brightness_2); // kontroll led 2

}

void loop() {
  // Expression Pedal
  // detection
  bool expression_connected_changed = expression_connected;
  expression_connected = digitalRead(EXPRESSION_DETECT_PIN);
  if(expression_connected_changed != expression_connected){
    expression_connected_changed = expression_connected;
    update_display();
  }
  // auswertung
  if(expression_connected){
    expression_value = map(analogRead(EXPRESSION_VALUE_PIN),0,1023,0,255);

    //Serial.println(expression_value);
  }
  long wave_increment = 0;
  NON_BLOCKING_INTERVAL = FADE_SPEED;

  if(FADE_SPEED < 0){
    NON_BLOCKING_INTERVAL = 0;
    wave_increment = FADE_SPEED * -1;
  }
  else{
    NON_BLOCKING_INTERVAL = FADE_SPEED;
    wave_increment = 1;
  }

  // non-blocking interval
   unsigned long current_millis = millis();
  if (current_millis - previous_millis >= NON_BLOCKING_INTERVAL) {
    previous_millis = current_millis;
    WAVE_INDEX += wave_increment;

  }

  uint8_t phase1 = (uint8_t)(WAVE_INDEX);
  uint8_t phase2 = (uint8_t)(WAVE_INDEX+PHASE);

  switch(MODE){
    case 0: // SINE
        LDR1_VALUE = pgm_read_byte_near(SINE_WAVE + phase1);
        LDR2_VALUE = pgm_read_byte_near(SINE_WAVE + phase2);
    break;
    case 1: // TRIANGLE
        LDR1_VALUE = pgm_read_byte_near(TRIANGLE_WAVE + phase1);
        LDR2_VALUE = pgm_read_byte_near(TRIANGLE_WAVE + phase2);
    break;
    default:
      LDR1_VALUE = 255; // Mute
      LDR2_VALUE = 255; // Mute
    break;
  }


  write_output();
  update_inputs();

  
}

void update_inputs(){
  eb1.update();
  eb2.update();
  sw1.update();
  sw2.update();
}

void drawProgressbar(int x,int y, int width,int height, int progress)
{
   progress = progress > 100 ? 100 : progress; // set the progress value to 100
   progress = progress < 0 ? 0 :progress; // start the counting to 0-100
   float bar = ((float)(width-1) / 100) * progress;
   display.drawRect(x, y, width, height, WHITE);
   display.fillRect(x+2, y+2, bar , height-4, WHITE); // initailize the graphics fillRect(int x, int y, int width, int height)
  }
