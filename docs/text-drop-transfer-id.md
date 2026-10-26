---
nav_exclude: true
---
# Text drop transfer IDs

Text and TextField drop-transfer callbacks now reject callbacks without a
transfer ID before invoking transfer completion or requesting another target.
This keeps malformed and teardown-time callbacks from reaching the transfer
API with a null context.

Regression coverage: `tests/automated/check-text-drop-transfer-id.py`.
