#include <Arduino.h>
#include "FS.h"
#include "WiFi.h"

#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "Wire.h"
#include "AC101.h"
#include "Audio.h"
#include "TFT_eSPI.h"

#include "defines.h"
#include "keys.h"
#include "touch_calibrate.h"

//------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  tft.init();
  tft.setRotation(3);
  touch_calibrate();
  tft.fillScreen(TFT_BLACK);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  SPI.begin(14 /* SCK */, 2 /* MISO */, 15 /* MOSI */);
  SPI.setFrequency(1000000);
  Serial.printf("Connect to AC101 codec... ");
  while (not ac.begin(IIC_DATA, IIC_CLK))
  {
    Serial.printf("Failed!\n");
    delay(1000);
  }
  Serial.printf("OK\n");

  ac.SetVolumeSpeaker(volume);
  ac.SetVolumeHeadphone(volume);
  // ac.DumpRegisters();

  // enable amplifier
  pinMode(GPIO_PA_EN, OUTPUT);

  // Configure keys on ESP32 Audio Kit board
  pinMode(PIN_VOL_UP, INPUT_PULLUP);
  pinMode(PIN_VOL_DOWN, INPUT_PULLUP);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_MODE_SEL, INPUT);
  pinMode(PIN_PA_ENABLE, INPUT);

  //13 indicates the Chip select pin
  if (SD.begin(13))
  {
    Serial.println("SD opened!");
  }
  audio.i2s_mclk_pin_select(I2S_MCLK);
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DSIN);
  audio.setVolume(volume); // 0...61
  audio.connecttohost(stations[curStation]);
  
  File dir = SD.open("/");
  for (uint8_t i = 0; i < MAXFILES; i++)
  {
    File entry = dir.openNextFile();
    if (!entry)
      break;
    if (!entry.isDirectory())
    {
      fileList[fileCount] = strdup(entry.name());
      fileCount++;
      Serial.print(fileCount - 1);
      Serial.println(fileList[fileCount - 1]);
      entry.close();
    }
  }
}

//------------------------------------------------------------------------------------------

void loop(void)
{
  digitalWrite(GPIO_PA_EN, digitalRead(PIN_PA_ENABLE)); //switch PA or Headphone
  audio.loop();
  keys();
  if (millis() > (scroll + 200))
  {
    scroll = millis();
    if (sStation.length() > 20)
    {
      tft.setFreeFont(&FreeSerif18pt7b);
      tft.fillRect(0, 0, 320, 30, TFT_BLACK);
      tft.setTextColor(TFT_WHITE);
      xPosStation -= 8;
      tft.drawString(sStation, xPosStation, 0);
      if (xPosStation <= sStation.length() * (-16))
        xPosStation = 0;
    }
    if (sinfo.length() > 26)
    {
      tft.setFreeFont(&FreeSerif12pt7b);
      tft.fillRect(0, 60, 320, 22, TFT_BLACK);
      tft.setTextColor(TFT_BLUE);
      xPos -= 4;
      tft.drawString(sinfo, xPos, 60);
      if (xPos <= sinfo.length() * (-10))
        xPos = 0;
    }
  }
}

void playSD()
{
  while (!audio.connecttoSD(fileList[playNumber]))
  {
    if (playNumber == 0)
      playNumber = fileCount - 1;
    playNumber++;
    if (playNumber == fileCount)
      playNumber = 0;
  }
}

//------------------------------------------------------------------------------------------

// optional
void audio_info(const char *info)
{
  Serial.print("info        ");
  Serial.println(info);
}
void audio_bitrate(const char *info)
{
  Serial.print("info        ");
  Serial.println(info);
}
void audio_id3data(const char *info)
{ //id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
  sinfo = String(info);
  if (sinfo.startsWith("Artist:"))
  {
    sinfo.replace("Artist: ", "");
    sArtist = sinfo;
    Serial.println(sArtist);
  }
  if (sinfo.startsWith("Title:"))
  {
    sinfo.replace("Title: ", " - ");
    sTitle = sinfo;
    Serial.println(sTitle);
  }
  if (sArtist != "" && sTitle != "")
  {
    sinfo = sArtist + sTitle;
    sArtist = "";
    sTitle = "";
    Serial.println(sinfo);
    xPos = 0;
    tft.fillRect(0, 60, 320, 22, TFT_BLACK);
    tft.setFreeFont(&FreeSerif12pt7b);
    tft.setTextColor(TFT_BLUE);
    tft.drawString(sinfo, xPos, 60);
  }
}
void audio_eof_mp3(const char *info)
{ //end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
  playNumber++;
  playSD();
}

void audio_showstation(const char *info)
{
  Serial.print("station     ");
  Serial.println(info);
  sStation = String(info);
  xPosStation = 0;
  tft.fillRect(0, 0, 320, 30, TFT_BLACK);
  tft.setFreeFont(&FreeSerif18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(sStation, xPosStation, 0);
}
void audio_showstreamtitle(const char *info)
{
  Serial.print("streamtitle ");
  Serial.println(info);
  sinfo = String(info);
  sinfo.replace("|", " - ");
  xPos = 0;
  tft.fillRect(0, 60, 320, 22, TFT_BLACK);
  tft.setFreeFont(&FreeSerif12pt7b);
  tft.setTextColor(TFT_BLUE);
  tft.drawString(sinfo, xPos, 60);
}
void audio_commercial(const char *info)
{ //duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info)
{ //homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info)
{ //stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}
void audio_eof_speech(const char *info)
{
  Serial.print("eof_speech  ");
  Serial.println(info);
}
