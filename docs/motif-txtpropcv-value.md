# Fallback text-property value allocation

The fallback path in `XmCvtXmStringToText` converts a compound string to a
text property before asking Xlib for a text list. Its temporary property value
is now checked before copying; allocation failure frees the compound text,
releases the string context, and returns `XNoMemory`.

The regression check is
`tests/automated/check-txtpropcv-value-allocation.py`.
