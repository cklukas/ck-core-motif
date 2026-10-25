# ColorSelector list allocation

When the color selector rebuilds its list widget, the temporary `XmString`
pointer array is checked against both host `size_t` and Xt `Cardinal` byte
limits before allocation. An unrepresentable or failed allocation leaves the
existing color data cleanup path intact.
