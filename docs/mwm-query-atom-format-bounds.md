# MWM query-atom formatting

For each managed screen, MWM creates a `_MOTIF_WM_QUERY_<screen>` atom name.
The name is now formatted with `snprintf` into the existing compatibility
buffer and checked for failure or truncation before interning the atom. This
keeps malformed or unexpectedly large screen identifiers from reaching Xlib as
an unterminated name.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-query-atom-format-bounds.py
```
