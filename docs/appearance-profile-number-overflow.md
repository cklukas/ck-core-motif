# Appearance-profile numeric overflow

Appearance-profile dimension and metric values now reject `strtoul()`
conversion overflow before the result is narrowed to `Dimension`.  This keeps
out-of-range resource values from becoming a platform-dependent maximum size.

The focused source-contract check is:

```sh
python3 tests/automated/check-appearance-profile-number-overflow.py
```
