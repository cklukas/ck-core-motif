# Charset-table resource conversion bounds

The string-to-charset-table resource converter builds one allocation containing
the table and all copied charset names.  It now checks each name length, the
number of table entries, and the combined allocation size before allocating.
Malformed or unrepresentably large resource values return conversion failure
after releasing the temporary input copy instead of wrapping a size calculation.
