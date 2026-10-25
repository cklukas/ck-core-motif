# LabelG pixmap metadata

LabelG redisplay now copies sensitive and insensitive pixmaps only after
`XmeGetPixmapData` succeeds.  The insensitive stipple fallback is inside the
same success path, so an unresolved pixmap cannot be interpreted through an
uninitialized depth or used for a fill operation.

Successful depth-matched copies and bitmap planes retain their existing
behavior.  The focused regression is
`tests/automated/check-labelg-pixmap-metadata.py`.
