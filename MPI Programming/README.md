# Programming Assignment IV: MPI Programming

## :memo: Part1

### Q1-1: How do you control the number of MPI processes on each node?
每個node的max processes都是4個，所以如果沒有設定的話都會等到四個都使用到才會在去使用下一個node。
![](https://i.imgur.com/Di3IUrJ.png)
如果要限制每個node內的process的數量就要改寫hosts:
![](https://i.imgur.com/jxbZ0G8.png)

> 這邊可以用 code block
> [name=Liu An Chi]

### Q1-2: Which functions do you use for retrieving the rank of an MPI process and the total number of processes?
      MPI_Comm_size(MPI_COMM_WORLD, &world_size);
      MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
### Q1-3: We use Open MPI for this assignment. What else MPI implementation is commonly used? What is the difference between them?
MPICH
Open MPI和MPICH兩者的需求問題就不同。MPICH被認為是最新的MPI標準的公認參考實施，是滿足特殊需要的基礎，而OpenMPI在使用和網絡管道方面都針對比較一般的大眾型問題而設計。

### Q2-1: Why MPI_Send and MPI_Recv are called “blocking” communication? 
Blocking communication是透過MPI_Send and MPI_Recv來完成的。他的意思是當communication開始後就會等到溝通結束才能return，也就是產生block去阻礙結束。這意味著傳遞給MPI_Send的緩衝區可以重複使用，這是因為MPI將其保存在某個地方，或者是因為它已被目標接收。 同樣，當接收緩衝區中已填充有效數據時，MPI_Recv返回。
### Q2-2: Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it.
### pi_block_linear
| MPI Processes | Runnimg Time |
| -------- | -------- |
| 2     | 6.586267 Seconds |
| 4     | 3.425749 Seconds |
| 8     | 1.697972 Seconds |
| 12    | 1.299808 Seconds |
| 16    | 0.853931 Seconds |

> SPEC 上明明寫要畫圖...
> [name=Liu An Chi]

### Q3-1: Measure the performance (execution time) of the code for 2, 4, 8, 16, 32 MPI processes and plot it.
### pi_block_tree
| MPI Processes | Runnimg Time |
| -------- | -------- |
| 2     | 8.318826 Seconds |
| 4     | 4.562167 Seconds |
| 8     | 1.728443 Seconds |
| 12    | 1.131042 Seconds |
| 16    | 0.846427 Seconds |
### Q3-2: How does the performance of binary tree reduction compare to the performance of linear reduction?
由Q2-2和Q3-1可以看出其實兩者的時間是差不多的。但細看的話會發現在processes越小的時候linear reduction的running time是比binary tree reduction來的小蠻多的，然而當processes越大則binary tree reduction的running time是比linear reduction來的小。
由此可以發現說binary tree reduction的方式確實是會比較快，但設置send和recv所花費的時間在processes少的時候會比直接運行來的久，當processes比較大的時候才會顯現出binary tree的效率。
### Q3-3: Increasing the number of processes, which approach (linear/tree) is going to perform better? Why? Think about the number of messages and their costs.
就像我在Q3-2講的，當processes增加時，tree reduction的表現是比較好的。這是因為當process越大時，tree中的每個process所花的時間會比較一致不會有loading balance的問題。就像助教提供的圖一樣:
linear
![](https://i.imgur.com/cgBue94.png)
tree
![](https://i.imgur.com/7mlSSY4.png)
當processes越多時，在linear中rank=0需要匯集所有其他的messages所花的時間會全部堵在這邊，而tree卻是把這些都分散在不同process中處理掉，所以在processes增加時，tree reduction的表現是比Linear來的好的。

### Q4-1: Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it.
### pi_nonblock_linear
| MPI Processes | Runnimg Time |
| -------- | -------- |
| 2    | 7.164497 Seconds |
| 4    | 3.584630 Seconds |
| 8    | 1.685702 Seconds |
| 12   | 1.128476 Seconds |
| 16   | 0.863399 Seconds |
### Q4-2: What are the MPI functions for non-blocking communication?
non-blocking communication is done using MPI_Isend() and MPI_Irecv().
### Q4-3: How the performance of non-blocking communication compares to the performance of blocking communication?
non-blocking communication比blocking communication來的快，因為non-blocking在通訊還沒結束時就會及時return，所以不會被等待的時間所卡住，這讓後面的計算和通信可以重疊，可以提高計算效率。

### Q5-1: Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it.
### pi_gather
| MPI Processes | Runnimg Time |
| -------- | -------- |
| 2    | 6.699864  Seconds|
| 4    | 5.434176 Seconds |
| 8    | 1.874692 Seconds |
| 12   | 1.127367 Seconds |
| 16   | 0.903151 Seconds |

### Q6-1: Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it.
### pi_reduce
| MPI Processes | Runnimg Time |
| -------- | -------- |
| 2    | 6.491907 Seconds |
| 4    | 3.360180 Seconds |
| 8    | 1.694080 Seconds |
| 12   | 1.391748 Seconds |
| 16   | 0.864415 Seconds |

### Q7-1: Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it.
### pi_one_side
| MPI Processes | Runnimg Time |
| -------- | -------- |
| 2    | 6.351087 Seconds |
| 4    | 4.583709 Seconds |
| 8    | 1.843496 Seconds |
| 12   | 1.234632 Seconds |
| 16   | 0.991348 Seconds |

### Q7-2: Which approach gives the best performance among the 1.2.1-1.2.6 cases? What is the reason for that?
其實就我的結果來看，每個方法的速度都是差不多的，並沒有誰有特別顯著的效果，因為有些方式是porcess少的時間表現比較好，但process多就變另一個方法表現比較好，所以我就單以number of process = 4來看。
以number of process = 4來看的話是reduce的方法比較好(3.360180 Seconds)，因為reduce會直接把每個process算出來的結果加總再一起，而其他的方法都需要再rank = 0時等蒐集完全部資料再跑回圈一次做加總，因此reduce會節省掉這個時間。

> 這邊我會希望放一張所有數據疊在一起的圖
> [name=Liu An Chi]

### Q7-3: Which algorithm or algorithms do MPI implementations use for reduction operations? You can research this on the WEB focusing on one MPI implementation.
我在網路上查到這篇Paper: Hierarchical Optimization of MPI Reduce Algorithms
這篇論文提出了對現有通信算法進行分層拓撲忽略的轉換，作為優化MPI集體通信算法和基於MPI的應用程序的一種新的方法。



### Q8-1: Plot ping-pong time in function of the message size for cases 1 and 2, respectively.
![](https://i.imgur.com/6LTglgt.png)

### Q8-2: Calculate the bandwidth and latency for cases 1 and 2, respectively.
| Case | Bandwidth | Latency |
| -------- | -------- | -------- |
| 1 | 1.51526E-10 | 7.15934E-05 |
| 2 | 8.56341E-09 | 0.000285884 |


### Q8-3: For case 2, how do the obtained values of bandwidth and latency compare to the nominal network bandwidth and latency of the NCTU-PP workstations. What are the differences and what could be the explanation of the differences if any? 
Case2的Bandwidth和Latency都比case1(標準網路)來的大，而且從Q8-1的圖更可以看出size越大case2花的時間就比case1來的大很多，這也就影響了Bandwidth和Latency。因為case2是使用了兩個node(每個node裡面只能跑一個process)，所以mpi_send和mpi_recv所花費的時間一定會比原本標準的case1(processes可以在同個node上跑)來的多。

> 你只解釋原因，但沒把數據寫出來
> [name=Liu An Chi]

## :memo: Part2
### Q9: Describe what approach(es) were used in your MPI matrix multiplication for each data set.
http://www.lac.inpe.br/~stephan/CAP-372/matrixmult_microsoft.pdf
我使用Block-Striped Decomposition最基本的演算法來完成這次作業。我會選擇這個方式是因為我查詢了很多不同的演算法，例如:cannon, foxs' algorithm 等等，剛好查到有人對這三個演算法做效率評估，如下圖:
![](https://i.imgur.com/Dw0Iaj2.png)
就我們的data set大小來看(300-500, 1000-2000)，兩個大小都是Block-Striped Decomposition速度提升的最多，因此我最終決定使用Block-Striped Decomposition。

> [color=red] Interesting!!
> [name=Liu An Chi]

Block-Striped Decomposition作法:
我的方法就是不同process接收matrix1的不同的行數，然後每個process都要跑每一欄的matrix2，這樣來完成result，其實就跟serial的想法很接近。
類似於下面這張圖的形式:
![](https://i.imgur.com/GxZPAnc.png)
他是不同process接收matrix2的不同的行數，跟我顛倒，但方式是一樣的。
然後我是透過MPI_Bcast、MPI_Gatherv和MPI_Scatterv來完成這次作業的，因為最一開始讀檔只能由rank=0來完成，所以使用MPI_Bcast來傳給其他node，然後我的部分matrix1的部分則是使用MPI_Scatterv來讓每個node讀取自己的部分matrix1，再去執行自己部份的相乘，最終再使用MPI_Gatherv取取部分result組成成完整的result並執行output。
