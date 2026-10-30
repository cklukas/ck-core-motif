---
nav_exclude: true
---

# BulletinBoard dialog-title getter guard

The synthetic `XmNdialogTitle` getter now validates the BulletinBoard widget
and output slot before reading the dialog-title field. Invalid callback inputs
return a deterministic null value; valid reads retain the existing copied
`XmString` ownership contract.

The focused source-contract check is
`python3 tests/automated/check-bulletin-dialog-title-guard.py`.
