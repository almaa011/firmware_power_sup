import os
import glob
import pandas as pd
import pandas.errors as pd_errors
import sys

if __name__ == "__main__":
    os.chdir(sys.argv[1])
    all_filenames = [i for i in glob.glob('*.{}'.format('csv'))]
    #combine all files in the list
    
    fout=open(f"{sys.argv[2]}.csv","w+")
    for f in all_filenames:
        f = open(f)
        for line in f:
            fout.write(line)
        f.close() # not really needed
        print(f"Finished copying from file: {f.name}")
    fout.close()
    print(f"Combined CSV files to output file: {sys.argv[2]}.csv")