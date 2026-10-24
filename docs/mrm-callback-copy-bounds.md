# Mrm callback descriptor copy bounds

SetValues callback descriptors copied from Mrm records now reject null
pointers, negative counts, and descriptor-size arithmetic that cannot be
represented by `size_t` before allocating or copying. The copy-size variable
also remains native `size_t`, avoiding truncation through the legacy `MrmSize`
type. Valid callback descriptors retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-callback-copy-bounds.py`.
