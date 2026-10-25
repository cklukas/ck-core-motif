---
nav_exclude: true
---
# Optional SVG image loading

SVG support is enabled with `--enable-svg` and requires `librsvg-2.0 >= 2.52`
through `pkg-config`. It is disabled by default. A request for the feature
fails during configuration when the maintained rasterizer is unavailable;
the normal build does not gain a librsvg runtime dependency.

The loader is reached through the existing `XmGetPixmapByDepth` and
`XmGetScaledPixmap` image-cache paths. Files whose names end in `.svg`
(case-insensitive) are read into a bounded 16 MiB buffer and handed to
librsvg from memory. This avoids granting the renderer a base URI for external
file retrieval. Intrinsic dimensions must be finite, positive and no larger
than 8192 pixels per edge. Rendering is flattened onto the requested X color
and requires a TrueColor or DirectColor visual; unsupported visuals and
malformed documents fail without creating a pixmap.

The existing pixmap cache still owns the resulting X pixmap. Its key retains
the display, depth, requested colors and scaling ratio, so repeated requests
reuse one entry while a different scale creates a separate entry. The
installed regression checks intrinsic dimensions, scale changes, cache reuse,
reference retirement, malformed input and an ordinary Xvfb window. Run it as:

```sh
python3 tests/automated/run-svg-image.py --prefix PREFIX --out ABSOLUTE-OUT
```

For a build configured without SVG, the same fixture can verify the explicit
unavailable result:

```sh
python3 tests/automated/run-svg-image.py --prefix PREFIX --out ABSOLUTE-OUT \
  --expect-unavailable
```
