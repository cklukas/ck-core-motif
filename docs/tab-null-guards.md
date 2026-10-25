# Tab value API null guards

`XmTabGetValues` and `XmTabSetValue` now reject null tab handles before
accessing tab fields. A null read returns `0.0`; a null write is a no-op.
Valid tab reads and writes are unchanged.
