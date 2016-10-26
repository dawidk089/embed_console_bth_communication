#include "finder.h"
#include "gc.h"
#include "err.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_BT_FIND 2

uint32_t err_code;
PBT_FOUND pbt_list;

PBT_FOUND find_bt_device(uint32_t * n_device) {
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
        err_code = 0x20000002;
        return NULL;
	}
	gc_add((void**)&pWSAQuerySet);

	//WSALookupServiceBegin
#define __PERFORM_WSALookupServiceEnd()							\
	{															\
		uint32_t res_status = WSALookupServiceEnd(hLookup);		\
		if (res_status != 0) err_code = GetLastError();		    \
	}

	pWSAQuerySet->dwNameSpace = NS_BTH;
	pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
	pWSAQuerySet->lpBlob = NULL;

	res_status = WSALookupServiceBegin(pWSAQuerySet, LUP_CONTAINERS | LUP_FLUSHCACHE, &hLookup);
	if (res_status == SOCKET_ERROR) {
        err_code = GetLastError();
        clear_bt_found();
        return NULL;
	}

	//petla po zadaniach wyszukania urzadzenia
	for (i_device = 0, is_again = false; i_device < MAX_BT_FIND; i_device++) {
		if (is_again) { --i_device; is_again = false; } //ponowienie wyszukania urzadzenia

		//WSALookupServiceNext
		memset (pWSAQuerySet, 0, required_size);
		res_status = WSALookupServiceNext(hLookup, LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_RETURN_TYPE, &required_size, pWSAQuerySet);

		if (res_status == 0) {
			size_t length = 0;

            //realokacja pamieci o kolejne znalezione urzadzenie
            pbt_list = realloc(pbt_list, (i_device + 1) * sizeof(BT_FOUND));
            if (pbt_list == NULL) {
                err_code = 0x20000003;
                __PERFORM_WSALookupServiceEnd();
                clear_bt_found();
                return NULL;
            }

			//alokacja pamieci na napis -- nazwe urzadzenia
			length = strlen(pWSAQuerySet->lpszServiceInstanceName) + sizeof(L'\0');
			pbt_list[i_device].name = (char *)calloc(length, sizeof(char));
			if (pbt_list[i_device].name == NULL) {
                err_code = 0x20000002;
				__PERFORM_WSALookupServiceEnd();
                clear_bt_found();
				return NULL;
			}
			gc_add((void**)&(pbt_list[i_device].name));

			//kopia wybranych wlasciwosci urzadzenia
			strcpy(pbt_list[i_device].name, pWSAQuerySet->lpszServiceInstanceName);
			pbt_list[i_device].address = ((SOCKADDR_BTH *)pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;
			pbt_list[i_device].flags = pWSAQuerySet->dwOutputFlags;
			pbt_list[i_device].COD = pWSAQuerySet->lpServiceClassId->Data1;
			memcpy(&pbt_list[i_device].ServiceClassId, pWSAQuerySet->lpServiceClassId, sizeof(GUID));
			memcpy(&pbt_list[i_device].SockAddr, (SOCKADDR_BTH *)pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr, sizeof(pbt_list[i_device].SockAddr)); //#SOCKADDR
			*n_device = i_device + 1;
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
						err_code = 0x20000004;
						__PERFORM_WSALookupServiceEnd();
                        clear_bt_found();
                        return NULL;
					}
					gc_update((void**)&old_pWSAQuerySet, (void**)&pWSAQuerySet);
				}

				is_again = true;
				continue;
			} //#WSAEFAULT
			err_code = res_status;
			__PERFORM_WSALookupServiceEnd();
            clear_bt_found();
            return NULL;
		} //#WSALookupServiceNext failed
	} //#for
	if (i_device == MAX_BT_FIND)
		res_status = 0x20000005;
	else if (i_device == 0)
		res_status = 0x20000006;
	else if (res_status == WSA_E_NO_MORE)
		res_status = 0;

    //return 0;
	err_code = res_status;
	__PERFORM_WSALookupServiceEnd();
    return pbt_list;
#undef __PERFORM_WSALookupServiceEnd
}

uint32_t get_finder_err(){
    return (!pbt_list && !err_code ? NOTINITERR : err_code);
}

void clear_bt_found(){
    if(pbt_list){
        free(pbt_list);
        pbt_list = NULL;
    }
}


