import sys
import signal
import os
import jack
import threading
import numpy as np

import vamp

if sys.version_info < (3, 0):
    signal.signal(signal.SIGINT, signal.SIG_DFL)
else:
    pass

argv = iter(sys.argv)
defaultclientname = os.path.splitext(os.path.basename(next(argv)))[0]
clientname = next(argv, defaultclientname)
servername = next(argv, None)

client = jack.Client(clientname, servername=servername)

if client.status.server_started:
    print('JACK server started')
if client.status.name_not_unique:
    print('unique name {0!r} assigned'.format(client.name))

event = threading.Event()

@client.set_process_callback
def process(frames):

    #assert frames == client.blocksize
    #assert (len(l_f) == len(r_f))

    l_f = client.inports[0].get_array()
    r_f = client.inports[1].get_array()

    '''
    l_f = np.frombuffer(l.get_buffer(), dtype=float)
    r_f = np.frombuffer(r.get_buffer(), dtype=float)
    '''

    m_f = np.add(l_f, r_f)
    m_f = np.true_divide(m_f, 2.0)

    try:
        data = vamp.collect(m_f, client.samplerate, "vamp-libxtract:loudness")
        vector = data['vector']
        L = vector[1]

        if L > 0.0:
            data = vamp.collect(m_f, client.samplerate, "pyin:pyin")
            L = data['list']
            print L[1]['values']

    except:
        pass

@client.set_shutdown_callback
def shutdown(status, reason):
    event.set()

for number in 1, 2:
    client.inports.register('input_{0}'.format(number))

with client:
    '''
    capture = client.get_ports(is_physical=True, is_output=True)
    if not capture:
        raise RuntimeError('No physical capture ports')

    for src, dest in zip(capture, client.inports):
        client.connect(src, dest)
    '''

    client.connect("system:monitor_1", client.inports[0])
    client.connect("system:monitor_2", client.inports[1])

    try:
        event.wait()
    except KeyboardInterrupt:
        print('\nInterrupted by user')
