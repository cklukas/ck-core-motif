#!/usr/bin/env python3
"""Observe real IBus/m17n XIM composition in native Text and TextField.

Run inside an owned X server and dbus-run-session with fresh XDG state. User
desktop settings are not changed. XTest sends real key events; no XIM callback
is fabricated.
"""
import datetime
import hashlib
import json
import os
from pathlib import Path
import select
import signal
import subprocess
import sys
import time
import uuid
from ibus_engine import select_global_engine

binary = Path(sys.argv[1]).resolve(strict=True)
out = Path(sys.argv[2]).absolute()
out.mkdir()
processes = []
observations = []
receipt = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
    provider="IBus XIM + m17n Vietnamese Telex, Hindi ITRANS and Unicode", observations=observations,
    success=False, scope="These exact engines on the recorded provider; broader IM interoperability is unqualified.")
began = time.monotonic()
env = dict(os.environ, LC_ALL="C.UTF-8", XMODIFIERS="@im=ibus", GIO_USE_VFS="local")
for k in ("IBUS_USE_PORTAL", "GTK_IM_MODULE", "QT_IM_MODULE"):
    env.pop(k, None)
# An explicit unique abstract endpoint avoids both ambient bus discovery and
# filesystem UNIX-socket path limits in deeply nested validation directories.
env["IBUS_ADDRESS"] = "unix:abstract=motif-xim-" + uuid.uuid4().hex
receipt["ibus_address"] = env["IBUS_ADDRESS"]
receipt["engine_api_helper_sha256"] = hashlib.sha256(Path(__file__).with_name("ibus_engine.py").read_bytes()).hexdigest()
for k in ("CONFIG_HOME", "CACHE_HOME", "DATA_HOME", "STATE_HOME", "RUNTIME_DIR"):
    path = out / k.lower()
    path.mkdir(mode=0o700)
    env["XDG_"+k] = str(path)

def command(args):
    r = subprocess.run(args, env=env, text=True, capture_output=True, timeout=10)
    if r.returncode:
        raise RuntimeError(f"command failed {args}: {r.stderr}")
    return r.stdout

def launch(args, name, **kw):
    p = subprocess.Popen(args, env=env, stderr=(out/(name+".stderr")).open("w"),
                         start_new_session=True, **kw)
    processes.append(p)
    return p

