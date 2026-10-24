# PNG XImage pixel layout

PNG conversion previously wrote four bytes for every 24/32-bit destination
pixel.  That is correct for the common 32-bit XImage layout, but a valid
24-bit XImage has three bytes per pixel and the extra write could corrupt the
following pixel or row.

The converter now derives the byte count from `XImage.bits_per_pixel`, accepts
three- and four-byte layouts, and writes the pixel in the configured
`MSBFirst` order.  Unsupported layouts fail cleanly.  The source contract is
covered by `tests/automated/check-png-pixel-layout.py`.
