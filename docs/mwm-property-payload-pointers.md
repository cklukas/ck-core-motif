# MWM X11 property payload validation

Several MWM property readers receive an allocated payload pointer from Xlib.
The readers now require that pointer to be non-null alongside the expected
type, format, and item count before copying or parsing data.  This keeps
malformed or failed property replies from becoming null-pointer accesses.
