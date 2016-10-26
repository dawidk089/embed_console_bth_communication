#include "finder.h"
#include "gc.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_BT_FIND 50

uint32_t find_bt_device(uint32_t * n_device, PBT_FOUND * pbt_list) {
	//*** deklaracja zmiennych ***//
	//rutynowe
	uint8_t i_device = 0;
	bool is_again;

	//WSALookupService
	HANDLE hLookup = NULL;
	DWORD required_size = sizeof(WSAQUERYSET);
	PWSAQUERYSET pWSAQuerySet = NULL;
	int32_t res_status;

	//*** WSALookupService ***//
	//alokacja struktury WSAQuerySet (informacja o znalezionym urzadzeniu)
	pWSAQuerySet = (PWSAQUERYSET)calloc(1, sizeof(WSAQUERYSET));
	if (pWSAQuerySet == NULL) {
		return 0x20000002;
	}
	gc_add((void**)&pWSAQuerySet);

	//sprawdzenie poprawnosci podanych argumentow funkcji
	if (*n_device != 0 || (*pbt_list) != NULL) {
		return 0x20000004;
	}

	//WSALookupServiceBegin
#define __PERFORM_WSALookupServiceEnd()							\
	{															\
		uint32_t res_status = WSALookupServiceEnd(hLookup);		\
		if (res_status != 0) return GetLastError();				\
	}

	pWSAQuerySet->dwNameSpace = NS_BTH;
	pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
	pWSAQuerySet->lpBlob = NULL;

	res_status = WSALookupServiceBegin(pWSAQuerySet, LUP_CONTAINERS | LUP_FLUSHCACHE, &hLookup);
	if (res_status == SOCKET_ERROR) {
		//__PERFORM_WSALookupServiceEnd();
		return GetLastError();
	}

	//petla po zadaniach wyszukania urzadzenia
	for (i_device = 0, is_again = false; i_device < MAX_BT_FIND; i_device++) {
		if (is_again) { --i_device; is_again = false; } //ponowienie wyszukania urzadzenia

		//WSALookupServiceNext
		memset (pWSAQuerySet, 0, required_size);
		res_status = WSALookupServiceNext(hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_RETURN_TYPE, &required_size, pWSAQuerySet);

		if (res_status == 0) {
			size_t length = 0;

			{//nowe otoczenie ze wzgledu na potrzebe przechowania starej wartosci wskaznika
				PBT_FOUND old_pbt_list = *pbt_list;
				//realokacja pamieci o kolejne znalezione urzadzenie
				*pbt_list = (PBT_FOUND)realloc((*pbt_list), (i_device + 1) * sizeof(BT_FOUND));
				if (*pbt_list == NULL) {
					__PERFORM_WSALookupServiceEnd();
					return 0x20000003;
				}
				if (i_device == 0) gc_add((void**)pbt_list);
				else gc_update((void**)&old_pbt_list, (void**)pbt_list);
			}

			//alokacja pamieci na napis -- nazwe urzadzenia
			length = strlen(pWSAQuerySet->lpszServiceInstanceName) + sizeof(L'\0');
			(*pbt_list)[i_device].name = (char *)calloc(length, sizeof(char));
			if ((*pbt_list)[i_device].name == NULL) {
				__PERFORM_WSALookupServiceEnd();
				return 0x20000002;
			}
			gc_add((void**)&((*pbt_list)[i_device].name));

			//kopia wybranych wlasciwosci urzadzenia
			strcpy((*pbt_list)[i_device].name, pWSAQuerySet->lpszServiceInstanceName);
			(*pbt_list)[i_device].address = ((SOCKADDR_BTH *)pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;
			(*pbt_list)[i_device].flags = pWSAQuerySet->dwOutputFlags;
			(*pbt_list)[i_device].COD = pWSAQuerySet->lpServiceClassId->Data1;
			memcpy(&(*pbt_list)[i_device].ServiceClassId, pWSAQuerySet->lpServiceClassId, sizeof(GUID));
			memcpy(&(*pbt_list)[i_device].SockAddr, (SOCKADDR_BTH *)pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr, sizeof((*pbt_list)[i_device].SockAddr)); //#SOCKADDR
			*n_device = i_device + 1;
			printf("GUID: 0x%I32X 0x%04X 0x%04X 0x%02X%02X%02X%02X%02X%02X%02X%02X \n",
                pWSAQuerySet->lpServiceClassId->Data1, pWSAQuerySet->lpServiceClassId->Data2, pWSAQuerySet->lpServiceClassId->Data3,
                pWSAQuerySet->lpServiceClassId->Data4[0], pWSAQuerySet->lpServiceClassId->Data4[1],
                pWSAQuerySet->lpServiceClassId->Data4[2], pWSAQuerySet->lpServiceClassId->Data4[3],
                pWSAQuerySet->lpServiceClassId->Data4[4], pWSAQuerySet->lpServiceClassId->Data4[5],
                pWSAQuerySet->lpServiceClassId->Data4[6], pWSAQuerySet->lpServiceClassId->Data4[7]);
            printf("GUID: 0x%I32X 0x%04X 0x%04X 0x%02X%02X%02X%02X%02X%02X%02X%02X \n",
                (*pbt_list)[i_device].ServiceClassId.Data1, (*pbt_list)[i_device].ServiceClassId.Data2, (*pbt_list)[i_device].ServiceClassId.Data3,
                (*pbt_list)[i_device].ServiceClassId.Data4[0], (*pbt_list)[i_device].ServiceClassId.Data4[1],
                (*pbt_list)[i_device].ServiceClassId.Data4[2], (*pbt_list)[i_device].ServiceClassId.Data4[3],
                (*pbt_list)[i_device].ServiceClassId.Data4[4], (*pbt_list)[i_device].ServiceClassId.Data4[5],
                (*pbt_list)[i_device].ServiceClassId.Data4[6], (*pbt_list)[i_device].ServiceClassId.Data4[7]);
		}
		else { //#WSALookupServiceNext failed
			res_status = GetLastError();
			if (res_status == WSA_E_NO_MORE) {
				break;
			}
			else if (res_status == WSAEFAULT) {

				{//nowe otoczenie ze wzgledu na potrzebe przechowania starej wartosci wskaznika
					PWSAQUERYSET old_pWSAQuerySet = pWSAQuerySet;
					pWSAQuerySet = (PWSAQUERYSET)realloc(pWSAQuerySet, required_size);
					if (pWSAQuerySet == NULL) {
						__PERFORM_WSALookupServiceEnd();
						return 0x20000004;
					}
					gc_update((void**)&old_pWSAQuerySet, (void**)&pWSAQuerySet);
				}

				is_again = true;
				continue;
			} //#WSAEFAULT
			__PERFORM_WSALookupServiceEnd();
			return res_status;
		} //#WSALookupServiceNext failed
	} //#for
	if (i_device == MAX_BT_FIND)
		res_status = 0x20000005;
	else if (i_device == 0)
		res_status = 0x20000006;
	else if (res_status == WSA_E_NO_MORE)
		res_status = 0;

	__PERFORM_WSALookupServiceEnd();
#undef __PERFORM_WSALookupServiceEnd
	return res_status;
}


