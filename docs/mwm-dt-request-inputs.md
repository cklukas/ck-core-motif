# MWM DT request inputs

`GetDtWmRequest` now returns immediately when its screen data or either output
pointer is missing. For valid calls it initializes `*pszReq` and `*pmore` to a
safe empty result before reading the property, while preserving the existing
request extraction behavior.

An empty or null payload is also rejected before extraction.  That prevents a
type-correct but zero-item X11 property from being converted into a synthetic
empty request and reaching the desktop-request parser.

The bounded X11 fetch must also be complete. A request property with bytes
remaining is discarded and deleted instead of allowing a truncated command to
reach the parser or remain queued indefinitely.

The focused source contract is covered by
`tests/automated/check-mwm-dt-request-inputs.py` and
`tests/automated/check-mwm-dt-request-empty-property.py`.
