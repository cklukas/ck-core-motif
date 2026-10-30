---
nav_exclude: true
---
# PNG conversion failure cleanup

The PNG converter now treats 8-bit indexed visuals as an explicit unsupported
format instead of returning success with unwritten pixels.  Whenever pixel
conversion fails, the newly allocated `XImage` is destroyed and the caller's
pointer is cleared before the error is returned.  Normal 16/24/32-bit output
is unchanged, while failed loads no longer leak or expose invalid image data.

The regression check is
`tests/automated/check-png-error-image-cleanup.py`.
