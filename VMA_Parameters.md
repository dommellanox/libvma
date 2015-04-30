### VMA Parameters ###

This wiki page will help you choose the correct VMA parameters for your application.

For the full list of VMA parameters, default values and explanations,  please see [README.txt](https://libvma.googlecode.com/git/README.txt).


Do not ignore any VMA errors/warnings as a result of changing VMA parameters! Some of your settings are probably problematic.


**Follow the descriptions which fit your application:**


## I only have offloaded traffic ##

If all of your traffic is offloaded, you can gain performance improvment by instructing VMA not to poll the OS for incoming traffic.

Use the following parameters:
  * VMA\_RX\_UDP\_POLL\_OS\_RATIO=0
  * VMA\_SELECT\_POLL\_OS\_RATIO=0


## Some of my traffic is not offloaded ##

If the not offloaded traffic have high priority, set the following parameters to low ratio (high polling):
  * VMA\_RX\_UDP\_POLL\_OS\_RATIO=1
  * VMA\_SELECT\_POLL\_OS\_RATIO=1


## I need to offloaded only specific traffic flows ##

You have several options to blacklist/whitelist traffic flows:
  * Use [libvma.conf](https://code.google.com/p/libvma/wiki/VMA_Configuration_File) for specifing which traffic flows should be offloaded.
  * Use [VMA extra API](https://code.google.com/p/libvma/source/browse/src/vma/vma_extra.h) - add\_conf\_rule() - for adding libvma.conf rules at run-time.
  * Use [VMA extra API](https://code.google.com/p/libvma/source/browse/src/vma/vma_extra.h) - thread\_offload() - to specify for each thread if sockets should be created as offloaded sockets or not.
  * Use the environment variable **VMA\_OFFLOADED\_SOCKETS** to specify if sockets should be created as offloaded/not-offloaded by default. It is recommended to use this with thread\_offload() extra API.


## My application is multithreaded ##

Currently,VMA is not handling well more than one thread per cpu core. The following instructions are mainly for applications with less threads than cores. Please see the end of this section for the case of several threads on the same core.


Currently, these instructions will benefit mainly applications which are using epoll. Application which are using select/poll will not benefit as much, or even suffer from performance degredation.

Please read first the explanation about VMA buffers in ["I need low memory usage"](VMA_Parameters#I_need_low_memory_usage.md).

**If you have less networking threads than cores**, use the following parameters:
  * VMA\_RING\_ALLOCATION\_LOGIC\_RX=20
  * VMA\_RING\_ALLOCATION\_LOGIC\_TX=20
  * VMA\_RX\_BUFS= #networking\_threads X 50000


Now, if VMA\_RX\_BUFS is too large (note that each buffer is of size VMA\_MTU [=1500]), you might want to change VMA\_RX\_WRE which will allow you to consume less memory.
A good VMA\_RX\_BUFS value will be = VMA\_RX\_WRE x #rings + some spare buffers. The number of rings is per offloaded interface and per thread (VMA\_RING\_ALLOCATION\_LOGIC=20).
For example, if you are accessing two interfaces (ib0, eth2) from each thread, and you have 8 threads, set VMA\_RX\_BUFS = (VMA\_RX\_WRE x (2\*8)) x 1.5.
If you want to have less rings in the system in order to save memory, you can use VMA\_RING\_LIMIT\_PER\_INTERFACE to limit the number of rings, but it is recommended to have a separate ring per thread/core.
You might have to use it if you have too many thread/cores.

You also might want to set VMA\_INTERNAL\_THREAD\_AFFINITY to a core on the close NUMA node that does not have something heavy running on (default is 0).

**If you have more threads than cores**, and you don't bind threads to cores, we suggest using:
  * VMA\_RING\_ALLOCATION\_LOGIC\_TX=31
  * VMA\_RING\_ALLOCATION\_LOGIC\_RX=31
  * VMA\_RX\_BUFS= #cores X 50000

**If you do bind threads to cores use 30 instead of 31.**


You may try using the option 30 or 31 even for less threads than cores.


Also, the parameter VMA\_THREAD\_MODE=3 might help, but it might make things worse, so you need to try and check.



## My application have thousands of sockets ##

You will need to enlarge the amount of VMA buffers.

For RX side, each socket might hold up to 2 X VMA\_RX\_WRE\_BATCHING (default: 64) unused buffers for each interface it is recieving from. Therfore, VMA\_RX\_BUFS (default: 200000) should be at least #sockets X VMA\_RX\_WRE\_BATCHING X 2 X #interfaces.

For TX side, each UDP socket might hold up to 8 unused buffers and each TCP socket might hold up to 16 unused buffers for each traffic flow it is sendting to. Therfore, VMA\_TX\_BUFS (default: 16000) should be at least #udp\_sockets X 8 X #traffic\_flows + #tcp\_sockets X 16.


## Many connections from a single client to a server listen socket ##

If you have a listen socket that accept many connections from the same source IP, you can improve performance by using VMA\_TCP\_3T\_RULES=1


## My machine have several numa nodes ##

You will probably see better performance if you bind your application to the closest numa node to the NIC.


First, check on which numa node your NIC is (you can use `lspci`), and which cpu cores are associated with this numa node (you can use `numactl --hardware`).

Then:
  * Set VMA\_INTERNAL\_THREAD\_AFFINITY=**_core\_id\_on\_the\_numa\_node_**
  * Use **taskset** to bind the application to the rest of the cores on this numa node


## My application was written in Java ##

Java is running over IPV6 by default.
VMA currently does not support IPV6, make sure you are running over IPV4:

`Java -Djava.net.preferIPv4Stack=true …`


## I need low memory usage ##

**You can control the amount of buffers in VMA using the following parameters:**
  * VMA\_RX\_BUFS
  * VMA\_TX\_BUFS

Use VMA\_RX\_BUFS to specifiy the amount of RX buffers for all interfaces and sockets in VMA.

The value of VMA\_RX\_BUFS must be gretaer than the number of offloaded interfaces multiplied by VMA\_RX\_WRE (VMA\_RX\_BUFS > num\_offloaded\_interfaces X VMA\_RX\_WRE). You will need to set VMA\_RX\_WRE accordingly.

The same goes for VMA\_TX\_BUFS and VMA\_TX\_WRE.

**The size of each buffer is determined by VMA\_MTU.**

If your application is only dealing with small packet sizes, you can lower VMA\_MTU in order to save memory.

**The total amount of memory consumed by VMA buffers is calculated by this formula:**
MEMORY = (VMA\_RX\_BUFS + VMA\_TX\_BUFS) x VMA\_MTU.


The default values of the involved parameters are:
  * VMA\_RX\_BUFS=200000
  * VMA\_TX\_BUFS=200000
  * VMA\_RX\_WRE=16000
  * VMA\_TX\_WRE=16000
  * VMA\_MTU=1500

## I need low CPU utilization ##

Although VMA is built to fully utilize CPU in its RX flows, you can use the following parameters in order to reduce it:
  * VMA\_RX\_POLL=0
  * VMA\_SELECT\_POLL=0

## I want to extract VMA logs ##

Use VMA\_TRACELEVEL to set the log level of messages printed to the screen. The default is 3, use 4 or 5 (or even 6 - not recommended) for more output. Each level will also print all lower levels.


Use VMA\_LOG\_FILE to redirect the log to a file.
If you use multithread application or severl VMA applications, use "%d" in the name of the log file in order for it to contain the relevent process/thread id. For example: VMA\_LOG\_FILE="/tmp/vmalog.%d".


Use VMA\_LOG\_DETAILS to control the details (thread-id, process-id and time) in every log message. Values are 0-3.


**Note that using high log levels will slow your application.** It can help if you redirect the log to a file.


## I want to extract VMA statistics ##

Use VMA\_STATS\_FILE to output VMA statistic to a file.

You can also see the statistics using `vma_stats`.


## Tuning IOMUX - epoll/select/poll ##

**VMA\_SELECT\_POLL**

While blocking, control the duration in which to poll the hardware on Rx path before going to sleep (pending an interrupt blocking or timeout on OS select(), poll() or epoll\_wait().

For best latency, use -1 for infinite polling.

Default value is 100000 (msec).


**VMA\_SELECT\_POLL\_OS\_RATIO**

Control the ratio in which VMA will poll the not-offloaded sockets.

Disable with 0 if all traffic is offloaded.

Set to high polling (low ratio) if not-offloaded traffic have high priority.

Default value is 10.


## Tuning RX calls ##


**VMA\_RX\_POLL**

For blocking sockets only, control the number of times to poll on RX path for ready packets before going to sleep (wait for interrupt in blocked mode).

For best latency, use -1 for infinite polling.

For low CPU usage use 1 for single poll.

Default value is 100000


## General tricks ##

  * Set VMA\_MTU / VMA\_MSS to your maximum packet size (sent and received). If your maximum packet size is small, this will save memory, might improve latency and give better cache utilization.