8 64Initialising a TLB with size: 64
8 64Initialising a TLB with size: 64
6 1536Initialising a TLB with size: 1536
8 64Initialising a TLB with size: 64
8 64Initialising a TLB with size: 64
6 1536Initialising a TLB with size: 1536
8 64Initialising a TLB with size: 64
8 64Initialising a TLB with size: 64
6 1536Initialising a TLB with size: 1536
8 64Initialising a TLB with size: 64
8 64Initialising a TLB with size: 64
6 1536Initialising a TLB with size: 1536
Initialising PT from file: /disk/local/traces/mcf_short//pt_dump
Loading page table with 244291 PT entries...
Loaded 244291 unique PT entries.
Initialising a cache with size: 16777216 with assoc: 16 with line size: 64
16 262144Initialising a cache with size: 262144 with assoc: 8 with line size: 64
8 4096Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 262144 with assoc: 8 with line size: 64
8 4096Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 262144 with assoc: 8 with line size: 64
8 4096Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 262144 with assoc: 8 with line size: 64
8 4096Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Initialising a cache with size: 32768 with assoc: 8 with line size: 64
8 512Finished warmup of TLB
Finished warmup of TLB
Finished warmup of TLB
Finished warmup of TLB
Heartbeat. 4194304 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                        2,629,935
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        1,121,623
    Misses:                        141,871
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       11.23%
  TLB-LL stats:
    Hits:                          140,761
    Misses:                          1,110
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  0.78%
    Child hits:                  3,751,558
    Total miss rate:                  0.03%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                        2,791,122
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        1,159,696
    Misses:                        152,472
    Invalidations:                       0
    Hits PT level     1:               1,194
    Misses PT level   1:                 116
    Hits PT level     2:               1,208
    Misses PT level   2:                 102
    Hits PT level     3:                 693
    Misses PT level   3:                 617
    Hits PT level     4:                 581
    Misses PT level   4:                 729
    Prefetch hits:                  39,175
    Prefetch misses:               114,861
    Miss rate:                       11.62%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                            8,038
    Misses:                        144,436
    Invalidations:                       0
    Hits PT level     1:                  95
    Misses PT level   1:                  21
    Hits PT level     2:                  89
    Misses PT level   2:                  13
    Hits PT level     3:                 533
    Misses PT level   3:                  84
    Hits PT level     4:                 570
    Misses PT level   4:                 159
    Prefetch hits:                   5,559
    Prefetch misses:               109,302
    Local miss rate:                 94.73%
    Child hits:                  3,993,669
    Total miss rate:                  3.48%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          105,945
    Misses:                         38,491
    Invalidations:                       0
    Hits PT level     1:                  21
    Misses PT level   1:                   0
    Hits PT level     2:                  13
    Misses PT level   2:                   0
    Hits PT level     3:                  84
    Misses PT level   3:                   0
    Hits PT level     4:                 147
    Misses PT level   4:                  12
    Prefetch hits:                  73,616
    Prefetch misses:                35,686
    Local miss rate:                 26.65%
    Child hits:                     14,884
    Total miss rate:                 24.16%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 4194304
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	20
L1,L1,L1,L1,	700
L1,L1,L1,L2,	156
L1,L1,L1,LLC,	24
L1,L1,L2,L1,	79
L1,L1,L2,L2,	307
L1,L1,L2,LLC,	62
L1,L1,LLC,L1,	6
L1,L1,LLC,L2,	29
L1,L1,LLC,LLC,	34
L1,L2,L1,L1,	4
L1,L2,L1,L2,	9
L1,L2,L1,LLC,	2
L1,L2,L2,L1,	9
L1,L2,L2,L2,	20
L1,L2,L2,LLC,	5
L1,L2,LLC,L2,	3
L1,L2,LLC,LLC,	2
L1,LLC,L1,L1,	7
L1,LLC,L2,L2,	2
L1,LLC,LLC,L2,	1
L2,L1,L1,L1,	4
L2,L1,L1,L2,	8
L2,L1,L2,L1,	3
L2,L1,L2,L2,	34
L2,L1,L2,LLC,	8
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	2
L2,L1,LLC,LLC,	1
L2,L2,L1,L1,	1
L2,L2,L1,L2,	4
L2,L2,L1,LLC,	3
L2,L2,L2,L2,	14
L2,L2,L2,LLC,	4
L2,L2,LLC,L2,	5
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	2
LLC,L1,L1,L2,	2
LLC,L1,L1,LLC,	1
LLC,L1,L2,L1,	1
LLC,L1,L2,L2,	6
LLC,L1,L2,LLC,	2
LLC,L1,LLC,L2,	1
LLC,L2,L1,L1,	1
LLC,L2,L2,L2,	3
LLC,L2,L2,LLC,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,LLC,	1




