// серво на пине D12
// датчики на A0, A1, A2
#define USE_KEYBOARD 1    // только для Micro!
#define DEBUG 1
#define DEBUG_ANALOG 0
#define MEDIAN_WINDOW 5

// правый датчик на кнопке, зум 175
int steady1, steady2;
uint32_t changeTimer, changeTime;
bool gap1 = 0, gap2 = 0;
bool prevGap1 = 0, prevGap2 = 0;

bool jumpState = 0;
int waitTime = 0;
int flyTime = 0;
uint32_t speedTime, dashTime;
int steadyF1, steadyF2;
int buf1[3], buf2[3], buf3[3], bufWait[3];

#include <Servo.h>
Servo servo;
#include <GyverTimer.h>
GTimer jumpTimer(MS);
GTimer timeoutTimer(MS, 7000);

#include "median.h"
median3 filCur1(buf1);
median3 filCur2(buf2);
median3 filCur3(buf3);
median3 filWait(bufWait);

uint32_t debounce1, debounce2;

#if (USE_KEYBOARD == 1)
#include <Keyboard.h>
#endif

void setup() {
#if (USE_KEYBOARD == 1)
  delay(5000);
  Keyboard.begin();
#endif

#if (DEBUG == 1)
  Serial.begin(9600);
  Serial.println("kek");
#endif

  servo.attach(12);
  servo.write(60);
}

void loop() {
  static int count = 0;
  static int countWait1 = 0, countWait2 = 0;
  count++;

  int curVal1 = filCur1.filter(analogRead(A1));
  int curVal2 = filCur2.filter(analogRead(A0));
  int curVal3 = filCur3.filter(analogRead(A2));

  static int lastCur3;

  if (count % 5 == 0) {
    int delta1 = steadyF1 - curVal1;
    int delta2 = steadyF2 - curVal2;
    int delta3 = curVal3 - lastCur3;

    if (abs(delta1) < 8) {
      steadyF1 = curVal1;
      countWait1 = 0;
    } else {
      countWait1++;
      if (countWait1 > 15) steadyF1 = curVal1;
      steadyF1 += delta3;
    }

    if (abs(delta2) < 8) {
      steadyF2 = curVal2;
      countWait2 = 0;
    } else {
      countWait2++;
      if (countWait2 > 15) steadyF2 = curVal2;
      steadyF2 += delta3;
    }
    lastCur3 = curVal3;
  }

  gap1 = (millis() - debounce1 > 150 && abs(curVal1 - steadyF1) > 20);
  gap2 = (millis() - debounce2 > 150 && abs(curVal2 - steadyF2) > 20);

  if (prevGap1 != gap1) {
    if (gap1) { // фронт
      speedTime = millis();
      dashTime = millis();
      timeoutTimer.reset();
    } else {    // спад
      flyTime = (millis() - dashTime) * 4.5 + 10;
      flyTime = constrain(flyTime, 0, 280);
      debounce1 = millis();
    }
    prevGap1 = gap1;    
  }

  if (prevGap2 != gap2) {
    if (gap2) {
      waitTime = filWait.filter(constrain(millis() - speedTime, 40, 180));
      jumpTimer.setTimeout(map(waitTime, 180, 50, 180, 5));
    } else {
      debounce2 = millis();
    }
    prevGap2 = gap2;    
  }

  if (jumpTimer.isReady()) {
    if (!jumpState) {
      jumpState = 1;
      //servo.write(120);
#if (USE_KEYBOARD == 1)
      Keyboard.release(KEY_DOWN_ARROW);
      delay(10);
      Keyboard.press(KEY_UP_ARROW);
#endif
      jumpTimer.setTimeout(flyTime);
    } else {
      //servo.write(60);
#if (USE_KEYBOARD == 1)
      Keyboard.release(KEY_UP_ARROW);
      delay(10);
      Keyboard.press(KEY_DOWN_ARROW);
#endif
      jumpTimer.stop();
      jumpState = 0;
    }
  }

// если проиграли, пнуть игру автоматически
#if (USE_KEYBOARD == 1)
  if (timeoutTimer.isReady()) {    
    Keyboard.press(KEY_UP_ARROW);
    delay(10);
    Keyboard.release(KEY_UP_ARROW);
    waitTime = 180;
  }
#endif

#if (DEBUG == 1)
#if (DEBUG_ANALOG == 0)
  Serial.print(curVal1);
  Serial.print(',');
  Serial.print(steadyF2);
  Serial.print(',');
  Serial.println(gap1 * 100);
  /*Serial.print(',');
    Serial.print(200 + gap1 * 100);
    Serial.print(',');
    Serial.println(200 + gap2 * 100);*/
#else
  Serial.print(curVal1);
  Serial.print(',');
  Serial.print(curVal2);
  Serial.print(',');
  Serial.println(curVal3);
#endif
#endif

  delay(5);
}
