# XmString direction-component copy allocation

`XmStringGetNextComponent` now checks both direction-component copy buffers.
If a caller requests copied direction data and allocation fails, the iterator
marks its error and returns `XmSTRING_COMPONENT_END` instead of dereferencing
a null `XtNew` result.
