# MWM automation reply sizes

`GetAutomationData` now checks every fixed-size and menu-label contribution
before adding it to the reply size. The final size is also bounded to Xt's
`Cardinal` allocation width before the reply buffer is allocated. This avoids
wraparound from long menu labels or future protocol extensions and preserves
the existing reply layouts for representable requests.

The source contract is covered by
`tests/automated/check-mwm-automation-reply-size.py`.
