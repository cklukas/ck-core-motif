---
nav_exclude: true
---
# Input-method locale modifier bounds

`get_xim_info` applies the optional `XmNinputMethod` resource by prepending
`@im=` and calling `XSetLocaleModifiers`. The old code concatenated the
application string into a fixed `BUFSIZ` stack buffer and asserted after the
write.

The modifier string is now allocated from the input length with Xt ownership,
with an allocator-size check before construction. The existing locale modifier
value is preserved for representable inputs; an unrepresentable value is
ignored safely.

`tests/automated/check-input-method-modifier-bounds.py` guards this contract.
