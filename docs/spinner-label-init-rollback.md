# Spinner label initialization rollback

Spinner now checks the private copy of its initial label.  If `XmStringCopy`
fails, it installs an empty compound string and avoids measuring a NULL label,
keeping construction and geometry calculation safe under allocation pressure.

The focused regression is
`tests/automated/check-spinner-label-init-rollback.py`.
