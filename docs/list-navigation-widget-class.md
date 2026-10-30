---
nav_exclude: true
---
# List navigation widget guards

The four public List viewport/navigation setters now reject non-List widgets
before entering application context or reading list-private geometry state.
Valid scrolling and item visibility behavior is unchanged.

The source contract is checked by
`tests/automated/check-list-navigation-widget-class.py`.
