---
nav_exclude: true
---

# Transfer parameter formats

`XmTransferSetParameters` stores parameters for the next selection request.
The X selection protocol supports only 8-, 16-, and 32-bit formats; the API
continues to treat zero as the existing 8-bit default and now ignores other
values before touching the transfer context.  This prevents malformed callers
from creating parameter requests whose length and payload interpretation do
not match the protocol; a non-null buffer with a `None` type is rejected as
well.

The regression check is
`tests/automated/check-transfer-parameter-format.py`.
