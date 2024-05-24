#include "blue-dream.h"

t_dword unpack_dword(t_byte *u, t_qword v)
{
	t_dword w, x, y, z;

	w = u[v + 0] << 24;
	x = u[v + 1] << 16;
	y = u[v + 2] << 8;
	z = u[v + 3];

	return w | x | y | z;
}

t_word unpack_word(t_byte *u, t_qword v)
{
	t_word x, y;

	x = u[v + 0] << 8;
	y = u[v + 1];

	return x | y;
}

t_dword lbyte(t_dword x) { return x & 0x00ff; }
t_dword hbyte(t_dword x) { return x & 0xff00; }
t_dword lword(t_dword x) { return x & 0x0000ffff; }
t_dword hword(t_dword x) { return x & 0xffff0000; }

t_dword bswap(t_dword x) { return hword(x) | lword(lbyte(x) << 8) | (hbyte(x) >> 8); }
t_dword wswap(t_dword x) { return (lword(x) << 16) | (hword(x) >> 16); }