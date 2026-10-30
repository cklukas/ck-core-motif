---
nav_exclude: true
---
# FontSelector secondary-choice allocation

The secondary-choice controls now validate their merged arguments, button box,
and scaling/name toggles before callbacks or child pointers are published.
The optional encoding menu is added to the sensitivity list only when its
construction succeeds, so a partial FontSelector remains safe to use.
