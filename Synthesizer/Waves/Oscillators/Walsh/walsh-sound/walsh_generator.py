import numpy as np
import scipy.io.wavfile
import os
from itertools import chain
import matplotlib.pyplot as plt
import tqdm

def walsh_transform(x):
    def flatten(l):
        return list(chain.from_iterable(l))

    n = len(x)
    if(n == 1):
        return x
    else:
        x0 = walsh_transform( [(a+b) for a, b in zip(x, x[n//2:])] )
        x1 = walsh_transform( [(a-b) for a, b,in zip(x, x[n//2:])] )
        return flatten(zip(x0, x1))

if __name__ == '__main__':
    resolution = 2**16
    max_n = 2**8
    samplerate = 44100
    gain = (2**14)
    wavdir = 'wav'
    pltdir = 'plt'

    os.makedirs(wavdir, exist_ok=True)
    os.makedirs(pltdir, exist_ok=True)

    fig = plt.figure(figsize=(12.0, 2.0))

    for n in tqdm.tqdm(range(max_n)):
        # translate to gray code
        g = n ^ (n>>1)

        # walsh spectrum
        spectrum = np.zeros((resolution,))
        spectrum[g] = 1

        # inverse transform
        write_frames = np.array( walsh_transform(spectrum) )*gain

        # write
        scipy.io.wavfile.write(
            os.path.join(wavdir, f'walsh_{n:04d}_{resolution:d}.wav'),
            samplerate,
            write_frames.astype(np.int16))
        
        # plot
        plt.clf()
        plt.xlim([0, resolution])
        plt.ylim([-gain*1.1, gain*1.1])
        plt.title(f'walsh_{n:04d}')
        plt.plot(write_frames, linewidth=1.0)
        plt.savefig(os.path.join(pltdir, f'walsh_{n:04d}_{resolution:d}.png'))
        
    plt.close(fig)
        
        
