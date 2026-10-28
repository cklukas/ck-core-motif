---
nav_exclude: true
---

# RowActions callback guards

RowActions validates its activation callback closure before walking private
action storage. Foreign or stale callback data is ignored safely while normal
action invocation remains unchanged.
