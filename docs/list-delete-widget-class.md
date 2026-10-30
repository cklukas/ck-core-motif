---
nav_exclude: true
---
# List deletion widget guards

The six public List deletion APIs now reject non-List widgets before entering
application context or reading list-private mutation state. Valid deletion
behavior and historical position handling are unchanged.

The source contract is checked by
`tests/automated/check-list-delete-widget-class.py`.
