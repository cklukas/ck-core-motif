# WSM request callback guard

`WSMProcessProtoTarget` now checks both the screen metadata and its registered
request callback before dispatching an incoming protocol request.  If either
is unavailable, the unpacked request is released and the conversion returns
`False` instead of dereferencing a null callback.

Callback registration also tolerates a missing screen record, which can occur
when WSM display metadata allocation fails. The registration request is then
ignored safely and no partially initialized record is dereferenced.

`tests/automated/check-wsm-request-callback.py` protects this lifecycle path.
