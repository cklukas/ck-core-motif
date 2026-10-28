---
nav_exclude: true
---
# MWM backdrop pixmap request input

The backdrop-change request now requires a complete hexadecimal pixmap ID that
fits the platform's unsigned-long conversion. Malformed and overflowing IDs
are rejected with a warning and never reach `SetNewBackdrop` as an accidental
zero or truncated value.
