1. normal malloc call + free + coalesce (involve splitting, 8192-chunk_size > 16)

   

2. free invalid pointer

   1. free the beginning of free_list 

   2. free beginning 往后10位

   3. call built-in malloc 

   4. 超过heap的最下面

      

3. malloc(zero), 不算错 return NULL, 

4. free (NULL), return error message 2 

5. normal malloc call when there is free_list size big enough

6. normal malloc call with there is free_list size big enouogh but it's not the first one 

7. malloc call with more than 8192-8, oversize 

8. request non-8

9. 

test malloc(8192-8-8)

free错误的之后 free 正确的