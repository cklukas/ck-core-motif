# MWM catalog warning formatting

MWM diagnostics use catalog-provided templates together with one string value,
usually a menu, bitmap, or binding name. Catalog entries are data and can be
locally supplied, so they must not be interpreted as unrestricted variadic
`printf` formats.

`MWarning` now performs bounded formatting itself: `%s` substitutes its sole
string argument and `%%` emits a percent character. Any other percent sequence
is retained literally. The final diagnostic is always NUL-terminated within the
legacy `MAXWMPATH` buffer.

The focused source-contract regression is:

```sh
python3 tests/automated/check-mwm-warning-format.py
```
