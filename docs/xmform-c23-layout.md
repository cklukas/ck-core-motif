# xmform C23 layout contracts

The `xmform` layout demonstration now declares its factory and internal form
helpers with typed C23 signatures.  Its letter-form input treats an empty
command-line string as a single blank form, and the distribution helper also
returns safely for an empty child list rather than dividing by zero.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmform-c23-layout.py
```
