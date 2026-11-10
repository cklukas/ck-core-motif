---
nav_exclude: true
---

# Compound String draw-length bounds

Compound String entries retain their byte count as an unsigned value, but the
Xlib, Xmb, UTF-8, and Xft draw interfaces accept signed `int` lengths.  Before
preparing an RTL buffer or calling a drawing interface, `_XmStringDrawSegment`
now rejects a segment larger than `INT_MAX`.  This prevents a negative
converted draw length and prevents the RTL loop's signed counters from
overflowing.

Normal labels and compound strings are unchanged.  An unrepresentably large
single segment is skipped safely rather than reaching a drawing backend with an
invalid length.

The focused contract check is:

    python3 tests/automated/check-xmstring-draw-length-bounds.py
