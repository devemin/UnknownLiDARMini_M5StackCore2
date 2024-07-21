#include <M5Unified.h>

#define LIDARSerial Serial1

typedef struct
{
  uint16_t x;
  uint16_t y;
} Point_t;

typedef enum
{
  STATE_WAIT_HEADER = 0,
  STATE_READ_HEADER,
  STATE_READ_PAYLOAD,
  STATE_READ_DONE
} State_t;

typedef struct {
  uint8_t header0;
  uint8_t header1;
  uint8_t header2;
  uint8_t header3;
  uint16_t rotation_speed;
  uint16_t angle_begin;
  uint16_t distance_0;
  uint8_t reserved_0;
  uint16_t distance_1;
  uint8_t reserved_1;
  uint16_t distance_2;
  uint8_t reserved_2;
  uint16_t distance_3;
  uint8_t reserved_3;
  uint16_t distance_4;
  uint8_t reserved_4;
  uint16_t distance_5;
  uint8_t reserved_5;
  uint16_t distance_6;
  uint8_t reserved_6;
  uint16_t distance_7;
  uint8_t reserved_7;
  uint16_t distance_8;
  uint8_t reserved_8;
  uint16_t distance_9;
  uint8_t reserved_9;
  uint16_t distance_10;
  uint8_t reserved_10;
  uint16_t distance_11;
  uint8_t reserved_11;
  uint16_t distance_12;
  uint8_t reserved_12;
  uint16_t distance_13;
  uint8_t reserved_13;
  uint16_t distance_14;
  uint8_t reserved_14;
  uint16_t distance_15;
  uint8_t reserved_15;
  uint16_t angle_end;
  uint16_t crc;
} __attribute__((packed)) LidarPacket_t;

const uint8_t header[] = { 0x55, 0xaa, 0x23, 0x10 };

void setup(void) {
  /*
  M5.begin();
  // M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.printf("Start\n");
  Serial.begin(115200);
  */

  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setCursor(0,0);
  M5.Display.print("start!\n");
  Serial.begin(115200);



  LIDARSerial.begin(230400, SERIAL_8N1, 32, 33);

  delay(10);

  Serial.printf("Start\n");
}

uint16_t convertDegree(uint16_t input)
{
  return (input - 40960) / 64;
}

uint16_t convertSpeed(uint16_t input)
{
  return input / 64;
}

void remapDegrees(uint16_t minAngle, uint16_t maxAngle, uint16_t *map)
{
  int16_t delta = maxAngle - minAngle;
  if (maxAngle < minAngle) {
    delta += 360;
  }

  if ((map == NULL) || (delta < 0)) {
    return;
  }
  for (int32_t cnt = 0; cnt < 16; cnt++)
  {
    map[cnt] = minAngle + (delta * cnt / 15);
    if (map[cnt] >= 360) {
      map[cnt] -= 360;
    }
  }
}

void plotDistanceMap(uint16_t* degrees, uint16_t* distances)
{
  int32_t i;
  uint32_t x, y;
  static Point_t pointCloud[360];      // 360度分の点群

  for (i = 0; i < 16; i++) {
    M5.Display.drawPixel(pointCloud[degrees[i]].x, pointCloud[degrees[i]].y, BLACK);
    if (distances[i] < 10000) {
      x = cos((1.f * PI * degrees[i]) / 180.0f) * (distances[i] / 20.0f) + 160;
      y = sin((1.f * PI * degrees[i]) / 180.0f) * (distances[i] / 20.0f) + 120;

      M5.Display.drawPixel(x, y, WHITE);

      pointCloud[degrees[i]].x = x;
      pointCloud[degrees[i]].y = y;

    }
  }
}

void loop() {
  static State_t state;
  static uint32_t counter;
  static uint8_t payload[64];

  if (LIDARSerial.available())
  {
    uint8_t data = LIDARSerial.read();
    switch (state)
    {
      case STATE_WAIT_HEADER:
        if (data == header[0]) {
          counter++;
          payload[0] = data;
          state = STATE_READ_HEADER;
        } else {
          //printf("?? (%02X) Please do LiDAR power cycle\n", data);
          LIDARSerial.flush();
        }
        break;
      case STATE_READ_HEADER:
        if (data == header[counter]) {
          payload[counter] = data;
          counter++;
          if (counter == sizeof(header)) {
            state = STATE_READ_PAYLOAD;
          }
        } else {
          counter = 0;
          state = STATE_WAIT_HEADER;
        }
        break;
      case STATE_READ_PAYLOAD:
        payload[counter] = data;
        counter++;
        if (counter == sizeof(LidarPacket_t)) {
          state = STATE_READ_DONE;
        }
        break;
      case STATE_READ_DONE:
        LidarPacket_t* packet = (LidarPacket_t*)payload;
        {
          uint16_t degree_begin;
          uint16_t degree_end;
          degree_begin = convertDegree(packet->angle_begin);
          degree_end = convertDegree(packet->angle_end);
          if ((degree_begin < 360) && (degree_end < 360)) {
            printf("%3drpm %5d - %5d\n", convertSpeed(packet->rotation_speed), convertDegree(packet->angle_begin), convertDegree(packet->angle_end));
            uint16_t map[16];
            uint16_t distances[16];
            remapDegrees(degree_begin, degree_end, map);
            distances[0] = packet->distance_0 & 0x3FFF;
            distances[1] = packet->distance_1 & 0x3FFF;
            distances[2] = packet->distance_2 & 0x3FFF;
            distances[3] = packet->distance_3 & 0x3FFF;
            distances[4] = packet->distance_4 & 0x3FFF;
            distances[5] = packet->distance_5 & 0x3FFF;
            distances[6] = packet->distance_6 & 0x3FFF;
            distances[7] = packet->distance_7 & 0x3FFF;
            distances[8] = packet->distance_8 & 0x3FFF;
            distances[9] = packet->distance_9 & 0x3FFF;
            distances[10] = packet->distance_10 & 0x3FFF;
            distances[11] = packet->distance_11 & 0x3FFF;
            distances[12] = packet->distance_12 & 0x3FFF;
            distances[13] = packet->distance_13 & 0x3FFF;
            distances[14] = packet->distance_14 & 0x3FFF;
            distances[15] = packet->distance_15 & 0x3FFF;
            plotDistanceMap(map, distances);
          }
        }
        M5.Display.setCursor(0, 0);
        M5.Display.printf("Speed : %d rpm  \n", convertSpeed(packet->rotation_speed));
        state = STATE_WAIT_HEADER;
        counter = 0;
        state = STATE_WAIT_HEADER;
        break;
    }
  }
}
