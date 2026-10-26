---
nav_exclude: true
---
# TextField wide insertion conversion

Compound-text and UTF-8 insertion now checks the converted length against
legacy integer and Xt allocation limits, and handles wide-buffer allocation
failure before calling `mbstowcs`.

Regression coverage: `tests/automated/check-textfield-wcs-insertion.py`.
