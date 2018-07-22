# obnubilate
Linux kernel module that decrypts an AES encrypted payload from a magic packet.

Registers a low level network handler to look for packets arriving on a closed
port that have certain flags set. When the right kind of packet is seen the
data payload is extracted and printed to the Kernel ring buffer.

The test file provided uses tiny-AES-c (https://github.com/kokke/tiny-AES-c) to
encrypt a string of exactly 16 characters and write the result to a file.
```
./test obnubilate_stuff ./x
```
The file can then be sent using hping3
```
sudo hping3 -V -S -p 444 -s 42420 -w 128 -d 42 -E ./x 10.0.0.10
using eth0, addr: 10.0.0.5, MTU: 65536
HPING 10.0.0.10: S set, 40 headers + 42 data bytes
--- localhost hping statistic ---
1 packets transmitted, 0 packets received, 100% packet loss
round-trip min/avg/max = 0.0/0.0/0.0 ms
```
When checking the Kernel ring buffer you should see the decrypted text
```
dmesg -T | tail -n1
[Sun Jul 22 08:00:40 2018] obnubilate_stuff
