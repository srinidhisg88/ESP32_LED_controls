#include <Arduino.h>
struct LedTask {
  uint8_t pin;
  uint16_t period;    
  uint16_t counter;
  bool state;
  volatile bool flag;
};

// Timer tick = 10 ms (reduces CPU load compared to 1 ms)
const uint16_t TIMER_TICK_MS = 1;

// Define LEDs (pin, period in ms)
LedTask leds[] = {
  {2,   500 / TIMER_TICK_MS, 0, false, false}, // onboard LED (500 ms)
  {22,   50 / TIMER_TICK_MS, 0, false, false}, // external LED (50 ms)
  {23,  167 / TIMER_TICK_MS, 0, false, false}  // external LED (167 ms)
};

const uint8_t LED_COUNT = sizeof(leds) / sizeof(leds[0]);

hw_timer_t * timer = NULL;


void IRAM_ATTR onTimer() {
  for (int i = 0; i < 3; i++) {
    leds[i].counter++;
    if (leds[i].counter >= leds[i].period) {
      leds[i].flag = true;
      leds[i].counter = 0;
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("LED Blinker Started");


  for (int i = 0; i < LED_COUNT; i++) {
    pinMode(leds[i].pin, OUTPUT);
    digitalWrite(leds[i].pin, LOW);
  }

  // Init hardware timer
  timer = timerBegin(0, 80, true);                  // Timer 0, prescaler 80 → 1 tick = 1 µs
  timerAttachInterrupt(timer, &onTimer, true);      // Attach ISR
  timerAlarmWrite(timer, TIMER_TICK_MS*1000 , true); 
  timerAlarmEnable(timer);                          // Start
}


void loop() {
  
  for (int i = 0; i < LED_COUNT; i++) {
    if (leds[i].flag) {
      leds[i].flag = false;
      leds[i].state = !leds[i].state;
      digitalWrite(leds[i].pin, leds[i].state);
    }
  }
  

  delay(1); // yield CPU
}