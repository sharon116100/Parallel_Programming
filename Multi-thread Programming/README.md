# Programming Assignment II: Multi-thread Programming

## :memo: Part2

### Q1: In your write-up, produce a graph of speedup compared to the reference sequential implementation as a function of the number of threads used FOR VIEW 1.
view 1
![](https://i.imgur.com/72pkBDU.png)
view 2
![](https://i.imgur.com/v10COxI.png)

    
### Is speedup linear in the number of threads used? In your writeup hypothesize why this is (or is not) the case? 

根據上面的speedup for view 1 and view 2，我們可以發現當numofthread = 3時，在view 1所花的時間居然比numofthread = 2還久，但在view2中卻不會有這個問題。
而我認為會出現這個問題是因為load balance的問題在這邊出現。
![](https://i.imgur.com/4r3XRNu.png)
因為我在處理data parallel是在照片中的高度做調整，有2個thread就把高度/2，有3個thread就把高度/3，以此方式去做data parallel，而這種方式在numofthread = 2時因為上下兩張照片的資料量是差不多的，但當numofthread = 3時，處理中間的thread的資料量是偏多的。如下圖：
![](https://i.imgur.com/43obb6A.png)
因此會有load balance的問題。而view 2則比較部會那麼嚴重的balance問題出現，因為並沒有那麼大差異的資料量出現。

### Q2: How do your measurements explain the speedup graph you previously created?
![](https://i.imgur.com/mRc5nMe.png)
我在workerThreadStart()中間入計算時間並印出他的numofthread後，如上圖，從圖中可看出thread 1,2,3所花的時間落差非常大，並且花最多時間的thread也如我所說的是中間的thread1，因此從我的時間測量結果可得知我的假設是正確的。

### Q3: In your write-up, describe your approach to parallelization and report the final 4-thread speedup obtained.
在Q2證明了我的假設成立後，我認為要加速效率的方法就是處理掉load balance的問題，而最終也成功將效率提高至3.79x(for 4 threads)，但在這過程中經歷了很多次失敗。
![](https://i.imgur.com/nCtdQYY.png)

一開始我便是根據Q2的發現，把中間thread會處理的資料量減少，也就是手動改變邊界的方式，如下圖。但此方式並不好，因為必須根據不同thread數量做不同的調整，而且view2也會需要不一樣的範圍調整，因此這個做法不行。
![](https://i.imgur.com/3ozaRlc.png)

上述方法失敗後，我就開始觀察照片，我發現兩張照片都有一些相似的形狀，像view1有很多圈圈上的小圈圈，而view2則是有類似觸手的形狀，所以我就想說或許把照片再切細一些會或許更可以增加他的效率，雖然說會使用到for迴圈去規定不同thread去哪哪個區域，但其實不會增加時間，因為本身call的mandelbrotSerial()裡也是用for迴圈去跑每個dv，因此用for去call function並不會增加到時間，而最終結果也是如此。

### Q4: Now run your improved code with eight threads. Is performance noticeably greater than when running with four threads? Why or why not? (Notice that the workstation server provides 4 cores 4 threads.)
![](https://i.imgur.com/4ILT9RS.png)
如圖所示，8個thread的效率提高了3.71x而4個thread則提高3.79x，顯然8個thread並沒有更好的表現。
我們的workstation提供了四個cores，當我們只用到四個thread時每個thread可以運用到完整的一個core的資源來加速運算的效率，但當有8個thread時，一個core裡就會有2個thread需要分配資源，但要均勻分配是很不容易的，有很大的機會會造成大部分的資源都給第一個thread而第二個thread就會起不了作用，而導致8個thread的效率和4個thread差不多。
然後再考慮到新增thread所花費的時間8個thread會是4個的兩倍，因此效率差不多或是差一些都是合理的範圍。


