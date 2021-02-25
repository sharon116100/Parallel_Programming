# Programming Assignment V: CUDA Programming

## :memo: Part1

### Q1 What are the pros and cons of the three methods? Give an assumption about their performances.
我想分成兩個部份來說明三種方法的優缺點:
1. **cudaMalloc VS cudaMallocPitch**

    一般來說，GPU是使用cudaMalloc函數來分配內存的，但是在二維或三維矩陣中，使用cudaMalloc來分配內存並不能獲得最佳性能，因為內存用於2D或3D，對齊方式非常 這是一個重要的屬性。如果我們要一個二維數組，並且如果使用cudaMalloc來分配內存空間，那麼我們訪問某一行，那麼我們必須遍歷前一個要訪問的所有元素。因此，如果我們希望我們的每一行都可以並行訪問，那麼當有多個線程訪問時，將可以訪問不同的存儲體。需要保持門地址對齊。
    CUDA中的存儲區機制涉及共享內存訪問也會讓這兩種方式有效率上的差距。當陣列位於共享內存中時，cuda會將他分成好幾個存儲體，如果兩個CUDA thread不訪問屬於同一存儲庫的內存，則它們可以同時訪問它，根據這個狀況我們通常會希望並行處理每一行，因此可以通過將每一行填充到新存儲區的開頭來確保可以模擬訪問它。這也讓cudaMallocPitch在二維的狀況下效率提升很多。
    
2. **per pixel VS a group of pixels**
    由於gpu是可以產生較多個thread去處理一些簡單的運算，而cpu則是產生較少的thread去處理較負責的運算，因此使用cuda時直接讓每一個thread處理一個pixel的計算是會加速非常多的，可依同時並行處理很多pixels。而group of pixels則就沒那麼適合運用在gpu比較適合用在cpu上。
3. **預期結果**
    因此我會預期  time of kernel2 < time of kernel1 << time of kernel3.

### Q2 How are the performances of the three methods? Plot a chart to show the differences among the three methods
* view1
![](https://i.imgur.com/QB82v00.png)
* view2
![](https://i.imgur.com/y5MIl6q.png)




### Q3 Explain the performance differences thoroughly based on your experimental results. Does the results match your assumption? Why or why not.
實驗的結果跟我當初的預期差不多，但差距並沒有到太大，我覺得是因為我程式的寫法讓他的效率沒辦法提升太多。這幾種方法可以產生差距的地方就是host device memory傳送的部分、寫入資料進memory的部分，而我並沒有使用到寫入資料進 memory，因為我是直接用argument傳入data在kernel中計算，所以cudaMallocPitch的加速就會少一個部分，那效率上就不會差到太多。
而group of pixels也照著預期的花費的時間大非常多，因此可以應證這個方式不適合運用在gpu，而是要用在cpu上，或者說應該可以用另一種方式去改進group of pixels，他應該是可以解決load balance的方法之一。

### Q4 Can we do even better? Think a better approach and explain it. 
我上網查了很多方法，查到很多人說CUDA動態並行的自適應並行計算是非常適合運用在Mandelbrot上，如下圖
![](https://i.imgur.com/0xAzQcD.png)
動態並行就是會根據這塊區域內的工作份量大而切細一點多點cuda thread去處理，然後工作份量小的話就少一些去處理。
![](https://i.imgur.com/3VYa7LP.png)
方法就是會在圖中設置很多個區塊，並且計算邊界是否一致，如果一致的話就統一填入那個數值，如果不一致就再細切這個區塊的大小去找一致，如果到最小邊框大小時就使用per pixel的方式填入這個區塊。這樣就可以減少per pixel一直做重複動作的時間。