Heartbeat. 8388608 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                        5,458,399
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        2,314,280
    Misses:                        314,178
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       11.95%
  TLB-LL stats:
    Hits:                          310,182
    Misses:                          3,996
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  1.27%
    Child hits:                  7,772,679
    Total miss rate:                  0.05%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                        5,679,345
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        2,351,342
    Misses:                        325,790
    Invalidations:                       0
    Hits PT level     1:               4,043
    Misses PT level   1:                 153
    Hits PT level     2:               4,048
    Misses PT level   2:                 148
    Hits PT level     3:               2,647
    Misses PT level   3:               1,549
    Hits PT level     4:               2,390
    Misses PT level   4:               1,806
    Prefetch hits:                  85,296
    Prefetch misses:               244,150
    Miss rate:                       12.17%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           18,156
    Misses:                        307,636
    Invalidations:                       0
    Hits PT level     1:                 125
    Misses PT level   1:                  28
    Hits PT level     2:                 128
    Misses PT level   2:                  20
    Hits PT level     3:               1,399
    Misses PT level   3:                 150
    Hits PT level     4:               1,378
    Misses PT level   4:                 428
    Prefetch hits:                  12,608
    Prefetch misses:               231,542
    Local miss rate:                 94.43%
    Child hits:                  8,129,111
    Total miss rate:                  3.64%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          243,973
    Misses:                         63,663
    Invalidations:                       0
    Hits PT level     1:                  28
    Misses PT level   1:                   0
    Hits PT level     2:                  20
    Misses PT level   2:                   0
    Hits PT level     3:                 150
    Misses PT level   3:                   0
    Hits PT level     4:                 413
    Misses PT level   4:                  15
    Prefetch hits:                 169,392
    Prefetch misses:                62,150
    Local miss rate:                 20.69%
    Child hits:                     33,794
    Total miss rate:                 18.65%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 8388608
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	20
L1,L1,L1,L1,	2320
L1,L1,L1,L2,	399
L1,L1,L1,LLC,	87
L1,L1,L2,MEMORY,	3
L1,L1,L2,L1,	255
L1,L1,L2,L2,	797
L1,L1,L2,LLC,	219
L1,L1,LLC,L1,	10
L1,L1,LLC,L2,	58
L1,L1,LLC,LLC,	60
L1,L2,L1,L1,	8
L1,L2,L1,L2,	17
L1,L2,L1,LLC,	5
L1,L2,L2,L1,	11
L1,L2,L2,L2,	29
L1,L2,L2,LLC,	9
L1,L2,LLC,L2,	4
L1,L2,LLC,LLC,	4
L1,LLC,L1,L1,	8
L1,LLC,L2,L2,	3
L1,LLC,L2,LLC,	3
L1,LLC,LLC,L2,	1
L2,L1,L1,L1,	4
L2,L1,L1,L2,	13
L2,L1,L1,LLC,	1
L2,L1,L2,L1,	4
L2,L1,L2,L2,	45
L2,L1,L2,LLC,	10
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	4
L2,L1,LLC,LLC,	3
L2,L2,L1,L1,	1
L2,L2,L1,L2,	6
L2,L2,L1,LLC,	4
L2,L2,L2,L2,	16
L2,L2,L2,LLC,	4
L2,L2,LLC,L2,	5
L2,LLC,L2,L2,	1
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	3
LLC,L1,L1,L2,	4
LLC,L1,L1,LLC,	1
LLC,L1,L2,L1,	1
LLC,L1,L2,L2,	7
LLC,L1,L2,LLC,	3
LLC,L1,LLC,L2,	1
LLC,L2,L1,L1,	1
LLC,L2,L2,L2,	3
LLC,L2,L2,LLC,	2
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	1
LLC,LLC,L2,LLC,	1




Heartbeat. 12582912 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                        8,299,121
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        3,498,564
    Misses:                        482,580
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.12%
  TLB-LL stats:
    Hits:                          474,996
    Misses:                          7,584
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  1.57%
    Child hits:                 11,797,685
    Total miss rate:                  0.06%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                        8,573,586
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        3,502,238
    Misses:                        527,580
    Invalidations:                       0
    Hits PT level     1:               7,533
    Misses PT level   1:                 251
    Hits PT level     2:               7,589
    Misses PT level   2:                 195
    Hits PT level     3:               4,912
    Misses PT level   3:               2,872
    Hits PT level     4:               4,481
    Misses PT level   4:               3,303
    Prefetch hits:                 139,496
    Prefetch misses:               394,705
    Miss rate:                       13.09%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           28,980
    Misses:                        498,602
    Invalidations:                       0
    Hits PT level     1:                 209
    Misses PT level   1:                  42
    Hits PT level     2:                 169
    Misses PT level   2:                  26
    Hits PT level     3:               2,647
    Misses PT level   3:                 225
    Hits PT level     4:               2,553
    Misses PT level   4:                 750
    Prefetch hits:                  20,974
    Prefetch misses:               373,731
    Local miss rate:                 94.51%
    Child hits:                 12,239,835
    Total miss rate:                  3.91%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          404,967
    Misses:                         93,635
    Invalidations:                       0
    Hits PT level     1:                  42
    Misses PT level   1:                   0
    Hits PT level     2:                  26
    Misses PT level   2:                   0
    Hits PT level     3:                 225
    Misses PT level   3:                   0
    Hits PT level     4:                 733
    Misses PT level   4:                  17
    Prefetch hits:                 281,044
    Prefetch misses:                92,687
    Local miss rate:                 18.78%
    Child hits:                     55,532
    Total miss rate:                 16.90%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 12582912
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	20
L1,L1,L1,L1,	4106
L1,L1,L1,L2,	755
L1,L1,L1,LLC,	181
L1,L1,L2,MEMORY,	5
L1,L1,L2,L1,	534
L1,L1,L2,L2,	1512
L1,L1,L2,LLC,	386
L1,L1,LLC,L1,	12
L1,L1,LLC,L2,	91
L1,L1,LLC,LLC,	91
L1,L2,L1,L1,	13
L1,L2,L1,L2,	19
L1,L2,L1,LLC,	6
L1,L2,L2,L1,	13
L1,L2,L2,L2,	36
L1,L2,L2,LLC,	10
L1,L2,LLC,L2,	4
L1,L2,LLC,LLC,	7
L1,LLC,L1,L1,	8
L1,LLC,L2,L2,	6
L1,LLC,L2,LLC,	3
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	13
L2,L1,L1,L2,	21
L2,L1,L1,LLC,	2
L2,L1,L2,L1,	8
L2,L1,L2,L2,	80
L2,L1,L2,LLC,	16
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	5
L2,L1,LLC,LLC,	4
L2,L2,L1,L1,	1
L2,L2,L1,L2,	7
L2,L2,L1,LLC,	4
L2,L2,L2,L1,	1
L2,L2,L2,L2,	24
L2,L2,L2,LLC,	11
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	2
L2,LLC,L2,L2,	1
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	3
LLC,L1,L1,L2,	4
LLC,L1,L1,LLC,	2
LLC,L1,L2,L1,	3
LLC,L1,L2,L2,	12
LLC,L1,L2,LLC,	5
LLC,L1,LLC,L2,	1
LLC,L1,LLC,LLC,	1
LLC,L2,L1,L1,	1
LLC,L2,L2,L2,	3
LLC,L2,L2,LLC,	3
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	2
LLC,LLC,L2,LLC,	1




