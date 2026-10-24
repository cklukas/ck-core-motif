# Paned sash action parameters

The legacy `XmPaned` sash action accepts its keyboard increment as text from
the translation table.  That value is stored in a signed `short`; the old
`atoi` conversion could wrap an oversized or malformed parameter and move the
sash in the wrong direction or by an unexpected amount.

The action now uses a checked `strtol` conversion.  Values outside the
representable `short` range, missing values, and other malformed strings use
the existing default behavior: `LargeIncr` selects ten pixels and every other
zero/default value selects one pixel.  Valid signed values retain their prior
behavior and the public action/resource ABI is unchanged.

`tests/automated/check-paned-action-parameters.py` guards the conversion
contract and prevents the unchecked `atoi` path from returning.
