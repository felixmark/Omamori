import math
import matplotlib.pyplot as plt
import numpy as np

def sigmoid(x):
  return 1 / (1 + math.exp(-x))

def main():
  curve = []

  for i in np.linspace(-10, 0, 125):
    curve.append(sigmoid(i))
  for i in np.linspace(0.1, 10, 125, endpoint=True):
    curve.append(sigmoid(i))
  #for i in reversed(curve):
  #  curve.append(i)

  curve = [round(point * 255) for point in curve]
  curve = list(filter(lambda x: 255 > x >= 0.5, curve))
  curve.append(255);

  # Print Array
  print("[ ", end='')
  print(', '.join([str(i) for i in curve]), end='')
  print(" ]")

  plt.plot(curve)
  plt.ylabel('Sigmoid')
  plt.show()

if __name__ == '__main__':
  main()