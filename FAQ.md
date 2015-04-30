### FAQ ###



## Contiguous Pages or Huge Pages? ##

From our testing, contiguous pages generally give better performance than huge pages.

In addition, it can help avoiding memory issues due to huge-page reserved resources.

VMA\_MEM\_ALLOC\_TYPE is set to contiguous pages by default.


## Does VMA\_RX\_POLL value take effect for non-blocking socket? ##

No.

VMA\_RX\_POLL determine the number of times to poll on Rx path for ready packets before going to sleep for **blocking sockets only**.