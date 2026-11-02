# Font Selector demo C23 callback contracts

The generated Font Selector demo now gives each Xt callback its complete
`Widget`, client-data, and callback-data signature, and gives its form factory
and program entry point typed parameter lists.  Callback registrations can
therefore be checked by a current C compiler instead of relying on legacy
unprototyped function declarations.  The explanation selector also converts
the Xt client-data token through `intptr_t` before narrowing it to its small
enumerated value.

The focused source-contract check is:

```sh
python3 tests/automated/check-fontsel-demo-c23.py
```
