---
nav_exclude: true
---

# WSM request counts

Request packing now rejects negative list counts, including nested WM-window
match-attribute counts.  Without this check, sizing loops could skip the
entries while `PackListNum` encoded the negative value as a large unsigned
count, creating an invalid wire message.