Heartbeat. 16777216 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       11,132,946
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        4,690,726
    Misses:                        650,019
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.17%
  TLB-LL stats:
    Hits:                          639,068
    Misses:                         10,951
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  1.68%
    Child hits:                 15,823,672
    Total miss rate:                  0.07%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       11,466,865
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        4,690,219
    Misses:                        699,200
    Invalidations:                       0
    Hits PT level     1:              10,868
    Misses PT level   1:                 283
    Hits PT level     2:              10,927
    Misses PT level   2:                 224
    Hits PT level     3:               7,164
    Misses PT level   3:               3,987
    Hits PT level     4:               6,478
    Misses PT level   4:               4,673
    Prefetch hits:                 184,597
    Prefetch misses:               523,770
    Miss rate:                       12.97%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           38,395
    Misses:                        660,807
    Invalidations:                       0
    Hits PT level     1:                 225
    Misses PT level   1:                  58
    Hits PT level     2:                 191
    Misses PT level   2:                  33
    Hits PT level     3:               3,677
    Misses PT level   3:                 310
    Hits PT level     4:               3,544
    Misses PT level   4:               1,129
    Prefetch hits:                  27,677
    Prefetch misses:               496,093
    Local miss rate:                 94.51%
    Child hits:                 16,377,118
    Total miss rate:                  3.87%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          539,155
    Misses:                        121,652
    Invalidations:                       0
    Hits PT level     1:                  58
    Misses PT level   1:                   0
    Hits PT level     2:                  33
    Misses PT level   2:                   0
    Hits PT level     3:                 310
    Misses PT level   3:                   0
    Hits PT level     4:               1,107
    Misses PT level   4:                  22
    Prefetch hits:                 374,594
    Prefetch misses:               121,499
    Local miss rate:                 18.41%
    Child hits:                     73,709
    Total miss rate:                 16.56%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 16777216
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	21
L1,L1,L1,L1,	5823
L1,L1,L1,L2,	1131
L1,L1,L1,LLC,	318
L1,L1,L2,MEMORY,	7
L1,L1,L2,L1,	790
L1,L1,L2,L2,	2070
L1,L1,L2,LLC,	572
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	25
L1,L1,LLC,L2,	121
L1,L1,LLC,LLC,	127
L1,L2,L1,L1,	14
L1,L2,L1,L2,	20
L1,L2,L1,LLC,	8
L1,L2,L2,L1,	14
L1,L2,L2,L2,	41
L1,L2,L2,LLC,	13
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	5
L1,L2,LLC,LLC,	7
L1,LLC,L1,L1,	8
L1,LLC,L1,L2,	1
L1,LLC,L1,LLC,	1
L1,LLC,L2,L2,	9
L1,LLC,L2,LLC,	4
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	15
L2,L1,L1,L2,	25
L2,L1,L1,LLC,	2
L2,L1,L2,L1,	8
L2,L1,L2,L2,	84
L2,L1,L2,LLC,	17
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	5
L2,L1,LLC,LLC,	5
L2,L2,L1,L1,	2
L2,L2,L1,L2,	7
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	2
L2,L2,L2,L2,	24
L2,L2,L2,LLC,	12
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	2
L2,LLC,L2,L2,	1
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	6
LLC,L1,L1,L2,	5
LLC,L1,L1,LLC,	3
LLC,L1,L2,L1,	3
LLC,L1,L2,L2,	16
LLC,L1,L2,LLC,	7
LLC,L1,LLC,L2,	2
LLC,L1,LLC,LLC,	1
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	1
LLC,L2,L2,L2,	4
LLC,L2,L2,LLC,	3
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	2
LLC,LLC,L2,LLC,	2




