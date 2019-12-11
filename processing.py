import numpy as np
import sys
import json
import scipy.signal as signal

for line in sys.stdin:

    # Deserializing json data coming from stdin initiated by python-shell at the server

    data = json.loads(line)
    t = str(data['FilterType'])
    n = int(data['Order'])
    fc1 = data['cutoff1']
    fc2 = data['cutoff2']

    # Bandpass and Bandstop requires 2 cutoff points

    if t == 'bandpass' or t == 'bandstop':
        fc = [float(fc1), float(fc2)]
    else:
        fc = float(fc1)

    # Generate filter coefficients

    sos = signal.butter(N=n, Wn=fc, btype=t, output='sos')

    # Create empty dictionary with DirectForm field

    out = {}
    out['DirectForm'] = []

    # Create as many fields of DirectForm as many the sos does in a json format

    for i in range(len(sos)):

        coeff = {
            "i" : i,
            "b0": sos[i][0],
            "b1": sos[i][1],
            "b2": sos[i][2],
            "a0": sos[i][3],
            "a1": sos[i][4],
            "a2": sos[i][5]
        }

        out['DirectForm'].append(coeff)

    # Serializing to json and sending to server

    print(json.dumps(out))
