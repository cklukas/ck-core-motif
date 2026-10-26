# WSM selection-name formatting

The MWM workspace-management library derives one pair of X selection names
for each screen. The names are now formatted with `snprintf` into the existing
`BUFSIZ` buffer, keeping the multiscreen path bounded while preserving the
standard `WM_S<number>` and `WSM_S<number>` atom names.

The focused source contract is checked with:

```sh
python3 tests/automated/check-wsm-selection-format-bounds.py
```
