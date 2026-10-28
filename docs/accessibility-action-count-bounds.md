# Accessibility action-count bounds

The AT-SPI `NActions` property is a signed 32-bit D-Bus integer, while Motif
stores the action count as an unsigned value.  The D-Bus adapter now saturates
counts above `INT32_MAX` before publishing them, instead of allowing a large
count to wrap into a negative property value.

Normal action counts are unchanged and the public Motif accessibility ABI is
unchanged.
