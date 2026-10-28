# DataField picture null results

Picture autofill and current-string generation can fail when their result
cannot be allocated.  The DataField verifier now rejects that result and
cleans up its temporary picture state before inspecting its length, avoiding a
null dereference while preserving the existing callback failure behavior.
