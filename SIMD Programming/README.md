# Programming Assignment I: SIMD Programming


## :memo: Part1

### Q1-1: Does the vector utilization increase, decrease or stay the same as VECTOR_WIDTH changes? Why?

1. **呈現N=2, 4, 8, 16的結果圖:(N=18)**
    Vector Width = 2
    Vector Utilization: 83.3%
    ![](https://i.imgur.com/4Pq4XMd.png)
    Vector Width = 4
    Vector Utilization: 78.3%
    ![](https://i.imgur.com/HSWVz0C.png)
    Vector Width = 8
    Vector Utilization: 73.3%
    ![](https://i.imgur.com/ZVEAd7l.png)
    Vector Width = 16
    Vector Utilization: 72.4%
    ![](https://i.imgur.com/cmq3axC.png)

    
    **由以上四個不同的Vector Width可得知Vector Utilization會隨著Vector Width的上升而下降。**
2. **為何會出現這樣的結果**
    因為平行時導致空間的浪費，而這個作業一共會有兩個地方產生浪費的現象。
    *   資料處理  
        ```=
        for (int i = 0; i < N; i += VECTOR_WIDTH){
        ...
        }
        ```
        也就是當N % VECTOR_WIDTH != 0時，空間會產生浪費。
        舉個例子:        
        當N = 10, VECTOR_WIDTH = 4
        | Data | Mask |
        | -------- | -------- |
        | 0, 1, 2, 3  | 1111  |
        | 4, 5, 6, 7  | 1111  |
        | 8, 9| 1100   |
        從Mask可以看出由於一次要跑四個Data，但最後一run不滿四個，會讓原本的四個空間只利用到兩個而已。
        而且當VECTOR_WIDTH放大，他造成的浪費會變大非常多。
        EX. N = 10, VECTOR_WIDTH = 9
        | Data | Mask |
        | -------- | -------- |
        | 0, 1, 2, 3, 4, 5, 6, 7, 8  | 111111111  |
        | 9  | 100000000  |

    *   指數處理
        ```=
        while(count_one > 0)
		{
            _pp_vgt_int(maskCountIsPositive, countExp, zero_int, maskIsNotZero); // if (count > 0) { 
            _pp_vmult_float(result, result, x, maskCountIsPositive); //  X*X
            _pp_vsub_int(countExp, countExp, one_int, maskCountIsPositive); //   count = count-1;}      
            count_one = _pp_cntbits(maskCountIsPositive);
		}
        ```
        指數處理這邊也會產生空間的浪費，因為四個資料的指數都不一樣，但同時處理必須一起開始一起結束。
        舉例來說:(只看2run)
        | Data | first run<br>Exponents | first run<br>Mask | second run <br>Exponents | second run<br>Mask | 
        | -------- | -------- | -------- | -------- | -------- |
        | 0 | 3 | 1 | 2 | 1
        | 1 | 2 | 1 | 1 | 1
        | 2 | 1 | 1 | 0 | 0
        | 3 | 0 | 0 | 0 | 0
        這個例子一共會跑4 run，而上面的表格只呈現2run，但就已經有空間的浪費產生。由於四個指數都不一樣，第四筆資料的指數是0，所以他在這四次的指數運算中都不會被處理，會造成大量的浪費。
## :memo: Part2
### Q2-1:Fix the code to make sure it uses aligned moves for the best performance. 
改寫前:
```=
  a = (float *)__builtin_assume_aligned(a, 16);
  b = (float *)__builtin_assume_aligned(b, 16);
  c = (float *)__builtin_assume_aligned(c, 16);
```
![](https://i.imgur.com/F25BHbV.png)

改寫後:
```=
  a = (float *)__builtin_assume_aligned(a, 32);
  b = (float *)__builtin_assume_aligned(b, 32);
  c = (float *)__builtin_assume_aligned(c, 32);
```
![](https://i.imgur.com/eJO6Nj2.png)


we align lda and ldc to the 32-byte memory alignment requirement of AVX2.
### Q2-2:What speedup does the vectorized code achieve over the unvectorized code? What additional speedup does using -mavx2 give (AVX2=1 in the Makefile)? You may wish to run this experiment several times and take median elapsed times; you can report answers to the nearest 100% (e.g., 2×, 3×, etc). What can you infer about the bit width of the default vector registers on the PP machines? What about the bit width of the AVX2 vector registers.
1. What speedup does the vectorized code achieve over the unvectorized code?
    unvectorized:
    ![](https://i.imgur.com/Us054TG.png)
    vectorized:
    ![](https://i.imgur.com/V6cdZ3G.png)
    由兩張圖片可看出有無vectorized在時間上差異非常多，沒加是8.17695sec，而有加則變成2.61155sec，在速度上快了三倍。
2. What additional speedup does using -mavx2 give (AVX2=1 in the Makefile)?You may wish to run this experiment several times and take median elapsed times; you can report answers to the nearest 100% (e.g., 2×, 3×, etc). 
    在編譯时需要添加 -mavx2 以啟用AVX2指令集。AVX引入了融合乘法累加（FMA）操作，加速了線性代數計算，例如點積、矩陣乘法，卷積等。
    
    VECTORIZE=1 AVX2=1:
    ![](https://i.imgur.com/C5fjIz6.png)
    加入AVX2後快了兩倍。
3. What can you infer about the bit width of the default vector registers on the PP machines?
    
    bit width of the default vector registers on the PP machines = 256bits.




### Q2-3:Provide a theory for why the compiler is generating dramatically different assembly.
原始的test2:
![](https://i.imgur.com/YMKL9WG.png)
使用patch file修改過的test2:
![](https://i.imgur.com/GtlIaC5.png)

從diff可看出使用patch file修改過的test2就有使用到vectorized，而理由是程式編碼上的邏輯問題。因為原本test2的寫法是先將a存進c中，再去進行if else的判斷去做c的改值，如此一來optimization會認為有overlap，所以會不讓vectorized執行。
而用patch file修改過的test2則只會再if else判斷過後對c的值進行一次的更動，也就不會產生overlap，如此一來optimization就會進行vectorized version。
