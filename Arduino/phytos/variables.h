
#define DATA_PIN (2)
#define CLOCK_PIN (3)

#define LIGHT_COUNT (64)
#define DEVICE_ID (1)
#define PARAMETER_MAX (255)
#define START_VISUALIZE_TIME (40000)


#define WINDOW_X (26)
#define WINDOW_Y (800)

Adafruit_WS2801 strip = Adafruit_WS2801(LIGHT_COUNT, DATA_PIN, CLOCK_PIN);


struct led
{
  int x;
  int y;
  char type;
  uint8_t id;
  boolean alive;
};

struct chem
{
  float value;
  int prev;
  int diff;
  float velocity;
  uint8_t timelapsevals[365];
};

void updateChem(int pin, chem* c);

enum states{
  GAME,   
  SLEEP,
  PULLDOWN,
};

enum types{
  PLANKTON,
  TEMPERATURE,
  PH,
  NITROGEN
};

led leds [LIGHT_COUNT];
long visualize_time = -1;

