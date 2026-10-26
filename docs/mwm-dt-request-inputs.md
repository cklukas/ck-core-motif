# MWM DT request inputs

`GetDtWmRequest` now returns immediately when its screen data or either output
pointer is missing. For valid calls it initializes `*pszReq` and `*pmore` to a
safe empty result before reading the property, while preserving the existing
request extraction behavior.

The focused source contract is covered by
`tests/automated/check-mwm-dt-request-inputs.py`.
