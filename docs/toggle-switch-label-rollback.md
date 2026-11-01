---
nav_exclude: true
---
# ToggleSwitch label rollback

`XmToggleSwitch` now copies a changed label before releasing the current
`XmString`. Allocation failure preserves the existing label, while NULL still
explicitly clears it.

The native allocation-failure regression is
`tests/automated/run-toggle-switch-failures.py`.

The native runner requires `--build` pointing to the configured build directory
and `--out` pointing to a fresh absolute results directory. It executes the real
widget rather than checking implementation text.
