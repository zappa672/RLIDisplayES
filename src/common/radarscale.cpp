#include "radarscale.h"

#include <QtMath>

static const RLIScale scales[APCTRL_SNDPULSE_TYPES][APCTRL_SCALE_NUM] =
{
  // Scales for short sounding pulse (SNDPLS_SHORT)
  {
    {   0.125, SNDPLS_SHORT, 0x03200009, 0x001f0024, 0x001f4003,  0.025f,   "0.125",  "0.025", "%.3f"  },
    {   0.250, SNDPLS_SHORT, 0x03200009, 0x001f0024, 0x001f4003,  0.050f,   "0.25" ,  "0.05" , "%.3f"  },
    {   0.500, SNDPLS_SHORT, 0x0320000A, 0x001f0024, 0x001f4003,  0.100f,   "0.5"  ,  "0.1"  , "%.3f"  },
    {   0.750, SNDPLS_SHORT, 0x0320000B, 0x001f0024, 0x001f4003,  0.150f,   "0.75" ,  "0.15" , "%.3f"  },
    {   1.500, SNDPLS_SHORT, 0x06400013, 0x001f0024, 0x001f4003,  0.250f,   "1.5"  ,  "0.25" , "%.3f"  },
    {   3.000, SNDPLS_SHORT, 0x0320000F, 0x001f0024, 0x001f4003,  0.500f,   "3.0"  ,  "0.5"  , "%.3f"  },
    {   6.000, SNDPLS_SHORT, 0x06400017, 0x001f0024, 0x001f4003,  1.000f,   "6.0"  ,  "1.0"  , "%.3f"  },
    {  12.000, SNDPLS_SHORT, 0x0960001E, 0x001f0024, 0x001f4007,  2.000f,  "12.0"  ,  "2.0"  , "%.2f"  },
    {  24.000, SNDPLS_SHORT, 0x0960001E, 0x001f0024, 0x001f400F,  4.000f,  "24.0"  ,  "4.0"  , "%.2f"  },
    {  48.000, SNDPLS_SHORT, 0x12C00036, 0x001f0024, 0x001f400F,  8.000f,  "48.0"  ,  "8.0"  , "%.2f"  },
    {   0.000, SNDPLS_NONE,  0x19000144, 0x001f0024, 0x001f400F,  0.000f,   "0.0"  ,  "0.0"  , "%.1f"  }, //  96 nm valid only for SNDPLS_LONG
    {   0.000, SNDPLS_NONE,  0x19000145, 0x001f0024, 0x001f400F,  0.000f,   "0.0"  ,  "0.0"  , "%.1f"  }  // 144 nm valid only for SNDPLS_LONG
  },
  //  Scales for long sounding pulse (SNDPLS_LONG)
  {
    {   0.000, SNDPLS_NONE,  0x03200009, 0x001f0024, 0x001f4003,  0.025f,   "0.0"  ,  "0.0"  , "%.3f"  }, // 0.125 nm valid only for SNDPLS_SHORT
    {   0.000, SNDPLS_NONE,  0x03200009, 0x001f0024, 0x001f4003,  0.050f,   "0.0"  ,  "0.0"  , "%.3f"  }, // 0.25  nm valid only for SNDPLS_SHORT
    {   0.500, SNDPLS_LONG,  0x0320000A, 0x001f0024, 0x001f4003,  0.100f,   "0.5"  ,  "0.1"  , "%.3f"  },
    {   0.750, SNDPLS_LONG,  0x0320000B, 0x001f0024, 0x001f4003,  0.150f,   "0.75" ,  "0.15" , "%.3f"  },
    {   1.500, SNDPLS_LONG,  0x0640000D, 0x001f0024, 0x001f4003,  0.250f,   "1.5"  ,  "0.25" , "%.3f"  },
    {   3.000, SNDPLS_LONG,  0x0320000F, 0x001f0024, 0x001f4003,  0.500f,   "3.0"  ,  "0.5"  , "%.3f"  },
    {   6.000, SNDPLS_LONG,  0x06400017, 0x001f0024, 0x001f4003,  1.000f,   "6.0"  ,  "1.0"  , "%.3f"  },
    {  12.000, SNDPLS_LONG,  0x06400017, 0x001f0024, 0x001f4007,  2.000f,  "12.0"  ,  "2.0"  , "%.2f"  },
    {  24.000, SNDPLS_LONG,  0x06400017, 0x001f0024, 0x001f400F,  4.000f,  "24.0"  ,  "4.0"  , "%.2f"  },
    {  48.000, SNDPLS_LONG,  0x0C800027, 0x001f0024, 0x001f400F,  8.000f,  "48.0"  ,  "8.0"  , "%.2f"  },
    {  96.000, SNDPLS_LONG,  0x19000144, 0x001f0024, 0x001f400F, 16.000f,  "96.0"  , "16.0"  , "%.1f"  },
    { 144.000, SNDPLS_LONG,  0x19000145, 0x001f0024, 0x001f400F, 24.000f, "144.0"  , "24.0"  , "%.1f"  }
  }
};

