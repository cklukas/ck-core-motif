# Text selection source guard

`SetSelection` now returns cleanly when a text source has not yet been
attached to a widget. Its saved selection endpoints also remain
`XmTextPosition` values while comparing the previous and new ranges, avoiding
truncation for very large sources. Normal realized-widget selection ownership
and highlighting are unchanged.

The focused source contract is checked by
`tests/automated/check-text-selection-source-guard.py`.
