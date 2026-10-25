---
nav_exclude: true
---
# Divider label width

Divider preferred-width calculation now performs label-length multiplication
in a wide type and clamps it to the platform `Dimension` maximum. Long labels
no longer wrap to a narrow geometry request, while ordinary labels retain the
same sizing.

`tests/automated/check-divider-label-width.py` protects this calculation.
