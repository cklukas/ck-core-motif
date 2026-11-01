---
nav_exclude: true
---
# ToggleSwitch label initialization rollback

ToggleSwitch now checks the private copy of its initial label.  If
`XmStringCopy` fails, it installs an empty compound string instead of passing a
NULL label to geometry measurement and drawing.  This keeps construction safe
and preserves a usable control under allocation pressure. If allocating the
empty string also fails, measurement and rendering tolerate the absent label.

The focused regression is
`tests/automated/run-toggle-switch-failures.py`.

The native runner requires `--build` pointing to the configured build directory
and `--out` pointing to a fresh absolute results directory. It executes the real
widget rather than checking implementation text.
