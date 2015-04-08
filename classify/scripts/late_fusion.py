#!/usr/bin/env python

import argparse
from math import exp

def parse_file(fl):
  cate = dict()
  state = 0
  for l in fl:
    l = l.strip()
    if state == 0:
      if l == '## start ##':
        entries = dict()
        state = 1
    elif state == 1:
      if l.startswith('## description:'):
        desc = l
      elif l.startswith('## tag:'):
        tag = l
      elif l == ('## end ##'):
        cate[tag] = (desc, entries)
        state = 0
      else:
        tokens = l.split()
        entries[tokens[0]] = (int(tokens[2]), float(tokens[3]), float(tokens[4]))
  return cate
 
def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('file1', help='Input File 1', type=str)
  parser.add_argument('file2', help='Input File 2', type=str)
  args = parser.parse_args()
  print args.file1, args.file2

  f1 = open(args.file1, 'r')
  cate1 = parse_file(f1);
  f1.close()

  f2 = open(args.file2, 'r')
  cate2 = parse_file(f2);
  f2.close()
  
  assert cate1.keys() == cate2.keys()

  for tag in cate1:
    de1 = cate1[tag][0]
    de2 = cate2[tag][0]
    en1 = cate1[tag][1]
    en2 = cate2[tag][1]
    assert de1 == de2
    assert set(en1.keys()) == set(en2.keys())
    print '## start ##'
    print de1
    print tag
    for key in en1.keys():
      assert en1[key][0] == en2[key][0]
      pl = en1[key][1] + en2[key][1]
      nl = en1[key][2] + en2[key][2]
      df = min(max(nl - pl, -100), 100)
      pp = 1.0 / (1.0 + exp(df))
      print "%15s %5.2f %5d %8.4g %8.4g" % (key, pp, en1[key][0], pl, nl)
    print '## end ##'

if __name__ == '__main__':
  main()

