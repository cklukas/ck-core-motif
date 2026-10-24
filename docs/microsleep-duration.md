# XmeMicroSleep duration handling

`XmeMicroSleep` converts a microsecond duration to a `timeval` for `select`.
Negative durations previously reached the system call and produced an invalid,
platform-dependent timeout.

Nonpositive durations now return immediately with success. Positive durations
retain the existing second and microsecond conversion.

`tests/automated/check-microsleep-duration.py` guards the duration check.
