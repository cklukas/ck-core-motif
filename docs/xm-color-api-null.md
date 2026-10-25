# Xm color API null-safety

`XmGetColors` now returns before display or application-context lookup when
given a null screen. `XmeGetDefaultPixel` likewise returns before touching the
widget or output value when either is null. Optional color output pointers and
valid color calculation behavior remain unchanged.
