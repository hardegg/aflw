'''
Created on 07.09.2010

@author: pwohlhart
'''

if __name__ == '__main__':
    import os    
    import subprocess
    #from sets import Set;
    path = './test/';
    filenames = os.listdir(path);
	
    deletedTypes = set([]);
    keptTypes = set([]);
    for fn in filenames:
        #p = os.popen('file -bi '+ path + fn);
        output = subprocess.Popen(["file", "-bi", path+fn], stdout=subprocess.PIPE).communicate()[0];
        ftype = output.partition(';')[0];
        maintype = ftype.partition('/')[0];        
        if maintype != "image":            
            print "deleting " + fn + ": " + ftype;
            deletedTypes = deletedTypes | set([ftype]);
            os.remove(path+fn);
        else:
            keptTypes = keptTypes | set([ftype]);
            
    
    print "list of deleted types";
    print deletedTypes;
    print "list of types not deleted";
    print keptTypes;
    