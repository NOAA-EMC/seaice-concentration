
// Make function to do the computations/filtering regarding weather.
// Isolate the decision to this, rather than the embedded structure
//   (in to nasa_team and team2) in the prior renditions of weather
//   filtering.
// Proximally prompted by the F-15 new filter, but makes sense for
//   any system

float new_weather(float &t19v, float &t19h, float &t22v, float &t37v, float &t37h,
             float &t87v, float &t87h, int satno) {
    float gr01, gr05;
    float grcrit_ice = -0.055, grcrit_water = +0.2412;
    // New filtering:
    gr01 = (t19v - t19h) / (t19v + t19h);
    gr05 = (t19v - t87v) / (t19v + t87v);
    if (gr01 > grcrit_water && gr05 > grcrit_ice) {
      return WEATHER;
    }
    else if (gr01 > grcrit_water) {
      return 0.0;
    }
    else if (gr05 > grcrit_ice) {
      return 1.0;
    }
    else {
      return WEATHER;
    }

    return WEATHER;
}

float old_weather(float &t19v, float &t19h, float &t22v, float &t37v, float &t37h,
             float &t87v, float &t87h, int satno) {
    float gr37, gr22;
    gr37 = (t37v - t19v) / (t37v + t19v);
    gr22 = (t22v - t19v) / (t22v + t19v);
    if (gr22 > SSMI_GR22LIM || gr37 > SSMI_GR37LIM) {
      if (gr37 > SSMI_GR37LIM) filt37 += 1;
      if (gr22 > SSMI_GR22LIM) filt22 += 1;
      return WEATHER;
    }
    return 1.0;
}
