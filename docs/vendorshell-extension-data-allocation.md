# VendorShell extension-data allocation

The VendorShell extension-object post-hook now allocates and initializes its
private `XmWidgetExtData` record before publishing it through
`_XmPushWidgetExtData`.  Both the extension record and its request-widget copy
are checked; failed allocation releases the partial record and returns without
exposing a null or incomplete extension to later hooks.

Successful extension setup and the existing VendorShell resource conversion
path are unchanged.
