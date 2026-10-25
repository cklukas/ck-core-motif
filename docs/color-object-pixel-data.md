# ColorObj pixel-data allocation

The ColorObj selection path copies a server-provided pixel-data property into
an Xt-owned, NUL-terminated buffer before parsing it. The allocation is now
checked before the copy; failure releases the X property buffer and returns
without entering the parser with a null input.
