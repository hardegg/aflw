import os, sys
from random import shuffle

#filepaths = ['faces/detection12_train.txt', 'neg12_train.txt']
filepaths = ['faces/detection12_val.txt', 'neg12_val.txt']

#ofilename = 'train_detection12.txt'
ofilename = 'val_detection12.txt'
lines = []

for filepath in filepaths:
    fl = open(filepath, 'r')
    lines = lines + fl.read().splitlines()
    fl.close()

fl = open(ofilename, 'w')
shuffle(lines)
for line in lines:
    fl.write(line+'\n')

