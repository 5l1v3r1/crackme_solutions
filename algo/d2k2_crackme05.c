#include <windows.h>
extern void _stdcall d2k2_crackme05_hash(DWORD* input, DWORD input_len, DWORD* output);

void hexprint(unsigned char* pin,int buflength,unsigned char* str)
{
	const char * hex = "0123456789ABCDEF";
	char * pout = str;
	int i = 0;
	for (; i < buflength - 1; ++i) {
		*pout++ = hex[(*pin >> 4) & 0xF];
		*pout++ = hex[(*pin++) & 0xF];
	}
	*pout++ = hex[(*pin >> 4) & 0xF];
	*pout++ = hex[(*pin) & 0xF];
	*pout = 0;
}

void process_serial(char *name, char *serial_out)
{
	unsigned char hashinp[32] = { 0 };
	unsigned char d2k2_hashout[64] = { 0 };
	unsigned char d2k2hash_formatted[64] = { 0 };
	DWORD* hashbuf_ptr = (DWORD*)d2k2_hashout;
	
	memcpy(hashinp, name, strlen(name));
	int namelen = strlen(name);
	
	for (int ctr = 0;ctr < namelen; ctr++)
	{
		int letter = hashinp[ctr] + namelen;
		int letter_xor = *(DWORD *)hashinp;
		letter_xor ^= letter;
		hashinp[ctr] = (BYTE)letter_xor;
	}
	
	for (int ctr = 0; ctr != namelen; ctr++)
	{
		BYTE *hashbuf_ptr_b = (BYTE*)hashbuf_ptr;
		int magic1 = 0x10101010;
		int magic2 = 0x68F6B76C;
		magic1 = ((DWORD)magic1 & 0xFFFFFF00) | (DWORD)hashinp[ctr] & 0xFF;
		magic1 <<= 5;
		magic2 *= namelen;
		magic1 *= namelen;
		magic2 ^= magic1;
		magic2 += *(DWORD *)hashinp;
		*(DWORD *)hashinp = magic2;
		//call hash function
		d2k2_crackme05_hash((DWORD*)hashinp, namelen, hashbuf_ptr);
		hashbuf_ptr_b += 6;
		magic1 = *(DWORD*)hashbuf_ptr_b;
		magic2 ^= magic1;
		magic2 = _rotl(magic2, 7);
		hashbuf_ptr_b = (char*)hashbuf_ptr;
		hashbuf_ptr_b += 8;
		magic1 = *(DWORD*)hashbuf_ptr_b;
		magic2 ^= magic1;
		hashinp[ctr] = LOBYTE(magic2);
		d2k2_crackme05_hash((DWORD*)hashinp, namelen, hashbuf_ptr);
		hashbuf_ptr_b = (char*)hashbuf_ptr;
		hashbuf_ptr_b += 10;
		magic1 = *(DWORD*)hashbuf_ptr_b;
		magic2 ^= magic1;
		magic2 = _rotl(magic2, 4);
		hashinp[ctr] = LOBYTE(magic2);
		d2k2_crackme05_hash((DWORD*)hashinp, namelen, hashbuf_ptr);
	}
	hexprint(d2k2_hashout, 16, d2k2hash_formatted);
	wsprintf(serial_out, "%s", d2k2hash_formatted);
}