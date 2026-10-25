# Legacy XmString constructor locking

`XmStringLtoRCreate` now delegates synchronization to the direction,
concatenation, and separator constructors it calls. The compatibility API no
longer holds the process lock across those nested operations.
