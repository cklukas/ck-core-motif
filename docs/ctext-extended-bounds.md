---
nav_exclude: true
---
# Extended compound-text segment bounds

Both extended-segment decoder paths now verify that the encoded segment length
fits between the current octet pointer and the input end before scanning or
advancing it.  Malformed input with a truncated segment therefore fails
cleanly instead of reading beyond the compound-text buffer.

`tests/automated/check-ctext-extended-bounds.py` protects both decoder paths.
