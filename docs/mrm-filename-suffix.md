# Mrm hierarchy filename suffix validation

The hierarchy loader checks whether a UID filename already ends in `.uid`
before asking Xt to resolve it with a suffix. That check now rejects a null
filename and handles names shorter than four characters without indexing before
the string. Normal suffix resolution and fallback lookup remain unchanged.

The source-level regression is `tests/automated/check-mrm-filename-suffix.py`.
