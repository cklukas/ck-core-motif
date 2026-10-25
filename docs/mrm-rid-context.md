# Mrm RID-resource context validation

`UrmIdbGetRIDResource` fills a caller-provided resource context and reads its
filters after the data lookup. It now rejects a null context before entering the
file and RID lookup path, preventing a null dereference while preserving valid
RID-resource behavior.