Heartbeat. 20971520 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       13,965,228
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        5,884,394
    Misses:                        817,497
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.20%
  TLB-LL stats:
    Hits:                          803,063
    Misses:                         14,434
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  1.77%
    Child hits:                 19,849,622
    Total miss rate:                  0.07%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       14,359,110
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        5,880,176
    Misses:                        870,389
    Invalidations:                       0
    Hits PT level     1:              14,344
    Misses PT level   1:                 290
    Hits PT level     2:              14,406
    Misses PT level   2:                 228
    Hits PT level     3:               9,600
    Misses PT level   3:               5,034
    Hits PT level     4:               8,594
    Misses PT level   4:               6,040
    Prefetch hits:                 229,973
    Prefetch misses:               652,008
    Miss rate:                       12.89%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           47,894
    Misses:                        822,497
    Invalidations:                       0
    Hits PT level     1:                 226
    Misses PT level   1:                  64
    Hits PT level     2:                 193
    Misses PT level   2:                  35
    Hits PT level     3:               4,642
    Misses PT level   3:                 392
    Hits PT level     4:               4,509
    Misses PT level   4:               1,531
    Prefetch hits:                  34,434
    Prefetch misses:               617,574
    Local miss rate:                 94.50%
    Child hits:                 20,516,203
    Total miss rate:                  3.85%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          673,379
    Misses:                        149,118
    Invalidations:                       0
    Hits PT level     1:                  64
    Misses PT level   1:                   0
    Hits PT level     2:                  35
    Misses PT level   2:                   0
    Hits PT level     3:                 392
    Misses PT level   3:                   0
    Hits PT level     4:               1,507
    Misses PT level   4:                  24
    Prefetch hits:                 467,626
    Prefetch misses:               149,948
    Local miss rate:                 18.13%
    Child hits:                     91,898
    Total miss rate:                 16.31%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 20971520
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	23
L1,L1,L1,L1,	7712
L1,L1,L1,L2,	1527
L1,L1,L1,LLC,	466
L1,L1,L2,MEMORY,	7
L1,L1,L2,L1,	1005
L1,L1,L2,L2,	2589
L1,L1,L2,LLC,	795
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	34
L1,L1,LLC,L2,	166
L1,L1,LLC,LLC,	154
L1,L2,L1,L1,	15
L1,L2,L1,L2,	20
L1,L2,L1,LLC,	8
L1,L2,L2,L1,	14
L1,L2,L2,L2,	42
L1,L2,L2,LLC,	13
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	5
L1,L2,LLC,LLC,	7
L1,LLC,L1,L1,	8
L1,LLC,L1,L2,	1
L1,LLC,L1,LLC,	1
L1,LLC,L2,L2,	10
L1,LLC,L2,LLC,	4
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	15
L2,L1,L1,L2,	25
L2,L1,L1,LLC,	2
L2,L1,L2,L1,	8
L2,L1,L2,L2,	85
L2,L1,L2,LLC,	17
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	5
L2,L1,LLC,LLC,	5
L2,L2,L1,L1,	2
L2,L2,L1,L2,	7
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	2
L2,L2,L2,L2,	24
L2,L2,L2,LLC,	12
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	2
L2,LLC,L2,L2,	1
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	6
LLC,L1,L1,L2,	5
LLC,L1,L1,LLC,	3
LLC,L1,L2,L1,	5
LLC,L1,L2,L2,	16
LLC,L1,L2,LLC,	9
LLC,L1,LLC,L2,	3
LLC,L1,LLC,LLC,	1
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	1
LLC,L2,L2,L2,	4
LLC,L2,L2,LLC,	3
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	3
LLC,LLC,L2,LLC,	2




Heartbeat. 25165824 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       16,807,945
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        7,069,386
    Misses:                        983,194
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.21%
  TLB-LL stats:
    Hits:                          964,970
    Misses:                         18,224
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  1.85%
    Child hits:                 23,877,331
    Total miss rate:                  0.07%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       17,254,982
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        7,029,300
    Misses:                      1,071,954
    Invalidations:                       0
    Hits PT level     1:              18,061
    Misses PT level   1:                 363
    Hits PT level     2:              18,131
    Misses PT level   2:                 293
    Hits PT level     3:              12,107
    Misses PT level   3:               6,317
    Hits PT level     4:              10,753
    Misses PT level   4:               7,671
    Prefetch hits:                 283,710
    Prefetch misses:               802,888
    Miss rate:                       13.23%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           58,045
    Misses:                      1,013,911
    Invalidations:                       0
    Hits PT level     1:                 284
    Misses PT level   1:                  79
    Hits PT level     2:                 254
    Misses PT level   2:                  39
    Hits PT level     3:               5,830
    Misses PT level   3:                 487
    Hits PT level     4:               5,724
    Misses PT level   4:               1,947
    Prefetch hits:                  42,425
    Prefetch misses:               760,463
    Local miss rate:                 94.59%
    Child hits:                 24,627,044
    Total miss rate:                  3.95%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          833,862
    Misses:                        180,049
    Invalidations:                       0
    Hits PT level     1:                  79
    Misses PT level   1:                   0
    Hits PT level     2:                  39
    Misses PT level   2:                   0
    Hits PT level     3:                 487
    Misses PT level   3:                   0
    Hits PT level     4:               1,921
    Misses PT level   4:                  26
    Prefetch hits:                 579,255
    Prefetch misses:               181,208
    Local miss rate:                 17.76%
    Child hits:                    112,562
    Total miss rate:                 15.98%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 25165824
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	23
L1,L1,L1,L1,	9617
L1,L1,L1,L2,	1963
L1,L1,L1,LLC,	599
L1,L1,L2,MEMORY,	9
L1,L1,L2,L1,	1232
L1,L1,L2,L2,	3275
L1,L1,L2,LLC,	1007
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	40
L1,L1,LLC,L2,	207
L1,L1,LLC,LLC,	193
L1,L2,L1,L1,	20
L1,L2,L1,L2,	23
L1,L2,L1,LLC,	9
L1,L2,L2,L1,	15
L1,L2,L2,L2,	53
L1,L2,L2,LLC,	21
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	5
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	8
L1,LLC,L1,L2,	1
L1,LLC,L1,LLC,	1
L1,LLC,L2,L2,	10
L1,LLC,L2,LLC,	4
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	21
L2,L1,L1,L2,	29
L2,L1,L1,LLC,	3
L2,L1,L2,L1,	11
L2,L1,L2,L2,	92
L2,L1,L2,LLC,	22
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	6
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	9
LLC,L1,L1,L2,	6
LLC,L1,L1,LLC,	4
LLC,L1,L2,L1,	5
LLC,L1,L2,L2,	19
LLC,L1,L2,LLC,	9
LLC,L1,LLC,L2,	3
LLC,L1,LLC,LLC,	2
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	4
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	3
LLC,LLC,L2,LLC,	3
LLC,LLC,LLC,LLC,	1




