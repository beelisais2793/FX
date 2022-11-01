import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("output.tsv", delimiter='\t', skiprows=1)

time = np.transpose(data[:,0])

part_num = 10
pos_tracks = []
mom_tracks = []
for i in range(1,part_num+1):
    pos_tracks.append(np.transpose(data[:,i]))
    mom_tracks.append(np.transpose(data[:,i+part_num]))

for track in pos_tracks:
    plt.plot(time,track)
plt.xlabel("Time")
plt.ylabel("Position")
plt.savefig("Positions.pdf")
plt.show()

for track in mom_tracks:
    plt.plot(time,track)
plt.xlabel("Time")
plt.ylabel("Momentum")
plt.savefig("Momenta.pdf")
plt.show()