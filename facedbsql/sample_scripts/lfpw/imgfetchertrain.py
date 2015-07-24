#!/usr/bin/env python
"""A simple script to fetch the images of the facetracer dataset.
"""

import os
import sys
import socket
socket.setdefaulttimeout(30)

if __name__ == '__main__':
	import sys
	import urllib
	zeros = '000000000000000000000';
	fname = 'kbvt_lfpw_v1_train.csv';
	lines = (l.strip().split('\t') for l in open(fname) if not l.startswith('#'))
	num = 0;
	for l in lines:
	    if l[1] == "average":
		nameStr = "train";
		num = num + 1;
		numStr = str(num);
		url = l[0];
		
		print nameStr;
		print numStr;
		print url;
		
		if int(numStr) == 1:
			cmd="mkdir \"" + nameStr + "\"";
			print cmd;
			os.system(cmd)
			
		
		
		fn = "train" + zeros[:6-len(numStr)] + numStr + '.jpg';
		fn = './' + nameStr + '/' + fn;
		print fn;
		if not os.path.isfile(fn):
			try:
				urllib.urlretrieve(url,fn)
			except:
				print 'error';
				
	       

		#sys.exit(0);
