# XPM buffer extension output bounds

The XPM buffer exporter now treats extension serialization as a checked
operation. `WriteExtensions` validates extension names and line arrays, checks
each `snprintf` result against the remaining output capacity, and reports
failure to the public exporter instead of allowing an unsigned size underflow
or a truncated pointer advance.

The focused source regression is
`tests/automated/check-xpm-buffer-extensions-bounds.py`.
