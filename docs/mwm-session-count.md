---
nav_exclude: true
---
# MWM session item counts

The session-file record count is now parsed as a complete, positive integer
within the representable range before `GetSessionHintsInfo` allocates storage.
Malformed, overflowing, zero, and negative counts are rejected with a warning,
so they cannot wrap into an unrelated allocation size.
