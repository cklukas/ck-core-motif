---
nav_exclude: true
---
# Mwm session database path bounds

The session-management database is loaded and saved through a fixed
`MAXPATHLEN` stack buffer.  Command-line and resource configuration can supply
an arbitrary database path, so copying that value directly could overwrite the
buffer before the session database was opened.

The non-WSM path now checks the complete source length before copying.  Paths
that fit retain their exact spelling; an overlong path falls back to the
standard `dtwmFileName`, matching the existing fallback used when the WSM
session path cannot fit. The fallback itself is bounded, so a future or
platform-specific default cannot overflow the fixed buffer.

`tests/automated/check-mwm-session-db-path.py` protects the source-level
contract.
