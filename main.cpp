#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1306 screen(128, 64, &Wire, 4);
Adafruit_MPU6050 mpu;

float base_val[] = {10, 10, 10.5};
float z_offset = 0;

enum Direction
{
  X = 0,
  Y = 1,
  Z = 2
};

float Rad2Deg(float r);
float Deg2Rad(float d);
float GetDeg(float n, int direct);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.println("Setup");
  if (!mpu.begin())
  {
    Serial.print("MPU init failed");
  }
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  screen.setTextSize(1);
  screen.setTextColor(WHITE);
  screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  screen.clearDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:
  screen.clearDisplay();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float AccX = a.acceleration.x;
  float AccY = a.acceleration.y;
  float AccZ = a.acceleration.z;
  //float AglX = GetDeg(AccX, X);
  float AglY = GetDeg(AccY, Y);
  float AglZ = GetDeg(AccZ, Z);
  screen.setCursor(0, 0);
  screen.print("AccX:");screen.println(AccX);
  screen.print("AccY:");screen.println(AccY);
  screen.print("AccZ:");screen.println(AccZ);
  screen.drawLine(0, 30, 50, 30, WHITE); //分割线
  screen.setCursor(0, 38);
  screen.print("AglY:");screen.println(AglY - 90);
  screen.print("AglZ:");screen.println(AglZ - 90);

  screen.drawCircle(95, 31, 31, WHITE); //圆圈
  
  screen.drawLine(90, 31, 100, 31, WHITE); //中央横线
  screen.drawLine(95, 11, 95, 51, WHITE);
  for (int i = 1; i < 6; i++) //小刻度
  {
    screen.drawLine(92, i * 10 + 1, 98, i * 10 + 1, WHITE);
  }

  z_offset = map(AglZ, 0, 180, 0, 64);
  float line_a = tan(Deg2Rad(AglY + 90));
  float line_b = z_offset - 96 * line_a;
  float c_D = -192;
  float c_E = -64;
  float c_F = 9279;

  float x1 = (-(2 * line_a * line_b + c_D + c_E * line_a) + sqrt(pow(2 * line_a * line_b + c_D + c_E * line_a, 2) - 4 * (line_a * line_a + 1) * (line_b * line_b + c_E * line_b + c_F))) / (2 * (line_a * line_a + 1));
  float x2 = (-(2 * line_a * line_b + c_D + c_E * line_a) - sqrt(pow(2 * line_a * line_b + c_D + c_E * line_a, 2) - 4 * (line_a * line_a + 1) * (line_b * line_b + c_E * line_b + c_F))) / (2 * (line_a * line_a + 1));

  float y1 = x1 * line_a + line_b;
  float y2 = x2 * line_a + line_b;

  screen.drawLine(x1, y1, x2, y2, WHITE);

  screen.display();
  delay(50);
}

float Rad2Deg(float r)
{
  return (180 / 3.1415) * r;
}

float Deg2Rad(float d)
{
  return (3.1415 / 180) * d;
}

float GetDeg(float n, int direct)
{
  float prop = n / base_val[direct];
  prop = prop > 1 ? 1 : prop;
  prop = prop < -1 ? -1 : prop;
  return Rad2Deg(acos(prop));
}
