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
    att = int(data['attenuation'])

    # Bandpass and Bandstop requires 2 cutoff points

    if t == 'bandpass' or t == 'bandstop':
        fc = [float(fc1), float(fc2)]
    else:
        fc = float(fc1)

    # Generate filter coefficients

    sos = signal.cheby2(N=n, rs=att, Wn=fc, btype=t, output='sos')

    # Create empty dictionary with DirectForm field

    out = {}
    out['DirectForm'] = []

    # Create as many fields of DirectForm as many the sos does in a json format

    for i in range(len(sos)):

        coeff = {
            "i" : i,
            "b0": np.round(sos[i][0]*(2**14)),
            "b1": np.round(sos[i][1]*(2**14)),
            "b2": np.round(sos[i][2]*(2**14)),
            "a0": np.round(sos[i][3]*(2**14)),
            "a1": np.round(sos[i][4]*(2**14)),
            "a2": np.round(sos[i][5]*(2**14))
        }

        out['DirectForm'].append(coeff)

    out['order'] = len(sos)
    # Serializing to json and sending to server

    print(json.dumps(out))
