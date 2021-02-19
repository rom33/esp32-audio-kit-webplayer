bool pressed( const int pin )
{
  if (millis() > (debounce + 500))
  {
    if (digitalRead(pin) == LOW)
    {
      debounce = millis();
      return true;
    }
  }
  return false;
}
void keys() {
  bool updateVolume = false;
  if (pressed(PIN_VOL_UP))
  {
    if (volume <= (61))
    {
      // Increase volume
      volume ++;
      updateVolume = true;
    }
  }
  if (pressed(PIN_VOL_DOWN))
  {
    if (volume >= 1)
    {
      // Decrease volume
      volume --;
      updateVolume = true;
    }
  }
  if (pressed(PIN_UP))
  {
    if (MODE == 1) {
      playNumber++;
      if (playNumber == fileCount)playNumber = 0;
      playSD();
    } else {
      curStation++;
      sinfo = "";
      tft.fillRect(0, 60, 320, 22, TFT_BLACK);
      if (curStation > sizeof(stations) / sizeof(stations[0]) - 1)curStation = 0;
      audio.connecttohost(stations[curStation]);
    }
  }
  if (pressed(PIN_DOWN))
  {
    if (MODE == 1) {
      if (playNumber == 0)playNumber = fileCount - 1;
      else playNumber--;
      playSD();
    } else {
      curStation--;
      sinfo = "";
      tft.fillRect(0, 60, 320, 22, TFT_BLACK);
      if (curStation == -1)curStation = sizeof(stations) / sizeof(stations[0]) - 1;
      audio.connecttohost(stations[curStation]);
    }
  }
  if (pressed(PIN_MODE_SEL))
  {
    MODE ++;
    if (MODE > 2) MODE = 0;
    switch (MODE) {
      case 0:
        tft.fillScreen(TFT_BLACK);
        audio.connecttohost(stations[curStation]);
        break;
      case 1:
        sinfo = "";
        tft.fillScreen(TFT_BLACK);
        tft.setFreeFont(&FreeSerif18pt7b);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(0, 24);
        tft.print("MP3 Player");

        playSD();
        break;
      case 2:

        break;
      case 3:

        break;
    }
  }
  if (updateVolume)
  {
    // Volume change requested
    Serial.printf("Volume %d\n", volume);
    ac.SetVolumeSpeaker(volume);
    ac.SetVolumeHeadphone(volume);
  }
}
