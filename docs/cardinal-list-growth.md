# Cardinal-list resource conversion bounds

The cardinal-list resource converter grows its output array while parsing
values.  Capacity doubling now checks both the `int` limit and the resulting
allocation size before reallocation.  Invalid or unrepresentably large input
uses the existing cleanup path, preventing a wrapped capacity from causing an
undersized array.
