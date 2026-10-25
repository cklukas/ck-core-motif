# XmScale API null-safety

`XmScaleSetValue` now returns for a null widget, and `XmScaleGetValue` returns
for either a null widget or null output pointer before Xt context lookup. Valid
scale updates and reads retain their existing behavior.
