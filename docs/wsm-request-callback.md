# WSM request callback guard

`WSMProcessProtoTarget` now checks both the screen metadata and its registered
request callback before dispatching an incoming protocol request.  If either
is unavailable, the unpacked request is released and the conversion returns
`False` instead of dereferencing a null callback.

`tests/automated/check-wsm-request-callback.py` protects this lifecycle path.
