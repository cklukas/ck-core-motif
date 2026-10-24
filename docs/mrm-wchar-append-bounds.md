# Mrm wide-string append bounds

Mrm wide-string record appends now count characters and compute byte lengths in
`size_t`, rejecting strings whose terminator-inclusive storage exceeds the
format's `MrmMaxResourceSize` before narrowing to `MrmSize`.

The source-level regression is `tests/automated/check-mrm-wchar-append-bounds.py`.
