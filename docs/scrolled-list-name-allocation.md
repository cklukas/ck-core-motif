---
nav_exclude: true
---
# Scrolled-list name allocation

`XmCreateScrolledList` now checks the `name`-plus-`SW` allocation for size
overflow and allocation failure before constructing the child window name.
An absent name selects its established `SW` fallback without passing a null
pointer to `XmStrlen`. Normal names are unchanged; an unrepresentable or
unavailable allocation returns `NULL` cleanly instead of writing through a
null pointer.

`tests/automated/check-scrolled-list-name-allocation.py` protects this contract.
