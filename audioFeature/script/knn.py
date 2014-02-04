#!/bin/env python

import sys
from operator import itemgetter
from collections import Counter
import math

K = 3

def parse(entry):
  tk = entry.split(':')
  idx = int(tk[0]) - 1;
  dis = float(tk[1]);
  return (idx, dis)

# Main function
def main(argv):
  with open(argv[0], 'r') as f:
    trclass = [int(line.split()[0]) for line in list(f)]

  with open(argv[1], 'r') as f:
    lines = list(f)
    teclass = [int(line.split()[0]) for line in lines]
    tedis = [map(parse, line.split()[2:]) for line in lines]

  correct = 0.0
  for i in range(len(teclass)):
    nn = [trclass[e[0]] for e in sorted(tedis[i], key=itemgetter(1), reverse=True)[:K]]
    count = Counter(nn)
    mc = count.most_common(1)[0]
    if mc[1] == 1:
      prclass = nn[0]
    else:
      prclass = mc[0]

    print prclass, teclass[i]
    if (prclass == teclass[i]):
      correct += 1.0

  print 'Accurracy = %f' % (correct / len(teclass))
  
if __name__ == "__main__":
  main(sys.argv[1:])

