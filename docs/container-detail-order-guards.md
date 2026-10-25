---
nav_exclude: true
---
# Container detail-order allocation guards

Container now checks detail-order array allocations both while initializing a
widget and while applying resource changes. The byte products are checked
against both host `size_t` and Xt's `Cardinal` width. A failed or
unrepresentable copy clears the array and count, leaving the widget in a valid
default-order state instead of dereferencing a null pointer.
