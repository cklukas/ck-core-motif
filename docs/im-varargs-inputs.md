# Input-method varargs setter input contract

`XmImVaSetFocusValues` and `XmImVaSetValues` now return immediately when
their widget argument is `NULL`, before application-context lookup or varargs
processing.  Valid varargs updates are unchanged.
