# MWM WSM screen-name initialization

When the WSM build replaces the default `fbk` screen name, it now formats the
screen number into a local, bounded buffer. The previous code wrote through the
general display-string buffer before that pointer was allocated, which could
crash MWM during multiscreen startup. The replacement checks the formatting
result, sizes the Xt allocation from the measured length, and copies only the
initialized bytes.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-screen-name-initialization.py
```
