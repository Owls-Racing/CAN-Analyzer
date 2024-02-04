#include <Arduino.h>
#include <ACAN2517FD.h>
/* CAN-Analyzer

[PURPOSE]
This program should serve you to find figure out what type of CAN is your
current device using.

[HOW IT WORKS]
It just does lol.

Owls Racing - Steffano Martinez - 2024
*/

/*
This lets the MCU know which SPI line to use as well as what the following PINS are:
PIN 14 -> CS
PIN 15 -> INT
*The chip in the tire temps use SPI to communicate with our MCU and CAN board, hence we need to declare it.
*/
SPIClass hspi;
ACAN2517FD myCanDevice(14, hspi, 15);

void setup()
{
  Serial.begin(115200); // Required for UART communication.

  /*
  We now need to define some settings within our MCP2517 chip inside the board,
  the following are set:
  - Oscillation at 40MHz
  - Data speed factor of : 1Mbit/s
  - Data speed rate factor of : 1x
  - Internal queue to hold up to 200 messages.
  */
  ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, 1000 * 1000, DataBitRateFactor::x1);
  settings.mDriverReceiveFIFOSize = 200;

  /* We defined the SPI line outside of the program, we need to initialize it now:
  PIN 18 -> SCK
  PIN 19 -> MISO
  PIN 23 -> MOSI
  PIN 14 -> SS
  */
  hspi.begin(18, 19, 23, 14);

  /*
  The errorCode holds a value that will gives us feedback in case our CAN settings are wrong.
  TODO: Explain the code a bit better.
  */
  const uint32_t errorCode = myCanDevice.begin(settings, []
                                               { myCanDevice.isr(); });

  if (errorCode == 0)
  {
    Serial.println("CAN initialization successful!");
  }
  else
  {
    Serial.print("CAN initialization FAILED, ERROR CODE -> ");
    Serial.println(errorCode, HEX);
  }
}

static uint32_t receivedFrameCount = 0;

void loop()
{
  CANFDMessage frame;

  if (myCanDevice.available())
  {
    myCanDevice.receive(frame);
    receivedFrameCount += 1;
    Serial.print("Quanitity of frames received: ");
    Serial.println(receivedFrameCount);
  }
}
