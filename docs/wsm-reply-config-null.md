---
nav_exclude: true
---

# WSM reply configuration metadata

WSM request and reply sizing and packing now reject missing or incomplete
configuration metadata instead of dereferencing a null format record. This
protects state and registered-window replies when a display has not completed
configuration initialization or the requested format is unavailable, and
keeps request failures classified as internal packing errors.
