---
nav_exclude: true
---
# Color-name digit suffix detection

Color matching checks whether a name ends in a digit.  Empty and missing names
now return false before pointer arithmetic, avoiding a one-before-start pointer
and making malformed color entries safe to process.
