# Image color-release callback

The XPM color-release callback decrements Motif's shared color-cache
references and is intentionally best-effort: the XPM reader does not consume a
failure result from this callback.  The implementation now makes that
contract explicit by discarding each internal release result and returning
zero after validating its inputs.  It no longer carries a dead status value
that suggested a result was propagated.

The callback contract is covered by
`tests/automated/check-image-free-colors-callback.py`.
