
#define POWER_MONITOR_PORT  MDR_PORTE
#define POWER_MONITOR_PIN   2



enum DeviceModes {MODE_NORMAL, MODE_CALIBRATION};
enum OutputModes {OUTPUT_UPDATE_NORMAL, OUTPUT_UPDATE_INSTANT};

extern uint8_t device_mode;


void PowerMonitor_Init(void);



