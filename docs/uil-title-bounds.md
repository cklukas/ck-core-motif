# UIL listing title bounds

The listing title stores the module name and optional version in a fixed
132-byte buffer.  Module identifiers are not limited to the buffer's size, so
the old `sprintf` calls could overwrite adjacent compiler state while opening
or updating a listing.

Both title updates now use the buffer size with `snprintf`.  Long names and
versions are truncated in the listing while parsing and compilation continue
with the original symbol values and diagnostics.

`tests/automated/check-uil-title-bounds.py` protects the bounded formatting
contract.
