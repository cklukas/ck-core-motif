---
nav_exclude: true
---

# MWM session workspace-string growth

WSM session save code joins inhabited workspace names into one temporary
string. The initial copy and subsequent space-separated growth now reject
lengths that exceed Xt's `Cardinal` allocation width, while preserving normal
session serialization and cleanup on failure.
