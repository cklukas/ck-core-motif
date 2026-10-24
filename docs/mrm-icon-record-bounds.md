# Mrm icon record bounds

Mrm icon argument values now validate the serialized icon size before passing it
to record storage. Null or invalid images and records larger than the format's
16-bit `MrmSize` limit are rejected instead of being truncated during
`GuaranteeSpace`; valid icons retain their existing copy behavior.

The source-level regression is `tests/automated/check-mrm-icon-record-bounds.py`.
