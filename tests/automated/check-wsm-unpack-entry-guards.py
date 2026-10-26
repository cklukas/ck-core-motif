from pathlib import Path

source = Path("clients/mwm/WmWsmLib/pack.c").read_text(encoding="utf-8")

request_start = source.index("_WSMUnpackRequest")
request_end = source.index("/* _WSMUnpackRequest */", request_start)
request = source[request_start:request_end]
assert "if (request == NULL)" in request
assert "memset(request, 0, sizeof(*request));" in request
assert "type != WSM_CONFIG_FMT" in request

reply_start = source.index("_WSMUnpackReply")
reply_end = source.index("/* _WSMUnpackReply */", reply_start)
reply = source[reply_start:reply_end]
assert "if (reply == NULL)" in reply
assert "memset(reply, 0, sizeof(*reply));" in reply
assert "type != WSM_SET_STATE" in reply
print("PASS WSM unpackers guard destinations and empty-message payloads")
