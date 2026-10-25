# WSM config-format debug output

The WSM diagnostic formatter now passes its caller-provided buffer to the
initial `sprintf` in `PrintConfigFormat`.  The previous call omitted the
destination argument, so printing a configuration-format reply had undefined
behavior even though the rest of the formatter expected the buffer to contain
the first line.

`tests/automated/check-wsm-debug-config-format.py` protects the call site.
