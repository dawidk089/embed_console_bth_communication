#include <inttypes.h>
#include <winsock2.h> //winsock data type
#include <ws2bth.h> //bluetooth support data type

typedef struct {
	char * name;
	uint64_t address;
	uint32_t flags;
	uint32_t COD;
	GUID ServiceClassId;
	SOCKADDR_BTH SockAddr;
}BT_FOUND, *PBT_FOUND;

uint32_t find_bt_device(uint32_t * n_device, PBT_FOUND * pbt_list);
