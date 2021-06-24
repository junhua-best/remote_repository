/*
功能作用：
1.过滤掉lv_*.o的文件所参与的调用链
2.搜索剩余调用链中，直接调用lv_mem_alloc的函数以及调用链中的顶层调用函数

使用准备文件：
data.map：map文件（只包含函数之间调用关系的map文件，无关信息已经清楚）
使用方法：
1.运行Filter_irrelevant.py
根据data.map生成test.map，tast.map即为过滤了lv_*.o文件的map file
2.运行Recursive_for_top_function.py  即可以按要求生成 “lv_mem_alloc直接调用与顶层函数.txt”
*/
