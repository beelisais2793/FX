## MTU

From Wikipedia:

    A larger MTU brings greater efficiency because each network packet carries more
    user data while protocol overheads, such as headers or underlying per-packet
    delays, remain fixed; the resulting higher efficiency means an improvement in
    bulk protocol throughput. A larger MTU also means processing of fewer packets
    for the same amount of data. In some systems, per-packet-processing can be a
    critical performance limitation.

Cuttlebone chops your state into chunks of N bytes. You are expected to choose
an N that less than the [MTU][] of your network. To discover this value for a
particular host use the ifconfig command. Here's an example:

    $ ifconfig
    lo0: flags=8049<UP,LOOPBACK,RUNNING,MULTICAST> mtu 16384
      options=3<RXCSUM,TXCSUM>
      inet6 ::1 prefixlen 128
      inet 127.0.0.1 netmask 0xff000000
      inet6 fe80::1%lo0 prefixlen 64 scopeid 0x1
      nd6 options=1<PERFORMNUD>
    gif0: flags=8010<POINTOPOINT,MULTICAST> mtu 1280
    stf0: flags=0<> mtu 1280
    en0: flags=8863<UP,BROADCAST,SMART,RUNNING,SIMPLEX,MULTICAST> mtu 1500
      ether 10:9a:dd:aa:e3:ea
      inet6 fe80::129a:ddff:feaa:e3ea%en0 prefixlen 64 scopeid 0x4
      inet 192.168.0.109 netmask 0xffffff00 broadcast 192.168.0.255
      nd6 options=1<PERFORMNUD>
      media: autoselect
      status: active
    p2p0: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> mtu 2304
      ether 02:9a:dd:aa:e3:ea
      media: autoselect
      status: inactive

Above, the interface named en0 is the primary ethernet interface which is
probably what you'll want to broadcast from. Note that it seems to have an
[MTU][] of 1500. This is a very common setting for ethernet interfaces. That's
why the default packet size for Cuttlebone is 1400.

If your network supports it, you may increase the [MTU][] to increase
efficiency and throughput. To accomplish this, you must first determine the
largest [MTU][] that your router will support. The largest I've seen is 9212.
This so called [Jumbo frame][] 

Overhead of several network layers, including Cuttlebone
  IP: 20 byte header
  UDP: 8 byte header
  CB: 16 byte header
  When you choose a "packet size" (N) in Cuttlebone, the actual
  broadcast/received packet will be N + IP + UDP + CB, or N + 44 bytes. XXX:
  confirm this with tcpdump and document how to confirm this.

## Determine MTU of your network

Use the tracepath command to discover the MTU on your network. Choose 2 hosts
on your network (say a.local and b.local) and use tracepath on each, to
eachother. Here's an example terminal exerpt:

    a $ tracepath -n 192.168.0.46
    1:  192.168.0.80                   0.086ms pmtu 1500
    1:  192.168.0.46                   0.388ms reached
    1:  192.168.0.46                   0.372ms reached
    Resume: pmtu 1500 hops 1 back 1

    b $ tracepath -n 192.168.0.80
    1:  192.168.0.46                   0.118ms pmtu 1500
    1:  192.168.0.80                   0.594ms reached
    1:  192.168.0.80                   0.334ms reached
    Resume: pmtu 1500 hops 1 back 64

The first exerpt is from a to b and the second is from b to a. Note the value
of pmtu (Path MTU) is 1500. This is typical of 100mb or gigabit ethernet. Your
network may or may not support increaing the MTU beyond 1500. Here's another
terminal exerpt:

    a $ tracepath -n 192.168.10.80
    1:  192.168.10.1                   0.080ms pmtu 9212
    1:  192.168.10.80                  0.232ms reached
    1:  192.168.10.80                  0.229ms reached
    Resume: pmtu 9212 hops 1 back 1

    b $ tracepath -n 192.168.10.1
    1:  192.168.10.80                  0.089ms pmtu 9212
    1:  192.168.10.1                   0.565ms reached
    1:  192.168.10.1                   0.312ms reached
    Resume: pmtu 9212 hops 1 back 64

In the excerpt above, hosts a and b and the router between them are configured
such that the MTU of the network is 9212. This allows so called "Jumbo frames"
to pass over the network, allowing for greater effiency. This configuration is
a manual process that depends on you router and host operating systems.

## Determine the maximum

  Use the ping command with flags that prohibit fragmentation to determine the
  largest payload you may send. ping will report an error if it cannot send the
  packet without fragmenation.

    on linux
      ping -M do -s <payloadSize> <hostName>
    on osx
      ping -D -s <payloadSize> <hostName>
    on windows
      ping -f -l <payloadSize> <hostName>

  references
    http://www.mylesgray.com/hardware/test-jumbo-frames-working/

Watch ping packets received on some host with this:

    sudo tcpdump -v -i eth10 icmp

## Configuring the network

To increase the maximum datagram (UDP packet) size on osx:

    sudo sysctl -w net.inet.raw.maxdgram=16384

how to use tcpdump
  http://danielmiessler.com/study/tcpdump/#basics

on linux
net.core.rmem_max = 16777216
net.core.wmem_max = 16777216
net.ipv4.tcp_rmem = 4096 87380 16777216
net.ipv4.tcp_wmem = 4096 65536 16777216
net.core.netdev_max_backlog = 250000

http://lists.apple.com/archives/macnetworkprog/2006/Jul/msg00018.html

MTU tuning....
http://synergy.cs.vt.edu/pubs/papers/hurwitz-ieeemicro2004-10gige.pdf


for osx...
  necessary? check with netstat -m while stressing...
  kern.ipc.maxsockbuf should be 16777216
  kern.ipc.maxsockbuf is/was 8388608
  sudo nvram boot-args="ncl=131072" # to increase network memory
  from here: https://rolande.wordpress.com/2014/05/17/performance-tuning-the-network-stack-on-mac-os-x-part-2/

## Random thoughts and notes

Maybe we should use RAW sockets because then you can set the DF bit on osx?
  http://www.tenouk.com/Module43a.html

[MTU]: http://en.wikipedia.org/wiki/Maximum_transmission_unit
[Jumbo frame]: http://en.wikipedia.org/wiki/Jumbo_frame
