'''


'''
import re
count=0 #numbers of function
number=48836 #number of map lines
map_data=[None] * number
function_name=[None] * 100000
map_data_copy=[None] * number
'''
open map file,save each line in ret array.
'''
def save_map_data(path):
    global number
    a=0;
    with open(path, 'r') as mf:
        while a<number:    
            map_data[a] = mf.readline()
            if len(map_data[a])==0:    
                number =a
                break
            a=a+1
   



#Filter out fake top-level functions
def Filter_fake_top_level(Str):
    global count
    d=0
    temp="refers to "
    temp=temp+Str
    temp=temp.strip()
    while d<number:        
            if temp in map_data_copy[d]:
                return False
            d=d+1
    if Str not in function_name:
                        function_name[count] = Str                     
                        file_handle.write(Str+"\n") 
                        count=count+1
'''

recursive funcion:
Parameters:The first half of the line that meets the condition
such as "oppo_heapins.o(i.oppo_heapins_db_calloc)"
from "oppo_heapins.o(i.oppo_heapins_oaf_malloc) refers to heap_6.o(i.Heap6Malloc) for Heap6Malloc "

'''
def recursive_find_top_func(Str,i,last):             
    k=0
    #function_numbers=0
    while k<number:
            temp="refers to "
            temp=temp+Str
            if temp in map_data[k]:            
                res=re.findall(r'[(](.*?)[)]', map_data[k])          
                if "i." in res[0] and "i." in res[1] :   
                    list=res[0].split("i.")
                    #The uncomment line prints out the call chain
                    #file_handle.write(str(i)+"层 "+list[1]+"-<")           
                    strlist = map_data[k].split(" refers to ")
                    ans=strlist[0] # oppo_heapins.o(i.oppo_heapins_db_calloc)
                    ans=ans.strip()
                    if last == ans:
                         #if A->B->A->B
                        break;
                    '''
Set to empty .This will cause the call chain to be somewhat incomplete, but it will avoid the situation where the recursion cannot be terminated,
so there will be part of the "false top function, so I need to deal with it later".
                    '''
                    map_data[k]=""
                    if len(ans) > 0 :
                        last=Str
                        recursive_find_top_func(ans,i+1,last)
                else:
                 # the end
                    file_handle.write("\n")
                    break
           
            k=k+1
    #the end ,Adds the top-level function to the file
    if Str not in function_name:
                        Filter_fake_top_level(Str)
                        

# oppo_heapins.o(i.oppo_heapins_oaf_malloc) refers to heap_6.o(i.Heap6Malloc) for Heap6Malloc          
if __name__ == '__main__':
    file_handle=open('lv_mem_alloc直接调用与顶层函数.txt',mode='w')
    c=0
    i=1
    save_map_data('./test.map')
    #Back up this data because map_data will be modified in the recursive function
    map_data_copy=map_data.copy()
    while c<number:
        if "refers to lv_mem.o(i.lv_mem_alloc)" in map_data[c]:
            file_handle.write("###################\n")
            file_handle.write("调用链"+str(i)+"\n")
            i=i+1
            res=re.findall(r'[(](.*?)[)]', map_data[c])
             #cut "i."   
            list=res[1].split("i.")
            list2=res[0].split("i.")
            strlist = map_data[c].split(" refers to ")
            res=strlist[0]
            res=res.strip()
            file_handle.write("直接调用关系："+list[1]+"-<"+list2[1]+"\n"+"本调用链的顶层调用函数：\n")
            recursive_find_top_func(res,2,"") 
        c=c+1
    file_handle.close()   
