---
nav_exclude: true
---
# ProgressBar label initialization rollback

ProgressBar now checks the private copy of its initial label.  If
`XmStringCopy` fails, it installs an empty compound string and avoids
measuring a NULL label, keeping construction and geometry calculation safe
under allocation pressure.

The focused regression is
`tests/automated/check-progressbar-label-init-rollback.py`.
