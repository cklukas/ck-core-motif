---
nav_exclude: true
---
# XmScale API null-safety

`XmScaleSetValue` now returns for null or non-Scale widgets, and
`XmScaleGetValue` returns for null/non-Scale widgets or a null output pointer
before Xt context lookup. Valid scale updates and reads retain their existing
behavior.
