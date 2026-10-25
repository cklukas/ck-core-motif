# Xm color API null-safety

`XmGetColors` now returns for a null screen before display/app-context lookup.
`XmeGetDefaultPixel` returns for a null widget or `XrmValue` output before
writing conversion results. Optional color output pointers and valid color
calculation behavior remain unchanged.
