# LabelG background pixmap metadata

LabelG background GC setup now adds stipple or tile fields only when
`XmeGetPixmapData` successfully resolves the parent background pixmap.  A
failed lookup falls back to the ordinary foreground/background GC instead of
using an uninitialized depth or an invalid pixmap resource.

Successful bitmap and pixmap metadata retain their existing opaque stipple and
tile behavior.  The focused regression is
`tests/automated/check-labelg-background-pixmap-metadata.py`.
