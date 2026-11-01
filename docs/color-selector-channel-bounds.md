---
nav_exclude: true
---
# ColorSelector channel bounds

`XmColorSelector` keeps RGB slider state in `int` fields but exposes it as
8-bit RGB text and 16-bit X11 color channels. Incoming slider callback values
are now normalized to 0–255, and the same normalization is applied before
formatting a hex name or constructing `XColor`. This keeps malformed or custom
scale callbacks from producing an invalid color string or wrapping a channel.

Filename-change detection also now converts `strcmp` to an explicit Boolean,
so every non-equal string is reliably treated as changed.

The focused source contract is covered by
`tests/automated/check-color-selector-channel-bounds.py`.
