---
nav_exclude: true
---

# WSM atom initialization rollback

Display metadata now checks the status returned by `XInternAtoms`.  If atom
initialization fails, the newly allocated record is removed from the display
list (or the global head) and freed, preventing a partially initialized record
from treating `None` atoms as valid protocol targets.
