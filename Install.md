# Installing VMA #

## Prerequisites ##

  * OS: MLNX\_OFED supported Linux x64 OS.

  * Mellanox ConnectX-3 network adapter

  * MLNX OFED 2.1-1.0.0

  * ConnectX-3 FW 2.30.8000 or newer

  * libnl1 software package (netlink)


## Installation ##

MLNX OFED should be installed using --vma option.

Starting from MLNX\_OFED 2.1, VMA will be installed automatically when using the --vma option.

For older MLNX\_OFED versions, install the VMA package:

For Ubuntu12.04:

`dpkg -i libvma.deb`

For other OS:

`rpm -ivh libvma.rpm`