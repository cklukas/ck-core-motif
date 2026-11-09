---
nav_exclude: true
---

# Appearance Xrm string bounds

Appearance profiles convert resource text through Xt's `XrmValue` structure,
whose size member is 32-bit.  The conversion path now rejects strings that
cannot include their terminating byte in that field before passing them to Xt.

Tab-mode, enumerated, and color profile properties share that guard.  Xrm
resource lookup and inherited pixel transport also use explicit Motif/Xt
boolean and argument-value conversions, avoiding implicit narrowing while
preserving the existing resource values.
