# Mrm data-entry buffer postconditions

`Idb__DB_GetDataEntry` reads the buffer returned by the Mrm buffer manager for
both the initial record and each overflow-chain segment. It now verifies that
the buffer and its record storage are present after every acquisition before
forming record pointers, returning `MrmFAILURE` on an invalid postcondition.
