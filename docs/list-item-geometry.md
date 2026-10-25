---
nav_exclude: true
---
# List item geometry

`XmList` item geometry queries derive the returned item width by subtracting
list margins and shadow thickness from the unsigned widget width. A collapsed
list could wrap that result and report a huge item rectangle to callers.

The query now computes available width in signed arithmetic and returns zero
when no positive content span remains. Normal item geometry is unchanged for
usable list sizes.