Heartbeat. 29360128 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       19,657,360
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        8,252,708
    Misses:                      1,143,871
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.17%
  TLB-LL stats:
    Hits:                        1,121,411
    Misses:                         22,460
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  1.96%
    Child hits:                 27,910,068
    Total miss rate:                  0.08%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       20,161,181
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        8,194,916
    Misses:                      1,250,337
    Invalidations:                       0
    Hits PT level     1:              22,291
    Misses PT level   1:                 369
    Hits PT level     2:              22,343
    Misses PT level   2:                 317
    Hits PT level     3:              15,181
    Misses PT level   3:               7,479
    Hits PT level     4:              13,332
    Misses PT level   4:               9,328
    Prefetch hits:                 329,521
    Prefetch misses:               938,309
    Miss rate:                       13.24%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           66,689
    Misses:                      1,183,650
    Invalidations:                       0
    Hits PT level     1:                 286
    Misses PT level   1:                  83
    Hits PT level     2:                 271
    Misses PT level   2:                  46
    Hits PT level     3:               6,913
    Misses PT level   3:                 566
    Hits PT level     4:               7,038
    Misses PT level   4:               2,290
    Prefetch hits:                  49,134
    Prefetch misses:               889,175
    Local miss rate:                 94.67%
    Child hits:                 28,758,765
    Total miss rate:                  3.94%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                          969,225
    Misses:                        214,425
    Invalidations:                       0
    Hits PT level     1:                  83
    Misses PT level   1:                   0
    Hits PT level     2:                  46
    Misses PT level   2:                   0
    Hits PT level     3:                 566
    Misses PT level   3:                   0
    Hits PT level     4:               2,263
    Misses PT level   4:                  27
    Prefetch hits:                 673,424
    Prefetch misses:               215,751
    Local miss rate:                 18.12%
    Child hits:                    130,331
    Total miss rate:                 16.32%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 29360128
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	23
L1,L1,L1,L1,	11993
L1,L1,L1,L2,	2523
L1,L1,L1,LLC,	729
L1,L1,L2,MEMORY,	10
L1,L1,L2,L1,	1427
L1,L1,L2,L2,	3962
L1,L1,L2,LLC,	1187
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	44
L1,L1,LLC,L2,	254
L1,L1,LLC,LLC,	219
L1,L2,L1,L1,	20
L1,L2,L1,L2,	24
L1,L2,L1,LLC,	9
L1,L2,L2,L1,	17
L1,L2,L2,L2,	63
L1,L2,L2,LLC,	24
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	6
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	10
L1,LLC,L1,L2,	3
L1,LLC,L1,LLC,	1
L1,LLC,L2,L2,	11
L1,LLC,L2,LLC,	6
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	21
L2,L1,L1,L2,	30
L2,L1,L1,LLC,	3
L2,L1,L2,L1,	11
L2,L1,L2,L2,	92
L2,L1,L2,LLC,	22
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	7
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	9
LLC,L1,L1,L2,	8
LLC,L1,L1,LLC,	4
LLC,L1,L2,L1,	5
LLC,L1,L2,L2,	20
LLC,L1,L2,LLC,	10
LLC,L1,LLC,L2,	3
LLC,L1,LLC,LLC,	2
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	4
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	3
LLC,LLC,L2,LLC,	3
LLC,LLC,LLC,LLC,	1




Heartbeat. 33554432 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       22,488,334
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                        9,446,490
    Misses:                      1,312,551
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.20%
  TLB-LL stats:
    Hits:                        1,284,169
    Misses:                         28,382
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  2.16%
    Child hits:                 31,934,824
    Total miss rate:                  0.09%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       23,055,034
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                        9,402,220
    Misses:                      1,405,495
    Invalidations:                       0
    Hits PT level     1:              28,209
    Misses PT level   1:                 373
    Hits PT level     2:              28,245
    Misses PT level   2:                 337
    Hits PT level     3:              20,090
    Misses PT level   3:               8,492
    Hits PT level     4:              17,947
    Misses PT level   4:              10,635
    Prefetch hits:                 369,890
    Prefetch misses:             1,055,442
    Miss rate:                       13.00%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           75,179
    Misses:                      1,330,318
    Invalidations:                       0
    Hits PT level     1:                 287
    Misses PT level   1:                  86
    Hits PT level     2:                 285
    Misses PT level   2:                  52
    Hits PT level     3:               7,839
    Misses PT level   3:                 653
    Hits PT level     4:               8,020
    Misses PT level   4:               2,615
    Prefetch hits:                  55,128
    Prefetch misses:             1,000,314
    Local miss rate:                 94.65%
    Child hits:                 32,921,635
    Total miss rate:                  3.88%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                        1,088,111
    Misses:                        242,207
    Invalidations:                       0
    Hits PT level     1:                  86
    Misses PT level   1:                   0
    Hits PT level     2:                  52
    Misses PT level   2:                   0
    Hits PT level     3:                 653
    Misses PT level   3:                   0
    Hits PT level     4:               2,585
    Misses PT level   4:                  30
    Prefetch hits:                 755,885
    Prefetch misses:               244,429
    Local miss rate:                 18.21%
    Child hits:                    146,738
    Total miss rate:                 16.40%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 33554432
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	23
L1,L1,L1,L1,	16401
L1,L1,L1,L2,	2902
L1,L1,L1,LLC,	843
L1,L1,L2,MEMORY,	13
L1,L1,L2,L1,	1620
L1,L1,L2,L2,	4505
L1,L1,L2,LLC,	1361
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	56
L1,L1,LLC,L2,	298
L1,L1,LLC,LLC,	249
L1,L2,L1,L1,	21
L1,L2,L1,L2,	28
L1,L2,L1,LLC,	10
L1,L2,L2,L1,	18
L1,L2,L2,L2,	67
L1,L2,L2,LLC,	25
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	7
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	10
L1,LLC,L1,L2,	4
L1,LLC,L1,LLC,	1
L1,LLC,L2,L2,	13
L1,LLC,L2,LLC,	8
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	21
L2,L1,L1,L2,	30
L2,L1,L1,LLC,	3
L2,L1,L2,L1,	11
L2,L1,L2,L2,	93
L2,L1,L2,LLC,	22
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	7
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	9
LLC,L1,L1,L2,	9
LLC,L1,L1,LLC,	4
LLC,L1,L2,L1,	5
LLC,L1,L2,L2,	20
LLC,L1,L2,LLC,	10
LLC,L1,LLC,L2,	3
LLC,L1,LLC,LLC,	2
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	5
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	4
LLC,LLC,L2,LLC,	3
LLC,LLC,LLC,LLC,	1




