---
nav_exclude: true
---
# List replacement widget guards

The five public List replacement APIs now reject non-List widgets before
entering application context or accessing list-private item and selection
state. Valid replacement behavior is unchanged.

The source contract is checked by
`tests/automated/check-list-replace-widget-class.py`.
