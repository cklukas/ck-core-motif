"""Inject libdbus construction/send failures; verify text-baseline recovery."""
import json
import os
import pathlib
import select
import subprocess
import sys
import time
import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

DBusGMainLoop(set_as_default=True)
fixture, interposer, receipt = map(pathlib.Path, sys.argv[1:4])
records = []
passed = False
process = None
try:
    address = str(dbus.SessionBus().get_object('org.a11y.Bus', '/org/a11y/bus').GetAddress(dbus_interface='org.a11y.Bus'))
    runtime = pathlib.Path(os.environ['XDG_RUNTIME_DIR']).resolve()
    assert address.startswith('unix:path=' + str(runtime) + '/'), (address, runtime)
    bus = dbus.bus.BusConnection(address)
    context = GLib.MainContext.default()
    for message in (1, 2):
        for step in range(1, 11):
            preload = [str(interposer.resolve())]
            runtime = os.environ.get('XM_TEST_TEXT_SANITIZER_PRELOAD')
            if runtime:
                runtime = pathlib.Path(runtime)
                assert runtime.is_absolute() and runtime.is_file(), 'Explicit sanitizer runtime is unavailable'
                preload.insert(0, str(runtime.resolve()))
            env = dict(os.environ, LD_PRELOAD=':'.join(preload),
                       XM_TEST_TEXT_FAIL_STEP=str(step), XM_TEST_TEXT_FAIL_MESSAGE=str(message))
            process = subprocess.Popen([str(fixture.resolve())], env=env,
                stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            def response():
                assert select.select([process.stdout], [], [], 30)[0], 'fixture timeout'
                line = process.stdout.readline()
                assert line, 'fixture exited'
                return json.loads(line)
            nodes = response()
            events = []
            def changed(detail, first, second, payload, properties):
                events.append((str(detail), int(first), int(second), str(payload)))
            receiver = bus.add_signal_receiver(changed, signal_name='TextChanged',
                dbus_interface='org.a11y.atspi.Event.Object', bus_name=nodes['bus'],
                path='/org/a11y/atspi/accessible/' + str(nodes['custom']))
            def run(command):
                events.clear()
                process.stdin.write(command + '\n'); process.stdin.flush()
                assert response()['done']
                bus.get_object(nodes['bus'], '/org/a11y/atspi/accessible/root', introspect=False).GetRole(dbus_interface='org.a11y.atspi.Accessible')
                deadline = time.monotonic() + .15
                while time.monotonic() < deadline:
                    while context.pending(): context.iteration(False)
                    time.sleep(.001)
                return list(events)
            first = run('custom')
            expected = [('delete', 0, 7, 'initial')] if (step == 10 and message == 2) else []
            assert first == expected, (message, step, first)
            assert run('recover') == [], (message, step, 'failed batch baseline remained known')
            assert run('unicode') == [('delete', 1, 9, 'eplacemen'), ('insert', 1, 2, 'β😀')]
            process.stdin.write('quit\n'); process.stdin.flush()
            stdout, stderr = process.communicate(timeout=10)
            assert process.returncode == 0, (message, step, stderr)
            assert 'text-event injected failure step ' + str(step) in stderr, (message, step, stderr)
            receiver.remove()
            records.append(dict(message=message, failed_step=step, first_events=first,
                                stderr=stderr, recovered=True))
            process = None
    passed = True
finally:
    if process and process.poll() is None:
        process.terminate(); process.wait(timeout=5)
    receipt.write_text(json.dumps(dict(passed=passed, records=records,
        scope='test interposition into actual libdbus construction/send; no allocator-wide OOM claim'), indent=2) + '\n')
