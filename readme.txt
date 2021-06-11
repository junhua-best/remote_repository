这是FreeRTOS官网下载的系统，在VS2019编译后的工程，删除了一部分不影响的文件。
使用方法：打开 \FreeRTOS_visual_studio_project\FreeRTOSv202104.00\FreeRTOS\Demo\WIN32-MSVC\WIN32.sln 运行本地调试器即可运行，无需配置。

如果您需要复现统计切换任务间隔时间的实验，请更换change_file里面的几个文件。方法如下：

路径1：
\FreeRTOSv202104.00\FreeRTOS\Demo\WIN32-MSVC\
请在路径下增加 calculate_tick.h 
覆盖 main_blinky.c 

路径2::
\FreeRTOSv202104.00\FreeRTOS\Source\portable\MSVC-MingW\
请在该路径下覆盖 port.c 
路径3：

\FreeRTOSv202104.00\FreeRTOS\Source\
请在该路径下覆盖 task.c 
