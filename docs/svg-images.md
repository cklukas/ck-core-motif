---
nav_exclude: true
---
# Optional SVG image loading

## Built-in action images

The named images `action_open`, `action_document`, `action_save`,
`action_print`, `action_undo`, `action_picture` and `action_archive` have a
24-pixel logical size. Request them through `XmGetScaledPixmap`; ratio zero
uses the requesting widget's display scale. SVG-enabled builds render the
coloured vector artwork at the target size. Other builds provide monochrome
outlines from the same geometry, using the requested foreground and background.

The application owns each acquired cache reference and must release it with
`XmDestroyPixmap`. Reacquire images when their target scale or colours change:
pixmaps contain flattened pixels and do not change when widget resources do.
Explicit `XmInstallImage` entries take precedence over these built-ins.

`scripts/generate-action-icons.py` maintains the SVG files and embedded vector
and bitmap data from a single geometry definition. Its `--check` option detects
stale generated assets. The native `run-action-icons.py --prefix PREFIX --out
OUTPUT` regression checks all seven images at 1x, 1.5x, 2x and 3x in both colour
directions, cache-reference ownership and installed-image override precedence.

## File loading

SVG support is enabled with `--enable-svg` and requires `librsvg-2.0 >= 2.52`
through `pkg-config`. It is disabled by default. A request for the feature
fails during configuration when the maintained rasterizer is unavailable;
the normal build does not gain a librsvg runtime dependency.

The loader is reached through the existing `XmGetPixmapByDepth` and
`XmGetScaledPixmap` image-cache paths. Files whose names end in `.svg`
(case-insensitive) are read into a bounded 16 MiB buffer and handed to
librsvg from memory. This avoids granting the renderer a base URI for external
file retrieval. Intrinsic dimensions must be finite, positive and no larger
than 8192 pixels per edge; the scaled output has the same limit. Rendering is flattened onto the requested X color
and requires a TrueColor or DirectColor visual; unsupported visuals and
malformed documents fail without creating a pixmap.

The existing pixmap cache still owns the resulting X pixmap. Its key retains
the display, depth, requested colors and scaling ratio, so repeated requests
reuse one entry while a different scale creates a separate entry. The
renderer receives the requested scale before rasterization. It draws directly
into the final pixel bounds, preserving vector edge detail instead of enlarging
an intrinsic-resolution bitmap. The bounds follow the existing rounded image
scaling convention, with a minimum of one pixel. Application-installed images
retain precedence and use ordinary bitmap scaling.

The installed regression checks intrinsic dimensions, scales from 0.01x through
3x, cache reuse, reference retirement, malformed input and oversized output.
Its native pixel-detail check rejects repeated 2x2 source-pixel blocks, which
caught the former bitmap-enlargement path. Run it as:

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

SVG-enabled libraries embed nine 40×40 vector symbols. The five message symbols are: `dialog_error`,
`dialog_information`, `dialog_question`, `dialog_warning`, and `dialog_working`.
The Periodic examples also use `dialog_template`, `dialog_file`,
`dialog_selection`, and `dialog_prompt`. Their outlined shapes, restrained red/blue/teal/amber/violet colors and bevel
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
cache reuse, doubled dimensions and vector detail in all nine symbols, automatic
scale changes through 3x, background flattening and explicit custom pixmap precedence. Run it in both SVG-enabled and non-SVG configurations.

```sh
python3 tests/automated/run-dialog-icons.py --build BUILD --out ABSOLUTE-OUT
python3 tests/automated/run-dialog-icons.py --build NON-SVG-BUILD \
  --out ANOTHER-ABSOLUTE-OUT --expect-unavailable
```

`XmGetScaledPixmap` is declared in the public `<Xm/Xm.h>` header. A ratio of
zero uses the widget's current display/print scale; a positive ratio requests
that explicit scale. Each successful call owns one cache reference, released
with `XmDestroyPixmap`. Periodic uses this loader for its nine dialog examples
in modern modes and releases those references when the icon gadgets are
destroyed. Classic/historical modes retain the UIL bitmaps; non-SVG builds also
retain them when the named vector artwork is unavailable.

Default MessageBox symbols follow palette changes without retaining a rectangle
from the old surface color. Explicit application pixmaps remain owned by the
application; see [MessageBox symbol palette](messagebox-symbol-palette.md).
