import numpy as np

x = np.linspace(0, 2 * np.pi, 1025)

cos_x = np.cos(x)

print("{")
print(*(y.hex() for y in  cos_x), sep=",\n")
print("}")