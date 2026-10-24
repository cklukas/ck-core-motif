# JPEG decoder error cleanup

The libjpeg error callback uses `longjmp`, including for failures after output
storage has been allocated.  The error branch now frees and clears
`image_data` after destroying the decompressor, so malformed or resource
exhaustion failures do not leak decoded pixels.

`tests/automated/check-jpeg-error-cleanup.py` protects this ownership rule.
