---
nav_exclude: true
---
# List selection widget guards

The five public List selection APIs now reject non-List widgets before
entering application context or reading list-private selection state. Valid
selection and callback behavior is unchanged.

The source contract is checked by
`tests/automated/check-list-selection-widget-class.py`.
