# Image-cache scale-ratio inputs

`_XmGetScaledPixmap` now rejects negative and non-finite scale ratios before
cache lookup or image loading. A ratio of zero remains valid and continues to
select the print-resolution scaling mode.

The focused source contract is checked by
`tests/automated/check-image-scaling-ratio-input.py`.
