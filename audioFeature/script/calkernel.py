#!/bin/env python

import sys, getopt
import math

SELF_SIM = 1.0
GAP_PANALTY = -2.0
NW_POWER = 5.0

# Needleman-Wunsch Similarity
# x, y: 2 sequences to be compared
# s   : Similarity matrix
# d   : Gap panalty
def nw(x, y, s, d):
  f = [[0 for j in range(len(y) + 1)] for i in range(len(x) + 1)]

  for i in range(len(x) + 1):
    f[i][0] = d * i
  for j in range(len(y) + 1):
    f[0][j] = d * j
       
  for i in range(1, len(x) + 1):
    for j in range(1, len(y) + 1):
      match = f[i - 1][j - 1] + s[x[i - 1]][y[j - 1]]
      delete = f[i - 1][j] + d
      insert = f[i][j - 1] + d
      f[i][j] = max(match, insert, delete)
  
  return f[len(x)][len(y)]

# Distance between 2 SOM nodes
# x, y: 2 ids of the nodes
# rows: Number of rows in the SOM
# cols: Number of columns in the SOM
def distance(x, y, rows, cols):
  dr = x / cols - y / cols
  dc = x % cols - y % cols
  return math.sqrt(dr * dr + dc * dc)

# Main function
def main(argv):
  rows = 6
  cols = 6

  opts, args = getopt.getopt(argv, "r:c:")
  for optn, optv in opts:
    if optn == '-r':
      rows = int(optv)
    elif optn == '-c':
      cols = int(optv)

  noi = rows * cols
  sim = [[0 for j in range(noi)] for i in range(noi)]

  for i in range(noi):
    sim[i][i] = SELF_SIM
    for j in range(i + 1, noi):
      sim[i][j] = -distance(i, j, rows, cols)
      sim[j][i] = sim[i][j]

  with open(args[0], 'r') as f:
    lines = [line.split() for line in list(f)]
  
  nol = len(lines)
  seq = [map(int, lines[i][2:]) for i in range(nol)]
  kernel = [[0 for j in range(nol)] for i in range(nol)]
  
  for i in range(nol):
    kernel[i][i] = nw(seq[i], seq[i], sim, GAP_PANALTY)
    for j in range(i + 1, nol):
      kernel[i][j] = nw(seq[i], seq[j], sim, GAP_PANALTY)
      kernel[j][i] = kernel[i][j]

  for i in range(nol):
    print lines[i][0], lines[i][1], '0:%d' % (i + 1),
    for j in range(nol):
      print '%d:%g' % (j + 1, kernel[i][j]),
    print
 
if __name__ == "__main__":
  main(sys.argv[1:])

