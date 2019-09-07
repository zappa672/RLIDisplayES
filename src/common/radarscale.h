#ifndef RADARSCALE_H
#define RADARSCALE_H

//#include <stdint.h>

#include <utility>
#include <sys/types.h>

#ifdef Q_OS_WIN
typedef unsigned __int32 u_int32_t;
#endif

#define APCTRL_SCALE_NUM         12 // Number of RLI scales
#define APCTRL_SNDPULSE_TYPES    2  // Numer of sounding pulse types (now only two: short and long)
#define APCTRL_SNDPULSE_SHORT    0  // Index in scales table for short sounding pulse
#define APCTRL_SNDPULSE_LONG     1  // Index in scales table for long sounding pulse

// Data types for managing scales
enum class SoundingPulses: int {
  NONE  = -1,
  FIRST =  0,
  SHORT =  0,
  LONG  =  1,
  LAST  =  1
};

struct RLIScale {
  double          len;              // Length of this scale in miles; 0 means invalid scale
  SoundingPulses  sndpls;           // Type of sounding pulse
  u_int32_t       pkidpkod;         // Settings for PKID and PKOD
  u_int32_t       gen_addr;         // Value to set the address of ADC Frequency Register
  u_int32_t       gen_dat;          // Value to set the contents of ADC Frequency Register
  float           dist_rng;         // Space in miles between distance rings
  const char*     display;          // Name of the scale for displaying
  const char*     dist_rng_display; // How to display space size between distance rings
  const char*     val_fmt;          // printf format to display float value
};

class RadarScale {
public:
  RadarScale();

  const RLIScale* getCurScale(void);

  int nextScale(void);
  int prevScale(void);

private:
  static const RLIScale* getScale(double len, SoundingPulses sndpls);

  static SoundingPulses getNextSndPlsType(SoundingPulses sndpls);
  static SoundingPulses getPrevSndPlsType(SoundingPulses sndpls);

  SoundingPulses sndpls;
  const RLIScale *current;
};

#endif // RADARSCALE_H
