# Color object property input bounds

Color objects receive serialized pixel sets from another X11 owner. The old
parser treated the property as an unbounded C string and repeatedly called
`sscanf`, so truncated or malformed properties could read beyond the received
payload and update colors from uninitialized data.

The parser now copies exactly the received length into a terminated scratch
buffer, validates each numeric record, and rejects malformed payloads before
updating state. Empty properties and invalid version markers are released
safely.

`tests/automated/check-color-object-property-input.py` guards these contracts.
