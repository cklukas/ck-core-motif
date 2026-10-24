# Mrm filter buffer lifetime

`Idb__DB_MatchFilter` obtains a data record and inspects its entry header to
apply group and type filters. The buffer must remain active for those reads;
decommitting before inspection leaves the routine holding a buffer that the
cache may reuse. The routine now validates and filters the entry first, then
decommits on every validation or filter exit path.
