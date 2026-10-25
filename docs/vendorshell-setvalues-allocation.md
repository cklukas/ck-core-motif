# VendorShell SetValues allocation

The VendorShell `SetValues` prehook now allocates both secondary-object
snapshots before publishing its extension-data record.  Failed `_XmExtObjAlloc`
calls release any partial snapshots and the record, avoiding null `memcpy`
targets and leaving the hook unregistered.  Successful updates retain the
existing old/request snapshot and resource-conversion behavior.
