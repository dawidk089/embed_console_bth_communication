#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "err.h"
#include "param.h"
#include "finder.h"
#include "gc.h"
#include "stringcheck.h"

#include <winsock2.h>
#include <ws2bth.h>




#define FOUND_MOD_ADDDRESS (1<<2)
#define FOUND_MOD_NAME (1<<1)
#define FOUND_MOD_COD (1<<0)

//#define __DEBUG_FLAG

#ifdef __DEBUG_FLAG
#define __DEBUG(...) \
		printf(__VA_ARGS__);
#else
#define __DEBUG(...)
#endif


int main(int argc, char **argv) {
	//*** deklaracja zmiennych ***//
	//rutynowe
	uint8_t i;

	//parametry wywolania
	PARAM param;
	memset(&param, 0, sizeof(param));

	//winsock
	uint32_t ret_status = 0;
	WSADATA WSAData = { 0 };
	uint16_t winsock_ver = MAKEWORD(2, 2); //the highest to negotiate
	uint32_t n_device = 0;
	uint32_t ii_device = 0xFFFFFFFF;

	//info o urzadzeniach
	PBT_FOUND pdevices_info = NULL;

	//socket
	SOCKET client;
	SOCKADDR_BTH address;
	memset(&address, 0, sizeof(address));
	memset(&address, 0, sizeof(client));

	//*** rozpakowanie parametrow ***//
	//wyswietlenie pomocy jesli blad lub -h, wyjscie
#define __NSILENT					\
if((!param.silent && !param.verbose) || (param.verbose))

#define __VERBOSE					\
if(param.verbose)
	switch(get_param(argc, argv, &param)){
    case 0:
        break;
    case 0x10: //tylko __DEBUG
        break;
    case 0x11:
        __DEBUG("-> device name in get_params allocation memory failed\n");
        __NSILENT printf("> error memory access\n");
        __NSILENT show_err(0x20000002);
        gc_free();
        return 0x11; //powod: blad alokacji pamieci (filename)
        break;
    case 0x20:
    case 0x21:
    case 0x31:
    case 0x32:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x51:
    case 0x52:
    case 0x53:
    default:
        __NSILENT printf("> callpath argument error\n");
        __NSILENT show_help();
        gc_free();
        return 0x20;
    }

/*
	if (!get_param(argc, argv, &param)) {
		__DEBUG("-> get_param run failed\n");
		//bez znaczenia czy sie udalo czy nie
#define __NSILENT					\
if((!param.silent && !param.verbose) || (param.verbose))

#define __VERBOSE					\
if(param.verbose)

		__NSILENT
			printf("> ! incorrect call parameter\n\n");
		__NSILENT
		show_help();
		gc_free();
		return 0x20; //powod: blad pobrania parametrow
	}
    else*/
	if (param.help) {
		__NSILENT show_help();
		gc_free();
		return 0; //powod: wybrano help
	}


	//wyswietlanie podanych parametrow
	__VERBOSE
	printf("> parameters:\n");
	__VERBOSE
	printf(
		param.name == NULL
		? "\tdevice name was not specified\n"
		: "\tdevice name: %s\n", param.name
	);
	__VERBOSE
		printf(
			param.address == 0
			? "\tdevice address was not specified\n"
			: "\tdevice address: %I64d\n", param.address
	);
	__VERBOSE
		printf(
			param.cod == 0
			? "\tdevice cod was not specified\n"
			: "\tdevice cod: %I32d\n", param.cod
	);

	//brak wymaganych parametrow wywolania w trybie -s
	if (param.silent && !param.is_device_specified) {
		__DEBUG("-> lack of required parameters in mode <silent>\n");
		gc_free();
		return 0x21; //powod: niemozliwa kontynuacja w trybie silent ze wzgledu na brak wymaganych parametrow wywolania
	}

	//przygotowanie domyslnej nazwy pliku do zapisu
	__DEBUG("-> default filename creation\n");
	if (param.filename == NULL) {
		time_t t = time(NULL);
		struct tm date = *localtime(&t);

		//alokacja pamieci
		param.filename = (char*)calloc(32, sizeof(char));
		if (param.filename == NULL) {
			__DEBUG("-> filename allocation memory failed\n");
			__NSILENT	printf("> error memory access\n");
			__NSILENT	show_err(ret_status);
			gc_free();
			return 0x11; //powod: blad alokacji pamieci (filename)
		}
		gc_add((void**)&(param.filename));

		//skladanie nazwy
		sprintf(param.filename, "data_%02d%02d%02d_%02d%02d%02d.txt", date.tm_mday, date.tm_mon+1 , (date.tm_year+1900)%100, date.tm_hour, date.tm_min, date.tm_sec);
		__VERBOSE
			printf("> filename: %s\n", param.filename);
	}

	//*** wyszukiwanie urzadzen ***//

	//inicjacja winsock
#define __PERFORM_WSACleanup()						\
	{												\
		int32_t res_status = WSACleanup();			\
		__DEBUG(									\
			res_status == SOCKET_ERROR				\
			? "-> WSACleanup run failed\n"			\
			: "-> WSACleanup run success\n"			\
		);											\
		if (res_status == SOCKET_ERROR) {			\
__NSILENT	show_err(WSAGetLastError());			\
			return 0x32; /*powod: blad WSACleanup*/	\
		}											\
	}

	__VERBOSE
		printf("> choosen version Windows Sockets: %d.%d.\n", HIBYTE(winsock_ver), LOBYTE(winsock_ver));
	ret_status = WSAStartup(winsock_ver, &WSAData);

	__DEBUG(
		ret_status == 0
		? "-> WSAStartup run success\n"
		: "-> WSAStartup run failed\n"
	);

	if (ret_status != 0) {
		__NSILENT
			show_err(ret_status);
		gc_free();
		return 0x31;  //powod: blad WSAStartup
	}

	__VERBOSE
		printf(
			"> expects version Windows Sockets: %d.%d\n"
			"> the highest version Windows Sockets supported: %d.%d\n",
			HIBYTE(WSAData.wVersion), LOBYTE(WSAData.wVersion),
			HIBYTE(WSAData.wHighVersion), LOBYTE(WSAData.wHighVersion)
		);

	//wyszukiwanie urzadzen
    __VERBOSE printf("> device discovering -- wait 2 to 15s\n");
	ret_status = find_bt_device(&n_device, &pdevices_info);
	switch (ret_status) {
	case 0:
		__VERBOSE printf("> finding device completed successsfully\n");
		break;
	case 0x20000005:
	    __NSILENT show_err(ret_status);
	    break;
	case 0x20000006:
		__NSILENT show_err(ret_status);
		__PERFORM_WSACleanup();
		gc_free();
		return 0x41;
	default:
		__DEBUG("-> find_bt_device run failed\n");
		__NSILENT printf("> search devices failed\n");
		if(ret_status == WSASERVICE_NOT_FOUND){
		    __NSILENT printf("> service found nothing or service has not been found\n");
		     __VERBOSE show_err(ret_status);
            __PERFORM_WSACleanup();
            gc_free();
            return 0x43; //powod: brak service
		}
        __NSILENT show_err(ret_status);
        __PERFORM_WSACleanup();
        gc_free();
        return 0x40; //powod: blad podczas wyszukiwania urzadzen
	}

	//*** analiza znalezionych urzadzen ***//

	//wylistownie znalezionych urzadzen
	if (param.verbose || !param.is_device_specified) {
		printf("\n");
		for (i = 0; i < n_device; i++) {
            printf("\n");
			printf("Device found number: %d\n", i);
			printf("\tName: %s\n", pdevices_info[i].name);
			printf("\tAddress: %04I64X:%02I64X:%06I64Xh\n", GET_NAP(pdevices_info[i].address), (GET_SAP(pdevices_info[i].address) & 0xFF000000) >> (6 * 4), GET_SAP(pdevices_info[i].address) & 0xFFFFFF);
			printf("\tIs it connected?: %s\n", ((pdevices_info[i].flags)&BTHNS_RESULT_DEVICE_CONNECTED ? "YES" : "NO"));
			printf("\tIs it remembered?: %s\n", ((pdevices_info[i].flags)&BTHNS_RESULT_DEVICE_REMEMBERED ? "YES" : "NO"));
			printf("\tIs it authenticated?: %s\n", ((pdevices_info[i].flags)&BTHNS_RESULT_DEVICE_AUTHENTICATED ? "YES" : "NO"));
			printf("\tCOD: %I32d\n", pdevices_info[i].COD);
		}
		printf("\n");
	}

	//pobranie wybranego numeru urzadzenia
	if (!param.is_device_specified) {
		char buffer[64];
		uint32_t i_device = 0;

		printf(">> type number of device or 'e' to end: ");
		fgets(buffer, sizeof(buffer), stdin);

		if (strlen(buffer) == 1 && buffer[0] == '\n') {
			__DEBUG("-> error due to fgets return empty string\n");
			__NSILENT	printf("> problem with receiving answer\n");
			gc_free();
			__PERFORM_WSACleanup();
			return 0x51; //powod: pusty napis
		}
		buffer[strlen(buffer) - 1] = '\0'; //usuwanie \n na \0
		if (strcmp(buffer, "e") == 0 || strcmp(buffer, "end") == 0) {
			gc_free();
			__PERFORM_WSACleanup();
			return 0; //powod: uzytkownik zazadal zakonczenia programu
		}
		if (!is_dec_number(buffer)) {
			__NSILENT	printf("> answers is not a number\n");
			gc_free();
			__PERFORM_WSACleanup()
				return 0x52; //powod: podano nieporawna wartosc -- odp nie jest liczba
		}
		i_device = strtol(buffer, NULL, 10);
		if (i_device > n_device - 1) {
			__NSILENT	printf("> device with this number is not listed\n");
			gc_free();
			__PERFORM_WSACleanup()
				return 0x53; //powod: podano niepoprawna wartosc -- odp nie jest liczba z dozwolonego zakresu
		}
		param.address = pdevices_info[i_device].address;
		ii_device = i_device;
	}
	else { //sprawdzenie czy znaleziono podane urzadzenie
		uint8_t * found_mod = NULL;
		uint8_t val_max_found_mod = 0;
		uint8_t i_max_found_mod = 0xFF;
		found_mod = (uint8_t *)calloc(n_device, sizeof(uint8_t));
		if (found_mod == NULL) {
			__DEBUG("-> found_mod allocation memory failed\n");
			__NSILENT	printf("> error memory access\n");
			gc_free();
			__PERFORM_WSACleanup()
				return 0x11; //powod: blad alokacji pamieci (found_mod)
		}
		gc_add((void**)&found_mod);

		//podliczanie punktow 'trafienia'
		for (i = 0; i < n_device; i++) {
			found_mod[i] = 0;
			if (param.address != 0 && param.address == pdevices_info[i].address)
				found_mod[i] |= FOUND_MOD_ADDDRESS;
			if (param.name != NULL && strcmp(param.name, pdevices_info[i].name) == 0)
				found_mod[i] |= FOUND_MOD_NAME;
			if (param.cod != 0 && param.cod == pdevices_info[i].COD)
				found_mod[i] |= FOUND_MOD_COD;

			//szukanie urzadzenia z najwieksza liczba pkt
			if (val_max_found_mod < found_mod[i]) {
				val_max_found_mod = found_mod[i];
				i_max_found_mod = i;
			}
		}

		if (val_max_found_mod == 0) {
			__NSILENT	printf("> not found requested device\n");
			gc_free();
			__PERFORM_WSACleanup()
				return 0x42; //powod: nie znaleziono zadanego urzadzenia
		}

		//ostrzezenie o innym adresie niz podano
		if (param.address != 0 && param.address != pdevices_info[i_max_found_mod].address)
			__NSILENT	printf("> given device has a different address\n");

		//ostrzezenie o innej nazwie niz podano
		if (param.name != NULL && strcmp(param.name, pdevices_info[i_max_found_mod].name) != 0)
			__NSILENT	printf("> given device has a different name\n");

		//ostrzezenie o innym cod niz podano
		if (param.cod != 0 && param.cod != pdevices_info[i_max_found_mod].COD)
			__NSILENT	printf("> given device has a different cod\n");

		param.address = pdevices_info[i_max_found_mod].address;
		ii_device = i_max_found_mod;
	}

	//printf("DEBUG\tAdres bluetooth to w koncu: 0x%12X\n", param.address);

	//i dalej sockety

	//#TODO spr zakresy danych przy spr parametrow


	//*** sockety ***//
	client = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(client == INVALID_SOCKET)
	    printf("TODO error socket\n");
    else{
        printf("TODO socket is ok\n");
        printf("i_device: %d\n", ii_device);
        address.btAddr = param.address;
        memcpy(&address.serviceClassId, &pdevices_info[ii_device].ServiceClassId, sizeof(GUID)); //#TODO sciagnac cod
        printf("GUID: 0x%I32X 0x%04X 0x%04X 0x%02X%02X%02X%02X%02X%02X%02X%02X \n",
                address.serviceClassId.Data1, address.serviceClassId.Data2, address.serviceClassId.Data3,
                address.serviceClassId.Data4[0], address.serviceClassId.Data4[1],
                address.serviceClassId.Data4[2], address.serviceClassId.Data4[3],
                address.serviceClassId.Data4[4], address.serviceClassId.Data4[5],
                address.serviceClassId.Data4[6], address.serviceClassId.Data4[7]);
        printf("GUID: 0x%I32X 0x%04X 0x%04X 0x%02X%02X%02X%02X%02X%02X%02X%02X \n",
                pdevices_info[ii_device].ServiceClassId.Data1, pdevices_info[ii_device].ServiceClassId.Data2, pdevices_info[ii_device].ServiceClassId.Data3,
                pdevices_info[ii_device].ServiceClassId.Data4[0], pdevices_info[ii_device].ServiceClassId.Data4[1],
                pdevices_info[ii_device].ServiceClassId.Data4[2], pdevices_info[ii_device].ServiceClassId.Data4[3],
                pdevices_info[ii_device].ServiceClassId.Data4[4], pdevices_info[ii_device].ServiceClassId.Data4[5],
                pdevices_info[ii_device].ServiceClassId.Data4[6], pdevices_info[ii_device].ServiceClassId.Data4[7]);
        address.addressFamily = AF_BTH;
        address.port = 1 & 0xFF; //#TODO

        if (connect (client, (SOCKADDR *)&address, sizeof(address))==SOCKET_ERROR){
            //Perform error handling.
            printf("TODO error connect: %I32d\n", WSAGetLastError());
            printf((closesocket (client) == SOCKET_ERROR ? "closesocket failed\n" : "closesocket success\n"));
        }else{
            uint32_t ret = 0;
            printf("TODO connect is ok\n");
            ret = send(client, "makarena\0", 9, 0);
            if(ret == SOCKET_ERROR) printf("send SOCKET_ERROR\n");
            else if(ret < 9) printf("nie wszystkie dane zostaly wyslane\n");
            else if(ret != 9) printf("cos poszlo nie tak\n");
            else printf("dane zostaly wyslane, wszystko jest ok\n");
            printf((closesocket (client) == SOCKET_ERROR ? "closesocket failed\n" : "closesocket success\n"));
        }
    }


	//*** zakonczenie programu z powodzeniem ***//
	__VERBOSE
		printf("> all done without error\n");

	gc_free();
	__PERFORM_WSACleanup()
#undef __PERFORM_WSACleanup
		return 0;
}







