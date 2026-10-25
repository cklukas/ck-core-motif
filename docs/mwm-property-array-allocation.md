---
nav_exclude: true
---

# MWM property-array allocation

WM protocol and `_MWM_MESSAGES` properties provide item counts from X11. MWM
now checks both multiplication overflow and Xt `Cardinal` representability
before allocating the corresponding `Atom` or `long` array. Invalid or
unrepresentable counts follow the existing insufficient-memory warning path;
normal property processing is unchanged.
