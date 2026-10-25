# CascadeButton shadow redisplay guards

An armed `XmCascadeButton` draws a 3D shadow around its label. The old path
only checked realization, so an intermediate resource replacement or narrow
geometry could pass missing GCs or underflowed dimensions to `XmeDrawShadows`.

The armed path now requires both shadow GCs, non-zero shadow thickness, and a
rectangle larger than its highlight margins. Menu etched-in selection and all
other cascade behavior are unchanged when the widget is valid.

The focused source contract is checked by
`tests/automated/check-cascade-button-shadow-redisplay-guards.py`.
