---
nav_exclude: true
---
# Mrm byte-swapped callback conversion status

Cross-endian widget records may contain callback descriptors either as an
argument value or as the widget creation callback.  Mrm now propagates a
failure from either descriptor conversion to the enclosing widget conversion.
Previously, an unsupported callback representation could make the descriptor
conversion fail while the widget record still reported `MrmSUCCESS` and was
subsequently treated as valid.  Valid callback descriptors and native-endian
widget records retain their existing behavior.

The focused source regression is
`tests/automated/check-mrm-widget-callback-swap-status.py`.
