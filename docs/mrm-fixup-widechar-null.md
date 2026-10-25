# Mrm wide-character fixup null entries

`Urm__CW_FixupValue` now rejects a null serialized wide-character entry before
reading its count or converting its bytes. Valid wide-character values retain
the existing allocation and multibyte conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fixup-widechar-null.py`.
