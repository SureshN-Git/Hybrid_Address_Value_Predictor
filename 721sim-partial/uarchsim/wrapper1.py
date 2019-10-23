import os
import sys

os.system("make clean")
os.system("make")


list = []
 
list.append('473.astar_rivers_ref.5883.0.20.gz') 
list.append('437.leslie3d_test.152.0.25.gz')     
'''list.append('444.namd_test.84.0.21.gz')         
list.append('483.xalancbmk_test.2.0.33.gz')       
list.append('445.gobmk_ref.2465.0.18.gz')        
list.append('453.povray_ref.2301.0.24.gz')   
list.append('458.sjeng_test.24.0.24.gz')                            
list.append('462.libquantum_ref.13679.0.28.gz')                
list.append('464.h264ref_ref.1989.0.46.gz')            
list.append('465.tonto_test.22.0.29.gz')         
list.append('471.omnetpp_test.6.0.58.gz')'''

#filename = "output.txt"
#open(filename, 'w').close()


for i in list:
	cmd = "./721sim --disambig=0,1 --perf=0,0,0,0 --fq=128 --cp=64 --al=512 --lsq=256 --iq=128 --iqnp=4 --fw=16 --dw=16 --iw=16 --rw=16 -m2048 -e100000000 -c" + i + " pk"

	print(cmd)

	os.system(cmd)
