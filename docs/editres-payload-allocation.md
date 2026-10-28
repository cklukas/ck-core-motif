---
nav_exclude: true
---

# Editres payload allocation

The Editres protocol helpers now check the terminator-inclusive wire-string
size against Xt's `Cardinal` width, as well as checking allocations for
decoded strings and widget-ID arrays before writing payload bytes. A rejected
size or failed allocation reports a protocol parse failure to the existing
cleanup path; zero-length widget lists remain valid.
SetValues value payloads are likewise checked before their terminator-inclusive
allocation.
The 32-bit protocol decoder also widens each 16-bit half before shifting, so
high-bit window IDs and stream lengths do not invoke signed-shift overflow.
Outgoing Editres strings are bounded to the protocol's 16-bit length field;
the writer never emits more bytes than it advertises, even if a malformed
resource name is unusually large.

The focused regression check is
`tests/automated/check-editres-payload-allocation.py`.

The string-output width check is
`tests/automated/check-editres-string-output.py`.