def stop(p):
    try:
        os.killpg(p.pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    try:
        p.wait(timeout=4)
    except subprocess.TimeoutExpired:
        os.killpg(p.pid, signal.SIGKILL)
        p.wait(timeout=4)

def read(p):
    if not select.select([p.stdout], [], [], 6)[0]:
        raise RuntimeError("native fixture reply timeout")
    line = p.stdout.readline()
    if not line:
        raise RuntimeError(f"native fixture exited {p.poll()}")
    return json.loads(line)

def snap(p, name):
    p.stdin.write("snapshot\n"); p.stdin.flush()
    value = read(p)
    for k in ("text", "field"):
        value[k] = bytes.fromhex(value[k]).decode("utf-8")
    observations.append(dict(name=name, **value))
    return value

def wait(p, name, predicate):
    deadline = time.monotonic()+6
    while True:
        value = snap(p, name)
        if predicate(value):
            return value
        if time.monotonic() >= deadline:
            raise AssertionError(f"{name}: {value}")
        time.sleep(.05)

def key(keysym):
    command(["xdotool", "key", "--clearmodifiers", keysym])
    time.sleep(.12)

def focus(ids, name):
    command(["xdotool", "windowfocus", str(ids["shell"])])
    command(["xdotool", "mousemove", "--window", str(ids[name+"_window"]), "200", "15", "click", "1"])
    time.sleep(.15)

def start_provider(index):
    p = launch(["ibus-daemon", "--address="+env["IBUS_ADDRESS"], "--xim", "--panel=disable", "--config=disable",
                "--emoji-extension=disable", "--cache=none"], "ibus-"+str(index),
               stdout=(out/("ibus-"+str(index)+".stdout")).open("w"))
    # Loading registry/engines is asynchronous; qualify the actual selected name.
    engine("m17n:vi:telex")
    return p

def engine(name):
    deadline = time.monotonic()+12
    while True:
        admission = select_global_engine(env["IBUS_ADDRESS"], name)
        observations.append(dict(name="engine-api", result=admission))
        if admission["matched"]:
            observations.append(dict(name="engine", engine=name, context_mapping_proven=False))
            return
        if time.monotonic() >= deadline:
            raise RuntimeError("provider did not select engine "+name)
        time.sleep(.1)

def client(policy):
    if policy == "widget": env["XM_XIM_PER_WIDGET"] = "1"
    else: env.pop("XM_XIM_PER_WIDGET", None)
    p = launch([str(binary)], "client-"+policy, stdin=subprocess.PIPE,
               stdout=subprocess.PIPE, text=True, bufsize=1)
    ids = read(p)
    observations.append(dict(name="identity-"+policy, **ids))
    return p, ids

def shutdown_client(p):
    p.stdin.write("quit\n");p.stdin.flush()
    if p.wait(timeout=5): raise RuntimeError("fixture did not exit cleanly")

signal.signal(signal.SIGTERM, lambda *_: (_ for _ in ()).throw(SystemExit(143)))
try:
    receipt["packages"] = command(["dpkg-query", "-W", "-f=${binary:Package} ${Version}\n",
        "ibus", "ibus-m17n", "ibus-data", "libm17n-0", "m17n-db", "libx11-6", "xdotool"])
    receipt["input_sources"] = {str(p):hashlib.sha256(p.read_bytes()).hexdigest()
        for p in map(Path, ["/usr/share/m17n/vi-telex.mim", "/usr/share/m17n/vi-base.mim",
                            "/usr/share/m17n/unicode.mim", "/usr/share/m17n/hi-itrans.mim",
                            "/usr/share/ibus/component/m17n.xml", "/usr/bin/ibus-daemon",
                            "/usr/libexec/ibus-x11", "/usr/libexec/ibus-engine-m17n"])}
    # Initially unavailable service must become usable without recreating widgets.
    p, ids = client("shell")
    wait(p,"late-provider-absent",lambda s:not s["xim"])
    provider = start_provider(0)
    wait(p,"late-provider-attached",lambda s:s["xim"])
    receipt["xim_servers"] = command(["xprop","-root","XIM_SERVERS"])
    for policy in ("shell", "widget"):
        if policy == "widget": p, ids = client(policy)
        focus(ids,"field")
        for name, pf, cursor, length, changes in (("field","fp","fc","fl","fv"),("text","tp","tc","tl","tv")):
            focus(ids,name)
            committed_changes = snap(p, "committed-baseline-"+policy+"-"+name)[changes]
            key("a");key("w")
            wait(p,policy+"-"+name+"-nonascii-preedit",lambda s:s[pf] and s[name]=="ă" and s[length]==1 and s[cursor]==1 and s[changes]==committed_changes)
            key("s");key("space")
            wait(p,policy+"-"+name+"-commit",lambda s:not s[pf] and s[name]=="ắ " and s[cursor]==2 and s[length]==2 and s[changes]==committed_changes+1)
            key("a");key("w")
            wait(p,policy+"-"+name+"-cancel-start",lambda s:s[pf] and s[name]=="ắ ă")
            key("BackSpace")
            wait(p,policy+"-"+name+"-cancelled",lambda s:not s[pf] and s[name]=="ắ " and s[cursor]==2 and s[changes]==committed_changes+1)
        # The provider can return a commit or cancellation during focus/reset.
        # Require exact contents and a matching committed-value callback count;
        # no transient text may survive either outcome.
        if policy == "shell":
            before_focus = snap(p, "focus-committed-baseline")
            key("a");key("w")
            wait(p,"focus-before",lambda s:s["tp"] and s["text"]=="ắ ă")
            p.stdin.write("focus-field\n");p.stdin.flush();read(p)
            time.sleep(.2)
            wait(p,"focus-transferred",lambda s:not s["tp"] and s["text"]=="ắ " and s["field"]=="ắ ă" and s["fc"]==3)
            key("Right")
            settled = wait(p,"focus-reset-settled",lambda s:not s["tp"] and not s["fp"])
            assert settled["text"] == "ắ " and settled["tv"] == before_focus["tv"], settled
            if settled["field"] == "ắ ă":
                assert settled["fc"] == 3 and settled["fv"] == before_focus["fv"] + 1, settled
                observations.append(dict(name="focus-outcome", outcome="provider-commit"))
                key("BackSpace")
                wait(p,"focus-committed-edit",lambda s:s["field"]=="ắ " and s["fc"]==2)
            else:
                assert settled["field"] == "ắ " and settled["fc"] == 2 and settled["fv"] == before_focus["fv"], settled
                observations.append(dict(name="focus-outcome", outcome="provider-cancellation"))
        focus(ids,"field");key("a");key("w")
        wait(p,"service-loss-active-"+policy,lambda s:s["fp"] and s["field"].endswith("ă"))
        stop(provider)
        wait(p,"service-loss-clean-"+policy,lambda s:not s["xim"] and not s["fp"] and s["field"]=="ắ " and s["text"]=="ắ ")
        provider=start_provider(1 if policy=="shell" else 2)
        wait(p,"service-reconnected-"+policy,lambda s:s["xim"])
        key("a");key("w");key("s");key("space")
        wait(p,"post-restart-commit-"+policy,lambda s:s["field"]=="ắ ắ " and s["fc"]==4)
        # Real Hindi composition commits two scalars in one grapheme; the
        # Unicode engine adds CJK and supplementary scalars through actual XIM.
        p.stdin.write("clear\n");p.stdin.flush();read(p)
        for name,pf,cur,ln in (("field","fp","fc","fl"),("text","tp","tc","tl")):
            focus(ids,name)
            engine("m17n:hi:itrans")
            key("k");key("i")
            wait(p,"hindi-preedit-"+policy+"-"+name,lambda s:s[pf] and s[name]=="कि" and s[ln]==2)
            key("space")
            wait(p,"hindi-commit-"+policy+"-"+name,lambda s:not s[pf] and s[name]=="कि " and s[ln]==3 and s[cur]==3)
            key("BackSpace")
            engine("m17n:t:unicode")
            for code in ("4e2d","1f642"):
                key("ctrl+u")
                for digit in code:key(digit)
                key("space")
                snap(p,"unicode-commit-"+policy+"-"+name+"-"+code)
            wait(p,"unicode-scalars-"+policy+"-"+name,lambda s:s[name]=="कि中🙂" and s[ln]==4 and s[cur]==4 and not s[pf])
            key("Left")
            wait(p,"emoji-boundary-"+policy+"-"+name,lambda s:s[cur]==3)
            key("Left")
            wait(p,"cjk-boundary-"+policy+"-"+name,lambda s:s[cur]==2)
            key("Left")
            wait(p,"combining-boundary-"+policy+"-"+name,lambda s:s[cur]==0)
            key("Right");key("BackSpace")
            wait(p,"grapheme-delete-"+policy+"-"+name,lambda s:s[name]=="中🙂" and s[ln]==2 and s[cur]==0)
        shutdown_client(p)
        engine("m17n:vi:telex")
    receipt["success"]=True
finally:
    for process in reversed(processes):stop(process)
    receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),seconds=time.monotonic()-began,
                   process_exits=[dict(pid=p.pid,args=p.args,exit=p.poll()) for p in processes])
    (out/"receipt.json").write_text(json.dumps(receipt,ensure_ascii=False,indent=2)+"\n")
print("Actual IBus/m17n XIM service fixture passed:",out/"receipt.json")
