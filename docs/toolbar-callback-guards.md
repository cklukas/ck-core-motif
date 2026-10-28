---
nav_exclude: true
---

# Toolbar callback guards

Toolbar validates its action callback closure before walking private action
storage. Foreign or stale callback data is ignored safely while normal
toolbar and menu-button activation remains unchanged.
