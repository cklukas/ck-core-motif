# Updating a rendition with a font name

When a rendition has an already-loaded font but no `XmNfontName`, adding a
font name must invalidate the old font so the next validation loads the named
font.  The update path now checks whether a prior font exists directly; it no
longer compares a font pointer with the newly supplied string.
