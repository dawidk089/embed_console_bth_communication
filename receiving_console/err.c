#include "err.h"
#include <inttypes.h>
#include <winsock2.h> //system errors macro
#include <stdio.h> //printf


void show_err(uint64_t status) {
	switch (status) {
	case WSASYSNOTREADY:
		printf("\terror code: WSASYSNOTREADY\n meaning: The underlying network subsystem is not ready for network communication.\n");
		break;
	case WSAVERNOTSUPPORTED:
		printf("\terror code: WSAVERNOTSUPPORTED\n meaning: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.\n");
		break;
	case WSAEINPROGRESS:
		printf("\terror code: WSAEINPROGRESS\n meaning: A blocking Windows Sockets 1.1 operation is in progress.\n");
		break;
	case WSAEPROCLIM:
		printf("\terror code: WSAEPROCLIM\n meaning: A limit on the number of tasks supported by the Windows Sockets implementation has been reached.\n");
		break;
	case WSAEFAULT:
		printf("\terror code: WSAEFAULT\n meaning: The lpWSAData parameter is not a valid pointer.\n");
		break;
	case WSANOTINITIALISED:
		printf("\terror code: WSANOTINITIALISED\n meaning: A successful WSAStartup call must occur before using this function.\n");
		break;
	case WSAENETDOWN:
		printf("\terror code: WSAENETDOWN\n meaning: The network subsystem has failed.\n");
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		printf("\terror code: WSA_NOT_ENOUGH_MEMORY\n meaning: There was insufficient memory to perform the operation.\n");
		break;
	case WSAEINVAL:
		printf("\terror code: WSAEINVAL\n meaning: One or more parameters were missing or invalid for this provider.\n");
		break;
	case WSANO_DATA:
		printf("\terror code: WSANO_DATA\n meaning: The name was found in the database but no data matching the given restrictions was located.\n");
		break;
	case WSASERVICE_NOT_FOUND:
		printf("\terror code: WSASERVICE_NOT_FOUND\n meaning: No such service is known. The service cannot be found in the specified name space.\n");
		break;
	case WSA_INVALID_HANDLE:
		printf("\terror code: WSA_INVALID_HANDLE\n meaning: The handle is not valid.\n");
		break;
		//bit 29 is reserved for application-defined error codes
		//no system error code has this bit set
		/*case 0x20000001:
		break;
		case 1: malloc*/
	case CALLOCFAILED:
		printf("\terror code: calloc allocation memory failed\n");
		break;
	case REALLOCFAILED:
		printf("\terror code: realloc allocation memory failed\n");
		break;
	case NOTINITERR:
		printf("\terror code: not init\n");
		break;
	case NOTFOUNDALLBTDEV:
		printf("\terror code: not found all devices\n");
		break;
	case NOTFOUNDANYBTDEV:
		printf("\terror code: no device is found\n");
		break;
	default:
		printf("\terror code: unexpected error code: %I64d\n", status);
	}
}
