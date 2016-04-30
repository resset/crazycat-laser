#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <Servo.h>

/*
 * Definitions
 */

#define PIN_SERVO_X 9
#define PIN_SERVO_Y 10
#define PIN_LASER   5

RF24 radio(7, 8);
byte addresses[][6] = {"1Node","2Node"};

Servo servo_x;
Servo servo_y;
uint32_t rx_data = 0;
uint8_t command = 0;
uint8_t servo_x_pos = 0;
uint8_t servo_y_pos = 0;

void servo(uint8_t *servo_x_pos, uint8_t *servo_y_pos)
{
  if (256 > *servo_x_pos && 256 > *servo_y_pos) {
    servo_x.write(*servo_x_pos);
    servo_y.write(*servo_y_pos);
  }
}

void setup()
{
  radio.begin();
  radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default
  radio.openReadingPipe(1, addresses[1]);
  radio.startListening();

  servo_x.attach(PIN_SERVO_X);
  servo_y.attach(PIN_SERVO_Y);

  servo_x_pos = 90;
  servo_y_pos = 90;
  servo(&servo_x_pos, &servo_y_pos);

  pinMode(PIN_LASER, OUTPUT);
  digitalWrite(PIN_LASER, LOW);

  Serial.begin(115200);

  delay(1000);

  Serial.println(F("CrazyCat v2 initialized"));
}

void loop()
{
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&rx_data, sizeof(uint32_t));
    }

    command = (rx_data >> 16) & 0xff;
    servo_x_pos = (rx_data >> 8) & 0xff;
    servo_y_pos = rx_data & 0xff;

    Serial.print("c: ");
    Serial.println(command);
    Serial.print("x: ");
    Serial.println(servo_x_pos);
    Serial.print("y: ");
    Serial.println(servo_y_pos);
    Serial.println();

    if (0x1 & command) {
      servo(&servo_x_pos, &servo_y_pos);
      if (0x2 & command) {
        digitalWrite(PIN_LASER, HIGH);
      } else {
        digitalWrite(PIN_LASER, LOW);
      }
    }
  }
}

