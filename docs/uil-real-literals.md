---
nav_exclude: true
---
# UIL real literals

UIL real-literal conversion now requires `strtod` to consume the complete
literal and produce a finite value without range errors. Overflow, NaN,
Infinity, malformed text, and trailing characters produce the existing real
literal diagnostic and are stored as `0.0` rather than propagating an unsafe
semantic value.
