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

The internal shell-boundary policy is now defined without changing the public
Widget ABI. For a realized shell client area, the output containing its center
is selected. If the center is in no output (for example while a window is in a
gap), the nearest output rectangle is selected using the sum of horizontal and
vertical distances; equal distances use the lowest RandR output id. Empty or
invalid geometry returns `None`, allowing the caller to retain the existing
global-DPI fallback. All descendants of a shell will therefore have one
selected scale rather than mixing output scales.

This package still does not subscribe to RandR events or reflow realized
widgets. Those operations require a separate UI-thread transaction contract.
The installed fixture checks the parser, ordinary-Xvfb fallback and this
selection policy. Pass `--display` to the runner when a real XLibre display is
available; in that mode every reported output must expose a valid `DPI`
property.
