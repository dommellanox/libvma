Use `vma_stats` in order to see VMA statistics in real time.


VMA Statistics
Usage:
> vma\_stats [-p pid] [-v view] [-d details] [-i interval]

Defaults:
> find\_pid=enabled, view=1, details=1, interval=1

Options:
> -p, --pid=`<pid>`               Show VMA statistics for proccess with pid: `<pid>`

> -n, --name=`<application>`      Show VMA statistics for application: `<application>`

> -f, --find\_pid                Find and show statistics for VMA instance running (default)

> -F, --forbid\_clean            By setting this flag inactive shared objects would not be removed

> -i, --interval=`<n>`            Print report every `<n>` seconds

> -v, --view=`<1|2|3|4>`          Set view type:1- basic info,2- extra info,3- full info,4- mc groups

> -d, --details=`<1|2>`           Set details mode:1- to see totals,2- to see deltas

> -z, --zero                    Zero counters

> -l, --log\_level=`<level>`       Set VMA log level to `<level>`(1 `<=` level `<=` 7)

> -D, --details\_level=`<level>`   Set VMA log details level to `<level>`(0 `<=` level `<=` 3)

> -s, --sockets=`<list|range>`    Log only sockets that match `<list>` or `<range>`, format: 4-16 or 1,9 (or combination)

> -V, --version                 Print version

> -h, --help                    Print this help message