Heartbeat. 37748736 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       25,321,847
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                       10,641,637
    Misses:                      1,477,321
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.19%
  TLB-LL stats:
    Hits:                        1,445,262
    Misses:                         32,059
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  2.17%
    Child hits:                 35,963,484
    Total miss rate:                  0.09%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       25,949,907
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                       10,601,070
    Misses:                      1,566,562
    Invalidations:                       0
    Hits PT level     1:              31,869
    Misses PT level   1:                 390
    Hits PT level     2:              31,919
    Misses PT level   2:                 340
    Hits PT level     3:              22,766
    Misses PT level   3:               9,493
    Hits PT level     4:              20,272
    Misses PT level   4:              11,987
    Prefetch hits:                 411,408
    Prefetch misses:             1,177,364
    Miss rate:                       12.87%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           83,204
    Misses:                      1,483,360
    Invalidations:                       0
    Hits PT level     1:                 297
    Misses PT level   1:                  93
    Hits PT level     2:                 286
    Misses PT level   2:                  54
    Hits PT level     3:               8,779
    Misses PT level   3:                 714
    Hits PT level     4:               9,070
    Misses PT level   4:               2,917
    Prefetch hits:                  61,374
    Prefetch misses:             1,115,990
    Local miss rate:                 94.69%
    Child hits:                 37,069,211
    Total miss rate:                  3.84%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                        1,211,995
    Misses:                        271,365
    Invalidations:                       0
    Hits PT level     1:                  93
    Misses PT level   1:                   0
    Hits PT level     2:                  54
    Misses PT level   2:                   0
    Hits PT level     3:                 714
    Misses PT level   3:                   0
    Hits PT level     4:               2,886
    Misses PT level   4:                  31
    Prefetch hits:                 841,869
    Prefetch misses:               274,121
    Local miss rate:                 18.29%
    Child hits:                    163,010
    Total miss rate:                 16.48%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 37748736
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	23
L1,L1,L1,L1,	18512
L1,L1,L1,L2,	3359
L1,L1,L1,LLC,	947
L1,L1,L2,MEMORY,	14
L1,L1,L2,L1,	1825
L1,L1,L2,L2,	5068
L1,L1,L2,LLC,	1519
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	62
L1,L1,LLC,L2,	323
L1,L1,LLC,LLC,	278
L1,L2,L1,L1,	21
L1,L2,L1,L2,	28
L1,L2,L1,LLC,	10
L1,L2,L2,L1,	18
L1,L2,L2,L2,	67
L1,L2,L2,LLC,	25
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	7
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	10
L1,LLC,L1,L2,	4
L1,LLC,L1,LLC,	1
L1,LLC,L2,L2,	14
L1,LLC,L2,LLC,	8
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	23
L2,L1,L1,L2,	31
L2,L1,L1,LLC,	3
L2,L1,L2,L1,	12
L2,L1,L2,L2,	93
L2,L1,L2,LLC,	28
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	7
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	9
LLC,L1,L1,L2,	9
LLC,L1,L1,LLC,	4
LLC,L1,L2,L1,	5
LLC,L1,L2,L2,	22
LLC,L1,L2,LLC,	12
LLC,L1,LLC,L2,	4
LLC,L1,LLC,LLC,	2
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L1,LLC,	1
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	5
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	4
LLC,LLC,L2,LLC,	4
LLC,LLC,LLC,LLC,	1




