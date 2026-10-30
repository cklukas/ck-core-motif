# Image-scaling depth input

`_XmPutScaledImage` now rejects an `XImage` with a non-positive `depth` before
converting it to an unsigned or `size_t` value.  Invalid depth values could
otherwise evade the plane-size checks for small rows and reach destination
image construction with an invalid X11 depth.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-scaling-depth-input.py
```
