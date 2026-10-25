# WSM send failure paths

`WSMSendMessage` now checks widget realization before packing a request, so an
unrealized widget cannot leave a packed message allocated on an early return.
Packing failure and request-state allocation failure both release the packed
message and notify a non-null reply callback with a null reply. A null callback
is accepted safely for callers that only need the Boolean result.

`tests/automated/check-wsm-send-failures.py` protects these failure paths.
