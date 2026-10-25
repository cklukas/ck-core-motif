# Shared etched shadow span bounds

`XmeDrawShadows` now computes the inner etched-shadow rectangle in signed
arithmetic and skips that second shadow when the outer rectangle is too small.
Tiny widgets therefore avoid wrapped `Dimension` spans while retaining the
existing two-pass etched rendering for normal sizes.

The source-level regression is `tests/automated/check-draw-shadow-span-bounds.py`.
