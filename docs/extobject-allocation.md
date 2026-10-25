# ExtObject allocation

`XmExtObject` initialization now checks the `XmWidgetExtData` allocation
before registering or filling the record. If allocation fails, initialization
returns without dereferencing a null extension record; successful extension
objects retain the existing initialization order and behavior.
