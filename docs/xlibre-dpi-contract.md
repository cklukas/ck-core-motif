---
nav_exclude: true
---
# XLibre per-output DPI contract

The optional `--enable-xlibre-dpi` build enables a narrow probe for the XLibre
RandR output property named `DPI`. This is a scale hint, not a change to the
X11 coordinate system. XLibre defines 96 as the unscaled baseline; 144 and
192 therefore represent 1.5x and 2x hints.

The probe accepts a value only when all of these conditions hold:

- the server identifies itself as XLibre;
- RandR is present at version 1.2 or newer;
- the output property has type `INTEGER`, format 32, exactly one item and no
  trailing data; and
- the signed value is in the documented range 1 through 4096 DPI.

Missing, malformed, out-of-range and non-XLibre values are unsupported and do
not become a zero or guessed scale. The optional build is disabled by default;
requesting it without the Xrandr development header and library fails during
configuration rather than producing a partially working library.

This package only validates and reads the property. It intentionally does not
yet select an output for a shell, subscribe to RandR events or reflow realized
widgets. Those operations require a separate UI-thread transaction contract.
The installed fixture checks the parser's valid and invalid cases and verifies
that ordinary Xvfb falls back safely. Pass `--display` to the runner when a
real XLibre display is available; in that mode every reported output must
expose a valid `DPI` property.
