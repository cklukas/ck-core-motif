# Xft colour cache

Xft draws with RGB values, and turning a pixel into one takes a server round
trip (`XQueryColor`). `_XmXftGetXftColor` keeps the answers so that renditions
and lists do not ask again for every draw. The cache was one process-wide array
that grew without limit, was never released and compared the pixel alone: a
second display, or a colormap other than the default one, was given the colours
of whichever lookup came first.

The cache is now organised as follows.

- `_XmXftGetXftColor(display, colormap, pixel)` takes the colormap the pixel
  belongs to. `XmList` passes the colormap of the widget; renditions, which have
  no widget, pass the default colormap of the default screen.
- Each display has its own table of 128 entries keyed by colormap and pixel,
  replaced in turn when it is full, so the memory is bounded.
- The table is released when the display is closed, through a close-display
  hook registered with Xlib the first time something is cached for the display.
  `_XmXftColorCacheCount(display)` reports the number of cached colours for
  a display (all displays when it is NULL).
- Only colormaps whose cells cannot change are cached: the default colormap of
  a screen whose default visual is TrueColor, StaticColor or StaticGray. The
  cells of any other colormap can be rewritten by any client, so those lookups
  always ask the server.
- The list of tables is protected by the process lock, which is not held while
  the server answers.
- A failed `XQueryColor` leaves the colour black instead of using
  uninitialised components, and `_XmXftDrawString2` takes its colour from the
  same cache and gives up when the graphics context cannot be read.

## Validation

`tests/automated/run-xft-color-cache.py PREFIX RESULTS` starts a 24-bit
TrueColor and an 8-bit PseudoColor virtual X server and runs
`tests/automated/check-xft-color-cache.c`, built with the same sanitizers as
the toolkit, against both. It checks that a repeated lookup does not reach the
server, that the same pixel number gives each display its own colour, that
writable and non-default colormaps are not cached and follow changes to their
cells, that the table stays within 128 entries while giving correct answers,
and that closing a display releases its colours. `--old-api` builds the program
for the earlier cache, which returns the colour of the first display for the
second.
