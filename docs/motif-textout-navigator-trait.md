---
nav_exclude: true
---
# TextOut navigator callback validation

TextOut now checks the navigator trait’s `getValue` callback before reading
horizontal, vertical, or slider positions. A partial navigator trait is
ignored safely, while complete navigator implementations keep the existing
scroll and slider behavior.
