---
nav_exclude: true
---
# Mwm config-file path bounds

Mwm searches several resource, locale, and system configuration paths in a
fixed `MAXWMPATH` buffer. `FopenConfigFile` now builds that buffer through
checked set/append helpers using `snprintf`; failed or unrepresentable paths
clear the candidate instead of allowing `strcpy`/`strncat` truncation or size
underflow. The existing search order and filenames remain unchanged.

`tests/automated/check-config-path-bounds.py` guards the helper contract and
ensures the config-file search has no direct unbounded copies.
