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

	//winsock
	uint32_t ret_status = 0;
	WSADATA WSAData = { 0 };
	uint16_t winsock_ver = MAKEWORD(2, 2); //the highest to negotiate
	uint32_t n_device = 0;
	uint32_t ii_device = 0xFFFFFFFF;

	//info o urzadzeniach
	PBT_FOUND pdevices_info = NULL;

	//required device
	bool is_device_specified = false;
	struct {
        char * name;
        uint32_t cod;
        uint64_t address;
	}rquird_dev = {NULL, 0, 0};

	//socket
	SOCKET client;
	SOCKADDR_BTH address;
	memset(&address, 0, sizeof(address));
	memset(&address, 0, sizeof(client));

	//filepath do zapisu
	char * filepath = NULL;

	//*** rozpakowanie parametrow ***// //#HOTTTODO
#define __NSILENT                   \
if((!param_is_silent() && !param_is_verbose()) || param_is_verbose())

#define __VERBOSE                   \
if(param_is_verbose())

    if(!(param_add('n', VAL_TYPE_STRING, "name")
    &&param_add('a', VAL_TYPE_UINT64, "address")
    &&param_add('c', VAL_TYPE_UINT32, "COD")
    &&param_add('f', VAL_TYPE_STRING, "filepath")))
       return 0x20; //#TODO doprecyzowac

    if(param_set(argc, argv)){
        if(param_is_help()){ param_show_help(); return 0; }
    }else{
        uint8_t err_code = param_get_err();
        switch(err_code){
        case 0x11:
            __DEBUG("-> param value in set_params allocation memory failed\n");
            param_cout(false, "> error memory access\n");
            __NSILENT show_err(0x20000002);
            param_clear();
            return 0x11; //powod: blad alokacji pamieci wartosci parametru
            break;
        case 0x12:
        case 0x13:
        case 0x23:
            __DEBUG("-> programmer mistake 0x12, 0x13 or 0x23\n");
            param_clear();
            break;
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x24:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x51:
        default:
            param_cout(false, "> callpath argument error\n");
            param_clear();
            return 0x20;
        }
    }

    //#TODO sprawdz czy cod i adres sa specyfikowane
    //#TODO rozwazyc strukture stalych
    {
        void * temp = NULL;

        rquird_dev.name = ((temp = param_get("name")) ? (char*)temp : NULL);
        rquird_dev.cod = ((temp = param_get("COD")) ? *(uint32_t*)temp : 0);
        rquird_dev.address = ((temp = param_get("address")) ? *(uint64_t*)temp : 0);
    }

	if ((rquird_dev.address != 0
      || rquird_dev.name != NULL
      || rquird_dev.cod != 0
      ) && !param_get_err()
     ) is_device_specified = true;


	//wyswietlanie podanych parametrow
	param_cout(true, "> parameters:\n");
	//#HOTTTODO
	param_cout(true, rquird_dev.name == NULL
		? "\tdevice name was not specified\n"
		: "\tdevice name: %s\n", rquird_dev.name
	);
	//#HOTTTODO
	param_cout(true, rquird_dev.address == 0
        ? "\tdevice address was not specified\n"
        : "\tdevice address: 0x%I64X\n", rquird_dev.address
	);
	//#HOTTTODO
	param_cout(true, rquird_dev.cod == 0
        ? "\tdevice cod was not specified\n"
        : "\tdevice cod: 0x%I32X\n", rquird_dev.cod
	);

	//#HOTTTODO
	//brak wymaganych parametrow wywolania w trybie -s
	if (param_is_silent() && !is_device_specified) {
		__DEBUG("-> lack of required parameters in mode <silent>\n");
		gc_free();
		param_clear();
		return 0x21; //powod: niemozliwa kontynuacja w trybie silent ze wzgledu na brak wymaganych parametrow wywolania
	}

	//#HOTTTODO
	//przygotowanie domyslnej nazwy pliku do zapisu
	__DEBUG("-> default filename creation\n");
	if ((filepath = (char *)param_get("filepath")) == NULL) {
		time_t t = time(NULL);
		struct tm date = *localtime(&t);

		//#HOTTTODO
		//alokacja pamieci
		filepath = (char*)calloc(32, sizeof(char));
		//#HOTTTODO
		if (filepath == NULL) {
			__DEBUG("-> filename allocation memory failed\n");
			 param_cout(false, "> error memory access\n");
			__NSILENT	show_err(ret_status);
			gc_free();
			param_clear();
			return 0x11; //powod: blad alokacji pamieci (filename)
		}
		//#HOTTTODO
		gc_add((void**)&(filepath));

		//skladanie nazwy
		//#HOTTTODO
		sprintf(filepath, "data_%02d%02d%02d_%02d%02d%02d.txt", date.tm_mday, date.tm_mon+1 , (date.tm_year+1900)%100, date.tm_hour, date.tm_min, date.tm_sec);
		//#HOTTTODO
		param_cout(true, "> filename: %s\n", filepath);
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

	param_cout(true, "> choosen version Windows Sockets: %d.%d.\n", HIBYTE(winsock_ver), LOBYTE(winsock_ver));
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
		param_clear();
		return 0x31;  //powod: blad WSAStartup
	}

	param_cout(true,
            "> expects version Windows Sockets: %d.%d\n"
			"> the highest version Windows Sockets supported: %d.%d\n",
			HIBYTE(WSAData.wVersion), LOBYTE(WSAData.wVersion),
			HIBYTE(WSAData.wHighVersion), LOBYTE(WSAData.wHighVersion)
    );


	//wyszukiwanie urzadzen
    param_cout(true, "> device discovering -- wait 2 to 15s\n");
	pdevices_info = find_bt_device(&n_device);
	switch ((ret_status = get_finder_err())) {
	case 0:
		param_cout(true, "> finding device completed successsfully\n");
		break;
	case 0x20000005:
	    __NSILENT show_err(ret_status);
	    break;
	case 0x20000006:
		__NSILENT show_err(ret_status);
		__PERFORM_WSACleanup();
		gc_free();
		param_clear();
		clear_bt_found();
		return 0x41;
	default:
		__DEBUG("-> find_bt_device run failed\n");
		 param_cout(false, "> search devices failed\n");
		if(ret_status == WSASERVICE_NOT_FOUND){
		     param_cout(false, "> service found nothing or service has not been found\n");
		     __VERBOSE show_err(ret_status);
            __PERFORM_WSACleanup();
            gc_free();
            param_clear();
            clear_bt_found();
            return 0x43; //powod: brak service
		}
        __NSILENT show_err(ret_status);
        __PERFORM_WSACleanup();
        gc_free();
        param_clear();
        clear_bt_found();
        return 0x40; //powod: blad podczas wyszukiwania urzadzen
	}

	//*** analiza znalezionych urzadzen ***//

	//wylistownie znalezionych urzadzen
	//#HOTTTODO
	if (param_is_verbose() || !is_device_specified) {
		param_cout(false, "\n");
		for (i = 0; i < n_device; i++) {
            param_cout(false,
                "\n"
                "Device found number: %d\n"
                "\tName: %s\n"
                "\tAddress: %04I64X:%02I64X:%06I64Xh\n"
                "\tIs it connected?: %s\n"
                "\tIs it remembered?: %s\n"
                "\tIs it authenticated?: %s\n"
                "\tCOD: %I32d\n",
                i,
                pdevices_info[i].name,
                GET_NAP(pdevices_info[i].address),
                (GET_SAP(pdevices_info[i].address) & 0xFF000000) >> (6 * 4), GET_SAP(pdevices_info[i].address) & 0xFFFFFF,
                ((pdevices_info[i].flags)&BTHNS_RESULT_DEVICE_CONNECTED ? "YES" : "NO"),
                ((pdevices_info[i].flags)&BTHNS_RESULT_DEVICE_REMEMBERED ? "YES" : "NO"),
                ((pdevices_info[i].flags)&BTHNS_RESULT_DEVICE_AUTHENTICATED ? "YES" : "NO"),
                pdevices_info[i].COD
            );
		}
		param_cout(false, "\n");
	}

	//pobranie wybranego numeru urzadzenia
	if (!is_device_specified) {
		char buffer[64];
		uint32_t i_device = 0;

		param_cout(false, ">> type number of device or 'e' to end: ");
		fgets(buffer, sizeof(buffer), stdin);

		if (strlen(buffer) == 1 && buffer[0] == '\n') {
			__DEBUG("-> error due to fgets return empty string\n");
            param_cout(false, "> problem with receiving answer\n");
			gc_free();
			__PERFORM_WSACleanup();
			param_clear();
			clear_bt_found();
			return 0x51; //powod: pusty napis
		}
		buffer[strlen(buffer) - 1] = '\0'; //usuwanie \n na \0
		if (strcmp(buffer, "e") == 0 || strcmp(buffer, "end") == 0) {
			gc_free();
			__PERFORM_WSACleanup();
			param_clear();
			clear_bt_found();
			return 0; //powod: uzytkownik zazadal zakonczenia programu
		}
		if (!is_dec_number(buffer)) {
            param_cout(false, "> answers is not a number\n");
			gc_free();
			__PERFORM_WSACleanup()
			param_clear();
			clear_bt_found();
            return 0x52; //powod: podano nieporawna wartosc -- odp nie jest liczba
		}
		i_device = strtol(buffer, NULL, 10);
		if (i_device > n_device - 1) {
            param_cout(false, "> device with this number is not listed\n");
			gc_free();
			__PERFORM_WSACleanup()
			param_clear();
			clear_bt_found();
            return 0x53; //powod: podano niepoprawna wartosc -- odp nie jest liczba z dozwolonego zakresu
		}
		//#HOTTTODO //...
		rquird_dev.address = pdevices_info[i_device].address;
		ii_device = i_device;
	}
	else { //sprawdzenie czy znaleziono podane urzadzenie
		uint8_t * found_mod = NULL;
		uint8_t val_max_found_mod = 0;
		uint8_t i_max_found_mod = 0xFF;
		found_mod = (uint8_t *)calloc(n_device, sizeof(uint8_t));
		if (found_mod == NULL) {
			__DEBUG("-> found_mod allocation memory failed\n");
			 param_cout(false, "> error memory access\n");
			gc_free();
			__PERFORM_WSACleanup()
			param_clear();
			clear_bt_found();
            return 0x11; //powod: blad alokacji pamieci (found_mod)
		}
		gc_add((void**)&found_mod);

		//podliczanie punktow 'trafienia'
		for (i = 0; i < n_device; i++) {
			found_mod[i] = 0;
			//#HOTTTODO
			if (rquird_dev.address != 0 && rquird_dev.address == pdevices_info[i].address)
				found_mod[i] |= FOUND_MOD_ADDDRESS;
			if (rquird_dev.name != NULL && strcmp(rquird_dev.name, pdevices_info[i].name) == 0)
				found_mod[i] |= FOUND_MOD_NAME;
			if (rquird_dev.cod != 0 && rquird_dev.cod == pdevices_info[i].COD)
				found_mod[i] |= FOUND_MOD_COD;

			//szukanie urzadzenia z najwieksza liczba pkt
			if (val_max_found_mod < found_mod[i]) {
				val_max_found_mod = found_mod[i];
				i_max_found_mod = i;
			}
		}

		if (val_max_found_mod == 0) {
            param_cout(false, "> not found requested device\n");
			gc_free();
			__PERFORM_WSACleanup()
			param_clear();
			clear_bt_found();
			return 0x42; //powod: nie znaleziono zadanego urzadzenia
		}

		//ostrzezenie o innym adresie niz podano
		//#HOTTTODO
		if (rquird_dev.address != 0 && rquird_dev.address != pdevices_info[i_max_found_mod].address)
            param_cout(false, "> given device has a different address\n");

		//ostrzezenie o innej nazwie niz podano
		//#HOTTTODO
		if (rquird_dev.name != NULL && strcmp(rquird_dev.name, pdevices_info[i_max_found_mod].name) != 0)
            param_cout(false, "> given device has a different name\n");

		//ostrzezenie o innym cod niz podano
		//#HOTTTODO
		if (rquird_dev.cod != 0 && rquird_dev.cod != pdevices_info[i_max_found_mod].COD)
            param_cout(false, "> given device has a different cod\n");

            //#HOTTTODO
		rquird_dev.address = pdevices_info[i_max_found_mod].address;
		ii_device = i_max_found_mod;
	}
	//#TODO spr zakresy danych przy spr parametrow

	//*** sockety ***//
	client = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(client == INVALID_SOCKET){
        param_cout(false, "TODO error socket\n");
	}else{
        param_cout(false, "TODO socket is ok\n");
        param_cout(false, "i_device: %d\n", ii_device);
        //#HOTTTODO
        address.btAddr = rquird_dev.address;
        memcpy(&address.serviceClassId, &pdevices_info[ii_device].ServiceClassId, sizeof(GUID)); //#TODO sciagnac cod
        address.addressFamily = AF_BTH;
        address.port = 1 & 0xFF; //#TODO

        if (connect (client, (SOCKADDR *)&address, sizeof(address))==SOCKET_ERROR){
            //Perform error handling.
            param_cout(false, "TODO error connect: %I32d\n", WSAGetLastError());
            if(closesocket (client) == SOCKET_ERROR){ param_cout(false, "closesocket failed\n");}
            else { param_cout(false, "closesocket success\n");}
        }else{
            uint32_t ret = 0;
            param_cout(false, "TODO connect is ok\n");
            ret = send(client, "makarena\0", 9, 0);
            if(ret == SOCKET_ERROR){ param_cout(false, "send SOCKET_ERROR\n");}
            else if(ret < 9){ param_cout(false, "nie wszystkie dane zostaly wyslane\n");}
            else if(ret != 9){ param_cout(false, "cos poszlo nie tak\n");}
            else { param_cout(false, "dane zostaly wyslane, wszystko jest ok\n");}
            if(closesocket (client) == SOCKET_ERROR){ param_cout(false, "closesocket failed\n");}
            else { param_cout(false, "closesocket success\n");}
        }
    }


	//*** zakonczenie programu z powodzeniem ***//
	param_cout(true, "> all done without error\n");

	gc_free();
	__PERFORM_WSACleanup()
#undef __PERFORM_WSACleanup
    param_clear();
    clear_bt_found();
    return 0;
}