RadarScale::RadarScale() {
  sndpls  = SNDPLS_SHORT;
  current = getScale(12.0, sndpls);

  if (current == nullptr)
    current = &scales[sndpls][0];
}

SoundingPulses RadarScale::getNextSndPlsType(SoundingPulses sndpls) {
  if (sndpls >= SNDPLS_LAST)
    sndpls = SNDPLS_FIRST;
  else
    sndpls = static_cast<SoundingPulses>(sndpls + 1);

  return sndpls;
}

SoundingPulses RadarScale::getPrevSndPlsType(SoundingPulses sndpls) {
  if (sndpls <= SNDPLS_FIRST)
    sndpls = SNDPLS_LAST;
  else
    sndpls = static_cast<SoundingPulses>(sndpls - 1);

  return sndpls;
}

const RLIScale* RadarScale::getScale(double len, SoundingPulses sndpls) {
  const RLIScale* pscale = nullptr;

  if ((sndpls < SNDPLS_FIRST) || (sndpls > SNDPLS_LAST))
    return nullptr;

  for (int i = 0; i < APCTRL_SCALE_NUM; i++) {
    if (fabs(scales[sndpls][i].len - len) < 0.001) {
      pscale = &scales[sndpls][i];
      break;
    }
  }

  return pscale;
}


const RLIScale *RadarScale::getCurScale(void) {
  return current;
}

int RadarScale::nextScale(void) {
  const RLIScale* nsc = nullptr; // Pointer to a new scale

  if (current == nullptr) {
      sndpls  = SNDPLS_SHORT;
      current = getScale(12.0, sndpls);
      nsc = current;
  } else {
    for (int i = 0; i < APCTRL_SCALE_NUM - 1; i++) {
      if (current == &scales[sndpls][i]) {
        nsc = &scales[sndpls][i + 1];

        if (nsc->sndpls == SNDPLS_NONE) {
          sndpls = getPrevSndPlsType(sndpls);
          nsc = &scales[sndpls][i + 1];
        }
      }
    }
  }

  if (nsc == nullptr)
    return 1;

  current = nsc;
  return 0;
}

int RadarScale::prevScale(void) {
  const RLIScale*  nsc = nullptr; // Pointer to a new scale

  if(current == nullptr) {
    sndpls  = SNDPLS_SHORT;
    current = getScale(12.0, sndpls);
    nsc = current;
  } else {
    for (int i = 1; i < APCTRL_SCALE_NUM; i++) {
      if (current == &scales[sndpls][i]) {
        nsc = &scales[sndpls][i - 1];

        if (nsc->sndpls == SNDPLS_NONE) {
          sndpls = getNextSndPlsType(sndpls);
          nsc = &scales[sndpls][i - 1];
        }
      }
    }
  }

  if(nsc == nullptr)
    return 1;

  current = nsc;
  return 0;
}
