---
nav_exclude: true
---
# Fallback text-property value allocation

The fallback path in `XmCvtXmStringToText` converts a compound string to a
text property before asking Xlib for a text list.  Its temporary property value
is now checked before copying and passes its bounded byte count explicitly as
an Xt `Cardinal`; allocation failure frees the compound text, releases the
string context, and returns `XNoMemory`.

The regression check is
`tests/automated/check-txtpropcv-value-allocation.py`.
