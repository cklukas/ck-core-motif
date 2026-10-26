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

## Built-in dialog artwork

SVG-enabled libraries embed five 40×40 vector symbols: `dialog_error`,
`dialog_information`, `dialog_question`, `dialog_warning`, and `dialog_working`.
Their outlined shapes, restrained red/blue/teal/amber/violet colors and bevel
highlights retain a workstation-era appearance at a readable size. Meaning is
also expressed by shape and symbol, not only by color. The SVGs contain paths
and shapes, so the result does not depend on installed fonts.

`XmMessageBox` first honors an explicit `XmNsymbolPixmap` or application image
override. Its default then uses the vector symbol, falling back to the original
bitmap if SVG rendering is unavailable. Non-SVG builds retain the original
bitmap path and acquire no new runtime dependency. A different requested
background or scaling ratio retains its own ordinary pixmap-cache entry.

The editable sources live in `lib/Xm/icons/dialog-*.svg` and are embedded in
`DialogIconsI.h`, avoiding runtime installation-path dependencies. Run
`scripts/generate-dialog-icons.py` after editing the artwork and use its
`--check` option to verify that the embedded representation is current.
The native `check-dialog-icons.c` fixture verifies all five default symbols,
cache reuse, doubled dimensions, background flattening and explicit custom
pixmap precedence. Run it in both SVG-enabled and non-SVG configurations.

```sh
python3 tests/automated/run-dialog-icons.py --build BUILD --out ABSOLUTE-OUT
python3 tests/automated/run-dialog-icons.py --build NON-SVG-BUILD \
  --out ANOTHER-ABSOLUTE-OUT --expect-unavailable
```
