---
nav_exclude: true
---

# MWM property-warning allocation widths

The shared `WmWarningWithString` helper formats localized property warnings
with `snprintf` and allocates the resulting message through Xt. It now rejects
negative or unrepresentable lengths before converting the signed formatter
result to an Xt `Cardinal`. Allocation failure and out-of-range formatting use
the existing warning fallback, while normal warning text and localization
remain unchanged.
