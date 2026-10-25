---
nav_exclude: true
---
# Mrm file-open filename validation

`UrmIdbOpenFileRead` requires a filename for the low-level file opener. It now
rejects a null name together with the existing output-pointer check, while
leaving the optional resolved-filename output buffer unchanged for callers that
do not need it.
