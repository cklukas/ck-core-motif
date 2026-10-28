---
nav_exclude: true
---

# Xft draw consumers

Text, label, compound-string, and clip consumers now handle a failed Xft draw
creation without dereferencing null. Existing rendering remains unchanged when
the drawable can be created successfully.
