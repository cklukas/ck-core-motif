---
nav_exclude: true
---
# RatingControl

`XmRatingControl` is an additive integer rating from zero through
`XmNratingCount`. It renders outlined/filled segments rather than relying on
star artwork or color alone. Arrow keys and pointer selection choose a value;
`XmNratingAllowClear` permits selecting the current value again to clear it,
and `XmNratingReadonly` disables interaction while retaining visible focus and
value semantics. `XmNratingValueChangedCallback` reports the count and
committed value. Existing toggle-button groups are not involved.
