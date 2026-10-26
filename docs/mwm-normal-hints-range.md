# MWM `WM_NORMAL_HINTS` range validation

The property reader stores X11 `long` property fields in Motif's signed
`int`-sized `SizeHints` structure.  It now rejects a malformed property when
any field cannot be represented, before narrowing conversions occur.  The
reader returns the same unknown/default-hints state used for an unavailable or
invalid property, preserving normal ICCCM behavior while preventing wrapped
geometry and resize constraints. Legacy ICCCM R2 records are checked only
across their 15-field payload; newer base-size and gravity fields are checked
only when the full current-format record is present.
