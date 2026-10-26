# MWM debug resource-dump paths

The debug `dump_resources` function writes one resource database per screen.
The generated `/tmp/dtwm.resources.<screen>` path now uses `snprintf` and is
checked for formatting failure or truncation before it is passed to Xrm. This
keeps the fixed compatibility buffer bounded without changing the established
debug file naming convention.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-debug-resource-path.py
```
