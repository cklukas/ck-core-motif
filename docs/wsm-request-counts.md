---
nav_exclude: true
---

# WSM request counts

Request packing now rejects negative and `CARD16`-unrepresentable list counts,
including nested WM-window match-attribute counts.  Without these checks,
sizing loops could skip entries or `PackListNum` could truncate a large count,
creating an invalid wire message.