Heartbeat. 41943040 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       28,162,842
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                       11,832,587
    Misses:                      1,638,802
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.17%
  TLB-LL stats:
    Hits:                        1,602,748
    Misses:                         36,054
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  2.20%
    Child hits:                 39,995,429
    Total miss rate:                  0.09%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       28,850,786
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                       11,788,225
    Misses:                      1,731,838
    Invalidations:                       0
    Hits PT level     1:              35,853
    Misses PT level   1:                 401
    Hits PT level     2:              35,911
    Misses PT level   2:                 343
    Hits PT level     3:              25,707
    Misses PT level   3:              10,547
    Hits PT level     4:              22,780
    Misses PT level   4:              13,474
    Prefetch hits:                 453,668
    Prefetch misses:             1,302,935
    Miss rate:                       12.81%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                           91,228
    Misses:                      1,640,612
    Invalidations:                       0
    Hits PT level     1:                 303
    Misses PT level   1:                  98
    Hits PT level     2:                 286
    Misses PT level   2:                  57
    Hits PT level     3:               9,772
    Misses PT level   3:                 775
    Hits PT level     4:              10,248
    Misses PT level   4:               3,226
    Prefetch hits:                  67,797
    Prefetch misses:             1,235,138
    Local miss rate:                 94.73%
    Child hits:                 41,212,930
    Total miss rate:                  3.82%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                        1,337,535
    Misses:                        303,077
    Invalidations:                       0
    Hits PT level     1:                  98
    Misses PT level   1:                   0
    Hits PT level     2:                  57
    Misses PT level   2:                   0
    Hits PT level     3:                 775
    Misses PT level   3:                   0
    Hits PT level     4:               3,192
    Misses PT level   4:                  34
    Prefetch hits:                 928,975
    Prefetch misses:               306,163
    Local miss rate:                 18.47%
    Child hits:                    179,634
    Total miss rate:                 16.65%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 41943040
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,L1,MEMORY,	26
L1,L1,L1,L1,	20819
L1,L1,L1,L2,	3854
L1,L1,L1,LLC,	1074
L1,L1,L2,MEMORY,	14
L1,L1,L2,L1,	2023
L1,L1,L2,L2,	5708
L1,L1,L2,LLC,	1671
L1,L1,LLC,MEMORY,	2
L1,L1,LLC,L1,	62
L1,L1,LLC,L2,	358
L1,L1,LLC,LLC,	302
L1,L2,L1,L1,	21
L1,L2,L1,L2,	28
L1,L2,L1,LLC,	10
L1,L2,L2,L1,	18
L1,L2,L2,L2,	67
L1,L2,L2,LLC,	25
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	7
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	11
L1,LLC,L1,L2,	4
L1,LLC,L1,LLC,	2
L1,LLC,L2,L2,	14
L1,LLC,L2,LLC,	9
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	24
L2,L1,L1,L2,	35
L2,L1,L1,LLC,	3
L2,L1,L2,L1,	12
L2,L1,L2,L2,	94
L2,L1,L2,LLC,	28
L2,L1,LLC,L1,	3
L2,L1,LLC,L2,	7
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	10
LLC,L1,L1,L2,	10
LLC,L1,L1,LLC,	4
LLC,L1,L2,L1,	5
LLC,L1,L2,L2,	23
LLC,L1,L2,LLC,	12
LLC,L1,LLC,L2,	5
LLC,L1,LLC,LLC,	3
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L1,LLC,	1
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	5
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	4
LLC,LLC,L2,LLC,	4
LLC,LLC,LLC,LLC,	1




Heartbeat. 46137344 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       31,002,596
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                       13,021,164
    Misses:                      1,803,887
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.17%
  TLB-LL stats:
    Hits:                        1,763,398
    Misses:                         40,489
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  2.24%
    Child hits:                 44,023,760
    Total miss rate:                  0.09%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       31,747,128
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                       12,957,784
    Misses:                      1,915,941
    Invalidations:                       0
    Hits PT level     1:              40,257
    Misses PT level   1:                 432
    Hits PT level     2:              40,344
    Misses PT level   2:                 345
    Hits PT level     3:              28,985
    Misses PT level   3:              11,704
    Hits PT level     4:              25,738
    Misses PT level   4:              14,951
    Prefetch hits:                 502,243
    Prefetch misses:             1,441,130
    Miss rate:                       12.88%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                          100,329
    Misses:                      1,815,614
    Invalidations:                       0
    Hits PT level     1:                 321
    Misses PT level   1:                 111
    Hits PT level     2:                 286
    Misses PT level   2:                  59
    Hits PT level     3:              10,846
    Misses PT level   3:                 858
    Hits PT level     4:              11,415
    Misses PT level   4:               3,536
    Prefetch hits:                  74,974
    Prefetch misses:             1,366,156
    Local miss rate:                 94.76%
    Child hits:                 45,342,479
    Total miss rate:                  3.84%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                        1,482,627
    Misses:                        332,987
    Invalidations:                       0
    Hits PT level     1:                 111
    Misses PT level   1:                   0
    Hits PT level     2:                  59
    Misses PT level   2:                   0
    Hits PT level     3:                 857
    Misses PT level   3:                   1
    Hits PT level     4:               3,498
    Misses PT level   4:                  38
    Prefetch hits:               1,029,578
    Prefetch misses:               336,578
    Local miss rate:                 18.34%
    Child hits:                    198,171
    Total miss rate:                 16.54%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 46137344
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,MEMORY,L2,	1
L1,L1,L1,MEMORY,	28
L1,L1,L1,L1,	23546
L1,L1,L1,L2,	4262
L1,L1,L1,LLC,	1204
L1,L1,L2,MEMORY,	15
L1,L1,L2,L1,	2240
L1,L1,L2,L2,	6396
L1,L1,L2,LLC,	1818
L1,L1,LLC,MEMORY,	3
L1,L1,LLC,L1,	66
L1,L1,LLC,L2,	408
L1,L1,LLC,LLC,	328
L1,L2,L1,L1,	21
L1,L2,L1,L2,	28
L1,L2,L1,LLC,	10
L1,L2,L2,L1,	18
L1,L2,L2,L2,	67
L1,L2,L2,LLC,	25
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	7
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	11
L1,LLC,L1,L2,	5
L1,LLC,L1,LLC,	2
L1,LLC,L2,L2,	15
L1,LLC,L2,LLC,	9
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	25
L2,L1,L1,L2,	37
L2,L1,L1,LLC,	5
L2,L1,L2,L1,	14
L2,L1,L2,L2,	104
L2,L1,L2,LLC,	28
L2,L1,LLC,L1,	4
L2,L1,LLC,L2,	7
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	13
LLC,L1,L1,L2,	12
LLC,L1,L1,LLC,	4
LLC,L1,L2,L1,	8
LLC,L1,L2,L2,	27
LLC,L1,L2,LLC,	13
LLC,L1,LLC,L2,	5
LLC,L1,LLC,LLC,	3
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L1,LLC,	1
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	5
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	4
LLC,LLC,L2,LLC,	4
LLC,LLC,LLC,LLC,	1




Heartbeat. 50331648 memrequests processed.
TLB simulation results:
Core #0 (1 thread(s))
  TLB-L1I stats:
    Hits:                       33,849,722
    Misses:                              0
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  TLB-L1D stats:
    Hits:                       14,210,401
    Misses:                      1,960,940
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                       12.13%
  TLB-LL stats:
    Hits:                        1,915,578
    Misses:                         45,362
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Local miss rate:                  2.31%
    Child hits:                 48,060,123
    Total miss rate:                  0.09%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cache simulation results:
