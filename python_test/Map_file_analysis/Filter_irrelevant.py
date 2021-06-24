
number=48840

import re
map_data=[None] * number
a=0;
'''
open map file,save each line in ret array.
'''
with open('./data.map', 'r') as mf:
    while a<number:    
        map_data[a] = mf.readline()
        a=a+1


#Filter out lv_ *.o lines, create a new file.
def Filter_of_lv(name):
    file_handle_filter=open(name,mode='w')
    k=0
    while k<number:
        map_data[k]=map_data[k].strip()
        res=re.search(r'lv\S*\.o\(', map_data[k])
        if res==None or "lv_mem.o(i.lv_mem_alloc)"  in map_data[k]:
            res2=re.match(r'lv_', map_data[k])
            if res2==None:
                file_handle_filter.write(map_data[k]+"\n")
        if len(map_data[k])==0:
            break
        k=k+1    
    file_handle_filter.close()

#create a test.map that without lv*.o files
if __name__ == '__main__':
     Filter_of_lv('test.map')
