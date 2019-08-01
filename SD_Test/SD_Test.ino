#include "SD.h"
#include "Wire.h"
#include "freeram.h"
#include "mpu.h"

#define packet_size 16

File file;
unsigned int count[packet_size], counter = 0;
unsigned long time[packet_size];
float x[packet_size], y[packet_size], z[packet_size];
int ret;

unsigned int c = 0;     //cumulative number of successful MPU/DMP reads
unsigned int np = 0;    //cumulative number of MPU/DMP reads that brought no packet back
unsigned int err_c = 0; //cumulative number of MPU/DMP reads that brought corrupted packet
unsigned int err_o = 0; //cumulative number of MPU/DMP reads that had overflow bit set

void setup() {
  Serial.begin(230400);
  SD.begin(4);
  if (SD.exists("data.txt"))
    SD.remove("data.txt");
  file = SD.open("data.txt", FILE_WRITE);
  Wire.begin();
  ret = mympu_open(200);
  ret = freeRam();
}

void loop() {

  ret = mympu_update();

  switch (ret)
  {
  case 0:
    c++;
    break;
  case 1:
    np++;
    return;
  case 2:
    err_o++;
    return;
  case 3:
    err_c++;
    return;
  default:
    Serial.print("READ ERROR!  ");
    Serial.println(ret);
    return;
  }

  if (!(c % 25))
  {
    count[counter % packet_size] = counter;
    time[counter % packet_size] = micros();
    x[counter % packet_size] = mympu.ypr[0];
    y[counter % packet_size] = mympu.ypr[1];
    z[counter % packet_size] = mympu.ypr[2];
    if (counter % 32 == packet_size - 1)
    {
      WriteDatas();
    }
    counter++;
  }
}

void WriteDatas(){
  file = SD.open("data.txt", FILE_WRITE);
  for (int i = 0; i < packet_size;i++){
    file.print("No.");
    file.print(count[i]);
    file.print(" ");
    file.print(time[i]);
    file.print(" : ");
    file.print(x[i]);
    file.print(" ");
    file.print(y[i]);
    file.print(" ");
    file.print(z[i]);
    file.println();
  }
  file.close();
}
