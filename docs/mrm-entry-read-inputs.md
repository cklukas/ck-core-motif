# Mrm data-entry reader inputs

The data-entry readers now reject a null `IDBFile` before handling header
records or asking the buffer manager to load a data record. This keeps invalid
public calls on the normal failure path and avoids dereferencing the file
handle in lower-level helpers.
