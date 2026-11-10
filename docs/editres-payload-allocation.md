---
nav_exclude: true
---

# Editres payload allocation

Strings, widget paths and SetValues values are allocated only after the number
of bytes they announce has been checked against the bytes left in the request.
A rejected size or a failed allocation reports a protocol parse failure to
the common cleanup path. The 32-bit decoder widens each half before shifting.
Outgoing Editres strings are bounded to the protocol's 16-bit length field;
the writer never emits more bytes than it advertises.

The focused regression checks are
`tests/automated/check-editres-payload-allocation.py`,
`tests/automated/check-editres-string-output.py` and
`tests/automated/check-editres-error-message-bounds.py`.
See [Editres protocol handler](editres-wire-protocol.md).
