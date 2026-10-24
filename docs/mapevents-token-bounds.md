# MapEvents token bounds

Translation-table parsing accepts key symbols, modifier names, and event types
from application resources. The parser previously copied each token into a
100-byte stack array with `strncpy` and no length check.

The parser now rejects a token that cannot fit, reports the translation as
invalid through its existing status path, and keeps all valid token behavior
unchanged. This prevents malformed or unexpectedly long resource strings from
corrupting the parser stack.

`tests/automated/check-mapevents-token-bounds.py` guards the bounded-copy
contracts.
