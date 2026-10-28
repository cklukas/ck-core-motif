---
nav_exclude: true
---
# Drag top-window property bounds

The multi-screen drag path receives top-window lists from an X property.  It
now validates the property pointers and count before allocating the receiver
array, and only replaces the previous array after allocation succeeds.  Valid
window lists retain their existing tracking behavior; malformed or oversized
lists leave the current drag state unchanged.
