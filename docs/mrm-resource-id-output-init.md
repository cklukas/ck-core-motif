# Mrm resource-id output initialization

`UrmIdbGetResourceId` now initializes its output to zero before validating the
file or allocating the next resource ID. The local RID is also initialized,
so an allocation failure cannot copy an indeterminate value to the caller.
Successful calls preserve the historical returned ID.
