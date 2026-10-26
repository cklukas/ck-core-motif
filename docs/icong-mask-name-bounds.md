# IconG mask-name allocation

When an icon needs an implicit mask, IconG derives a cache name from the image
name.  The name length is now checked for `size_t` overflow and Xt's
allocation-width limit before suffix space is added.  Missing or
unrepresentable names skip mask creation safely.

The focused source contract is checked by
`tests/automated/check-icong-mask-name-bounds.py`.
