---
nav_exclude: true
---

# Spinner event and timer guards

Spinner event and timer callbacks can race widget destruction. The callbacks
now validate the widget type, event pointer, and timer data before touching
Spinner state; normal visibility synchronization and animation continue
unchanged for live widgets.
