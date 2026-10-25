---
nav_exclude: true
---
# Charset-table resource conversion bounds

The string-to-charset-table resource converter builds one allocation containing
the table and all copied charset names.  It now checks each name length, the
number of table entries, and the combined allocation size before allocating.
The combined size is also checked against Xt's `Cardinal` range and passed
explicitly at the allocator boundary.
Malformed or unrepresentably large resource values return conversion failure
after releasing the temporary input copy instead of wrapping a size calculation.