Core #0 (1 thread(s))
  L1I stats:
    Warmup hits:                   103,844
    Warmup misses:                       7
    Hits:                       34,651,638
    Misses:                              2
    Invalidations:                       0
    Hits PT level     1:                   0
    Misses PT level   1:                   0
    Hits PT level     2:                   0
    Misses PT level   2:                   0
    Hits PT level     3:                   0
    Misses PT level   3:                   0
    Hits PT level     4:                   0
    Misses PT level   4:                   0
    Miss rate:                        0.00%
  L1D stats:
    Warmup hits:                    39,248
    Warmup misses:                   8,424
    Hits:                       14,128,576
    Misses:                      2,091,439
    Invalidations:                       0
    Hits PT level     1:              45,129
    Misses PT level   1:                 433
    Hits PT level     2:              45,217
    Misses PT level   2:                 345
    Hits PT level     3:              32,718
    Misses PT level   3:              12,844
    Hits PT level     4:              29,118
    Misses PT level   4:              16,444
    Prefetch hits:                 547,020
    Prefetch misses:             1,574,485
    Miss rate:                       12.89%
  L2 stats:
    Warmup hits:                         8
    Warmup misses:                   8,423
    Hits:                          108,901
    Misses:                      1,982,540
    Invalidations:                       0
    Hits PT level     1:                 321
    Misses PT level   1:                 112
    Hits PT level     2:                 286
    Misses PT level   2:                  59
    Hits PT level     3:              11,884
    Misses PT level   3:                 960
    Hits PT level     4:              12,588
    Misses PT level   4:               3,856
    Prefetch hits:                  81,425
    Prefetch misses:             1,493,060
    Local miss rate:                 94.79%
    Child hits:                 49,479,416
    Total miss rate:                  3.84%
Core #1 (0 thread(s))
Core #2 (0 thread(s))
Core #3 (0 thread(s))
LLC stats:
    Warmup hits:                         1
    Warmup misses:                   8,422
    Hits:                        1,615,720
    Misses:                        366,820
    Invalidations:                       0
    Hits PT level     1:                 112
    Misses PT level   1:                   0
    Hits PT level     2:                  59
    Misses PT level   2:                   0
    Hits PT level     3:                 959
    Misses PT level   3:                   1
    Hits PT level     4:               3,814
    Misses PT level   4:                  42
    Prefetch hits:               1,122,363
    Prefetch misses:               370,697
    Local miss rate:                 18.50%
    Child hits:                    215,405
    Total miss rate:                 16.69%
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
num_requests : 50331648
num_not_found : 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MEMORY,MEMORY,MEMORY,MEMORY,	2
L1,MEMORY,MEMORY,MEMORY,	1
L1,L1,MEMORY,MEMORY,	4
L1,L1,MEMORY,L2,	1
L1,L1,L1,MEMORY,	30
L1,L1,L1,L1,	26648
L1,L1,L1,L2,	4759
L1,L1,L1,LLC,	1335
L1,L1,L2,MEMORY,	17
L1,L1,L2,L1,	2508
L1,L1,L2,L2,	7021
L1,L1,L2,LLC,	1961
L1,L1,LLC,MEMORY,	3
L1,L1,LLC,L1,	76
L1,L1,LLC,L2,	459
L1,L1,LLC,LLC,	369
L1,L2,L1,L1,	21
L1,L2,L1,L2,	28
L1,L2,L1,LLC,	10
L1,L2,L2,L1,	18
L1,L2,L2,L2,	67
L1,L2,L2,LLC,	25
L1,L2,LLC,L1,	1
L1,L2,LLC,L2,	7
L1,L2,LLC,LLC,	8
L1,LLC,L1,L1,	11
L1,LLC,L1,L2,	5
L1,LLC,L1,LLC,	2
L1,LLC,L2,L2,	15
L1,LLC,L2,LLC,	9
L1,LLC,LLC,L2,	1
L1,LLC,LLC,LLC,	1
L2,L1,L1,L1,	25
L2,L1,L1,L2,	37
L2,L1,L1,LLC,	5
L2,L1,L2,L1,	14
L2,L1,L2,L2,	104
L2,L1,L2,LLC,	28
L2,L1,LLC,L1,	4
L2,L1,LLC,L2,	7
L2,L1,LLC,LLC,	7
L2,L2,L1,L1,	2
L2,L2,L1,L2,	14
L2,L2,L1,LLC,	5
L2,L2,L2,L1,	4
L2,L2,L2,L2,	35
L2,L2,L2,LLC,	17
L2,L2,LLC,L2,	5
L2,L2,LLC,LLC,	4
L2,LLC,L2,L2,	3
L2,LLC,LLC,LLC,	1
LLC,L1,L1,L1,	13
LLC,L1,L1,L2,	12
LLC,L1,L1,LLC,	5
LLC,L1,L2,L1,	8
LLC,L1,L2,L2,	27
LLC,L1,L2,LLC,	13
LLC,L1,LLC,L2,	5
LLC,L1,LLC,LLC,	3
LLC,L2,L1,L1,	2
LLC,L2,L1,L2,	2
LLC,L2,L1,LLC,	1
LLC,L2,L2,L1,	1
LLC,L2,L2,L2,	5
LLC,L2,L2,LLC,	4
LLC,L2,LLC,L2,	1
LLC,LLC,L1,MEMORY,	1
LLC,LLC,L1,L1,	1
LLC,LLC,L1,L2,	1
LLC,LLC,L2,L2,	4
LLC,LLC,L2,LLC,	4
LLC,LLC,LLC,LLC,	1




Interrupted: exiting.
 