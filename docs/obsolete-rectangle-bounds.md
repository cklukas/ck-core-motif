# Legacy rectangle helper bounds

The compatibility drawing helpers check rectangle-count arithmetic before
allocating their reusable `XRectangle` arrays. Impossible sizes are ignored
safely instead of wrapping the allocation or the Xlib rectangle count; normal
shadow and highlight rendering is unchanged.

The source-level regression is
`tests/automated/check-obsolete-rectangle-bounds.py`.
