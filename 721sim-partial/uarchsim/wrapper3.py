import os
import sys

os.system("make clean")
os.system("make")


list = []
  
list.append('444.namd_test.84.0.21.gz')         
list.append('483.xalancbmk_test.2.0.33.gz')       
list.append('445.gobmk_ref.2465.0.18.gz')        
#list.append('453.povray_ref.2301.0.24.gz')   

#filename = "output.txt"
#open(filename, 'w').close()


for i in list:
	cmd = "./721sim --disambig=0,1 --perf=0,0,0,0,0 --vflags=0,1,0,1 --vp=2 --psize=1024 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c" + i + " pk"

	print(cmd)

	os.system(cmd)
