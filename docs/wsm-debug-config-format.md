---
nav_exclude: true
---
# WSM config-format debug output

The WSM diagnostic formatter now passes its caller-provided buffer to the
initial `sprintf` in `PrintConfigFormat`.  The previous call omitted the
destination argument, so printing a configuration-format reply had undefined
behavior even though the rest of the formatter expected the buffer to contain
the first line.

All debug sections now append through a bounded formatter.  Oversized
extension, attribute, or value lists are truncated at the existing static
buffer boundary instead of overwriting adjacent state; normal output remains
unchanged while it fits.

`tests/automated/check-wsm-debug-config-format.py` protects both the original
call site and the bounded formatter contract.
