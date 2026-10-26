# ColorObj property format validation

The desktop color cache stores its serialized pixel data as an 8-bit X11
property.  `ColorCachePropertyExists` now rejects properties returned with a
different format before treating `length` as a byte count and parsing the
payload.  This keeps malformed 16- or 32-bit properties from being truncated
or misinterpreted by the color parser.
