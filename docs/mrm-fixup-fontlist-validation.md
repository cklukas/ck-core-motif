# Mrm font-list fixup validation

`Urm__CW_FixupValue` now rejects missing font-list buffers and counts above the
format limit for both legacy and current font-list layouts before copying or
swapping items. Valid font lists retain their existing compatibility path.

The source-level regression check is
`tests/automated/check-mrm-fixup-fontlist-validation.py`.
