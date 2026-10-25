---
nav_exclude: true
---
# Font selector size-list allocation

Font selector size and alternate-family list refreshes now check their
temporary `XmString` arrays before filling them. Allocation failure returns
without updating the corresponding list, preventing null-pointer writes while
preserving normal behavior for valid lists.
