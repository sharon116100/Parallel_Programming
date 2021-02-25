# Assignment VI: OpenCL Programming

## :memo: Q1:Explain your implementation. How do you optimize the performance of convolution?
我一開始先寫了最基礎的opencl singal版本，kernel內的程式跟serial幾乎一樣，其中有優化的點是:
1. 而傳入inputimage和outputimage則使用clCreateImage2D來init，他會比clCreateBuffer來的快一些。
2. 還有傳入kernel的filter我使用`__constant float4 * filter ` filter在執行過程中並不會改變，因此可以將其存儲在常量存儲器中，而不是將其存儲在全局存儲器中。常量內存比全局內存快，因為它在內核啟動之前已被預先緩存。
3. 使用 `#define Height, Width`:將它們在kernel內宣告而不是argment傳進來也可以提升一些計算速度。
