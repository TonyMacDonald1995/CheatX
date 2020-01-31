#include "memfuncs.h"

struct freeze_entry *freeze_entries = NULL;
int freeze_entry_cnt = 0;

HRESULT __stdcall freeze_memory(LPCSTR szCommand, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc) {
	char code_address_buf[16], val_buf[16];
	DWORD code_address, val;
	
	sscanf(szCommand, "freezemem! %s %s", code_address_buf, val_buf);
	
	code_address = strtol(code_address_buf, NULL, 16);
	val = strtol(val_buf, NULL, 16);
	
	freeze_entry_cnt++;
	freeze_entries = realloc(freeze_entries, sizeof(struct freeze_entry) * freeze_entry_cnt);
	
	freeze_entries[freeze_entry_cnt-1].physical_addr = code_address;
	freeze_entries[freeze_entry_cnt-1].mapped_addr = MmMapIoSpace(code_address, 4, PAGE_READWRITE);
	freeze_entries[freeze_entry_cnt-1].val = val;
	
	return XBDM_NOERR;
}

HRESULT __stdcall poke_memory(LPCSTR szCommand, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc) {
	char code_address_buf[16], val_buf[16];
	DWORD code_address, val;
	
	sscanf(szCommand, "pokemem! %s %s", code_address_buf, val_buf);
	
	code_address = strtol(code_address_buf, NULL, 16);
	val = strtol(val_buf, NULL, 16);
	
	PVOID addr = MmMapIoSpace(code_address, 4, PAGE_READWRITE);
	*(DWORD*)addr = val;
	
	MmUnmapIoSpace(addr, 4);
	
	return XBDM_NOERR;
}

HRESULT __stdcall dump_memory(LPCSTR szCommand, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc) {
	FILE *fp = fopen("\\Device\\Harddisk0\\Partition1\\DEVKIT\\dxt\\memdump.bin", "wb");
	
	PVOID addr = MmMapIoSpace(PHYSICAL_ADDR_BASE, PHYSICAL_ADDR_SIZE, PAGE_READWRITE);
	
	fwrite(addr, PHYSICAL_ADDR_SIZE, 1, fp);
	
	MmUnmapIoSpace(addr, PHYSICAL_ADDR_SIZE);
	
	fclose(fp);
	
	return XBDM_NOERR;
}
