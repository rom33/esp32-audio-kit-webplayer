TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

// SPI GPIOs
#define SD_CS 13
#define SPI_MOSI 15
#define SPI_MISO 2
#define SPI_SCK 14

// I2S GPIOs, the names refer on AC101
#define I2S_DSIN 25
#define I2S_BCLK 27
#define I2S_LRC 26
#define I2S_MCLK 0
#define I2S_DOUT 35

// I2C GPIOs
#define IIC_CLK 32
#define IIC_DATA 33

// Tasten
#define PIN_VOL_UP 5     // KEY 5
#define PIN_VOL_DOWN 18  // KEY 6
#define PIN_UP 23        // KEY 4
#define PIN_DOWN 19      // KEY 3
#define PIN_MODE_SEL 36  // KEY 1
#define PIN_PA_ENABLE 39 // no Headphone pluged in?

// PA enable
#define GPIO_PA_EN 21

//Switch S1: 1-OFF, 2-ON, 3-ON, 4-ON, 5-ON

const char *ssid = "xxx";
const char *password = "xxx";

String stations[] = {
    "0n-80s.radionetz.de:8000/0n-70s.mp3",
    "mediaserv30.live-streams.nl:8000/stream",
    "www.surfmusic.de/m3u/100-5-das-hitradio,4529.m3u",
    "stream.1a-webradio.de/deutsch/mp3-128/vtuner-1a",
    "mp3.ffh.de/radioffh/hqlivestream.aac", //  128k aac
    "www.antenne.de/webradio/antenne.m3u",
    "listen.rusongs.ru/ru-mp3-128",
    "edge.audio.3qsdn.com/senderkw-mp3",
    "macslons-irish-pub-radio.com/media.asx",
    "http://www.wdr.de/wdrlive/media/einslive.m3u",
};

static AC101 ac;
int volume = 30; // 0...61
int curStation;
int xPosStation;
int xPos;
unsigned short MODE;
unsigned long debounce = 0;
unsigned long scroll = 0;
String sinfo;
String sStation;
String sArtist;
String sTitle;

#define MAXFILES 100
char* fileList[MAXFILES];
int fileCount;
int playNumber;
Audio audio;

void touch_calibrate();
void keys();
void playSD();
bool pressed(const int pin);
