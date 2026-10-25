---
nav_exclude: true
---

# MWM session database suffix updates

Session database names now increment a numeric suffix with checked conversion
and bounded formatting. Non-numeric names still receive `.0`, but the append
is bounded by the spare capacity reserved for successive session files.
Overflowing or unrepresentable suffixes are left unchanged rather than invoking
undefined integer conversion behavior.
