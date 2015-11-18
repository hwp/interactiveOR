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
  parser.add_argument('file', help='Input File', type=str)
  args = parser.parse_args()
  print args.file1, args.file2

  f1 = open(args.file, 'r')
  cates = parse_file(f1);
  f1.close()

  for tag in cate2:
    de = cate2[tag][0]
    en = cate2[tag][1]
    print '## start ##'
    print de
    print tag
    #TODO
    for key in enkeys():
      pl = en2[key][1] + en2[key][1]
      nl = en1[key][2] + en2[key][2]
      df = (nl - pl) / 20.0
      df = min(max(df, -100.0), 100.0)
      pp = 1.0 / (1.0 + exp(df))
      print "%15s %5.2f %5d %8.4g %8.4g" % (key, pp, en1[key][0], pl, nl)
    print '## end ##'

if __name__ == '__main__':
  main()

