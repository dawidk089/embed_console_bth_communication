#include "param.h"
#include "CuTest.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** funkcje narzedziowe **/
int rand_from_arr(int arr[], int len){
    srand( time( NULL ) );
    return arr[rand()%len];
}

VAL_TYPE rand_val_type(){
    int arr[] = {VAL_TYPE_BOOL, VAL_TYPE_STRING, VAL_TYPE_UINT16, VAL_TYPE_UINT32, VAL_TYPE_UINT64, VAL_TYPE_UINT8};
    int len = sizeof(arr)/sizeof(int);
    return (VAL_TYPE)rand_from_arr(arr, len);
}

char rand_corr__dash_switch(){
    int arr[] = {'a','b','c','d','e','f','g',    'i','j','k','l','m','n','o','p','q','r',    't','u',    'w','x','y','z',
                 'A','B','C','D','E','F','G',    'I','J','K','L','M','N','O','P','Q','R',    'T','U',    'W','X','Y','Z'};
    int len = sizeof(arr)/sizeof(int);
    return (char)rand_from_arr(arr, len);
}

char tcases_name[] = "name"; //test name [16-chars]

#define RAND_RANGE(beg, end) (rand()%((end)-(beg)+1)+(beg))
#define RAND_PRNT_CH() (RAND_RANGE(0x20, 0x7E))

/** testy zakresu argumentu -- przypadki testowe **/
typedef struct{
        char dsw;
        VAL_TYPE vtyp;
        char name[16];
        bool is_neg;
        uint8_t err_code;
}TCASES__PARAM_ADD;

void test__param_add(CuTest *tc, char * what_test, TCASES__PARAM_ADD * tcases, int n_tcases){
    int i;
    for(i=0; i<n_tcases; ++i){
        bool cond;
        uint8_t err_code;
        CuString str_res;
        CuString str_err;

        param_clear();

        cond = param_add(tcases[i].dsw, tcases[i].vtyp, tcases[i].name);
        printf("Test <param_add(\'%c\', VAL_TYPE(%d), \"%s\");>\n",
               tcases[i].dsw, tcases[i].vtyp, tcases[i].name);
        err_code = param_get_err();

        CuStringInit(&str_res);
        CuStringAppendFormat(&str_res,
            "\n"
            "Test of %s param in param_add function %s but should be %s.\n"
            "return: <%s>\n"
            "Error code: 0x%X\n"
            "argm[0]: <%c>\n"
            "argm[1]: <%d>\n"
            "argm[2]: <%s>\n",
            what_test, (cond?"PASS":"FAIL"), (tcases[i].is_neg?"FAILED":"PASSED"), (cond?"true":"false"), err_code, tcases[i].dsw, tcases[i].vtyp, tcases[i].name);

        CuAssert(tc, str_res.buffer, (tcases[i].is_neg? !cond : cond));


        CuStringInit(&str_err);
        CuStringAppendFormat(&str_err,
            "\n"
            "Test of %s in param_add function fail.\n"
            "Error code: <0x%X> opposite to expected 0x%X\n"
            "argm[0]: <%c>\n"
            "argm[1]: <%d>\n"
            "argm[2]: <%s>\n",
            what_test, err_code, tcases[i].err_code, tcases[i].dsw, tcases[i].vtyp, tcases[i].name);

        CuAssert(tc, str_err.buffer, err_code == tcases[i].err_code);
    }
    printf("\n");
}

void test_range__dash_switch__sign(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        {0x20}, {0x21}, {0x22}, {0x25}, {0x2A}, {0x2F}, //przedzial 2F:20
        {0x3A}, {0x3B}, {0x3F}, {0x40},                 //przedzial 40:3A
        {0x5B}, {0x5C}, {0x5F}, {0x60},                 //przedzial 60:5B
        {0x7B}, {0x7C}, {0x7E}//, {0x7F}                //przedzial 7F:7B
    }; //0x7F is deleted ASCII sign
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i;
    for(i=0; i<n; ++i){
        tcases[i].vtyp = rand_val_type();
        strcpy(tcases[i].name,tcases_name);
        tcases[i].is_neg = true;
        tcases[i].err_code = 0x22;
    }

    test__param_add(tc, "ASCII signs as dash_switch argument", tcases, n);
}

void test_range__dash_switch__digit(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        {'0'}, {'1'}, {'4'}, {'8'}, {'9'}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i;
    for(i=0; i<n; ++i){
        tcases[i].vtyp = rand_val_type();
        strcpy(tcases[i].name,tcases_name);
        tcases[i].is_neg = true;
        tcases[i].err_code = 0x22;
    }
    test__param_add(tc, "ASCII digits as dash_switch argument", tcases, n);
}

void test_range__dash_switch__lowercase(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        {'a'}, {'b'}, {'i'}, {'p'}, {'y'}, {'z'}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i;
    for(i=0; i<n; ++i){
        tcases[i].vtyp = rand_val_type();
        strcpy(tcases[i].name,tcases_name);
        tcases[i].is_neg = false;
        tcases[i].err_code = 0x10;
    }
    test__param_add(tc, "ASCII lowercase as dash_switch argument", tcases, n);
}

void test_range__dash_switch__uppercase(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        {'A'}, {'B'}, {'I'}, {'P'}, {'Y'}, {'Z'}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i;
    for(i=0; i<n; ++i){
        tcases[i].vtyp = rand_val_type();
        strcpy(tcases[i].name,tcases_name);
        tcases[i].is_neg = false;
        tcases[i].err_code = 0x10;
    }
    test__param_add(tc, "ASCII uppercase as dash_switch argument", tcases, n);
}

void test_range__dash_switch__mbed(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        {'s'}, {'v'}, {'h'}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i;
    for(i=0; i<n; ++i){
        tcases[i].vtyp = rand_val_type();
        strcpy(tcases[i].name,tcases_name);
        tcases[i].is_neg = true;
        tcases[i].err_code = 0x23;
    }
    test__param_add(tc, "ASCII embedded switch as dash_switch argument", tcases, n);
}


CuSuite* suitetest_range__dash_switch(){
    CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_range__dash_switch__sign);
	SUITE_ADD_TEST(suite, test_range__dash_switch__digit);
	SUITE_ADD_TEST(suite, test_range__dash_switch__lowercase);
	SUITE_ADD_TEST(suite, test_range__dash_switch__uppercase);
	SUITE_ADD_TEST(suite, test_range__dash_switch__mbed);
	return suite;
}


void test_range__name_pass(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        { rand_corr__dash_switch(), rand_val_type(), "n", false, 0x10},
        { rand_corr__dash_switch(), rand_val_type(), "to", false, 0x10},
        { rand_corr__dash_switch(), rand_val_type(), "pos", false, 0x10},
        { rand_corr__dash_switch(), rand_val_type(), "break", false, 0x10},
        { rand_corr__dash_switch(), rand_val_type(), "paczkomaty", false, 0x10},
        { rand_corr__dash_switch(), rand_val_type(), "embedded system", false, 0x10}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    test__param_add(tc, "valid name as name argument", tcases, n);
}

void test_range__name_empty(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        { rand_corr__dash_switch(), rand_val_type(), "", true, 0x10}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    test__param_add(tc, "empty string name as name argument", tcases, n);
}

void test_range__name_nend(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        { rand_corr__dash_switch(), rand_val_type(), "n", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "to", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "pos", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "break", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "paczkomaty", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "embedded system", true, 0x13}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i, j;
    for(i=0; i<n; ++i){
        for(j=0; j<16; ++j) if(tcases[i].name[j] == '\0') break;
        for(; j<16; ++j) tcases[i].name[j] = RAND_PRNT_CH();
    }
    test__param_add(tc, "not terminated string name as name argument", tcases, n);
}

void test_range__name_digit_sign(CuTest *tc){
    TCASES__PARAM_ADD tcases[] = {
        { rand_corr__dash_switch(), rand_val_type(), "5pos", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "break5", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "embed 5 systems", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "embed \' systems", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "\'embed systems", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "5embed#systems", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "#embed2systems", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "#embed3systems#", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "2embed#systems2", true, 0x13},
        { rand_corr__dash_switch(), rand_val_type(), "embed systems\'", true, 0x13}
    };
    unsigned n = sizeof(tcases)/sizeof(tcases[0]);
    unsigned i, j;
    for(i=0; i<n; ++i){
        for(j=0; j<16; ++j) if(tcases[i].name[j] == '\0') break;
        for(; j<16; ++j) tcases[i].name[j] = RAND_PRNT_CH();
    }
    test__param_add(tc, "with digit and sign name as name argument", tcases, n);
}

CuSuite* suitetest_range__name(){
    CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_range__name_pass);
	SUITE_ADD_TEST(suite, test_range__name_empty);
	SUITE_ADD_TEST(suite, test_range__name_nend);
	SUITE_ADD_TEST(suite, test_range__name_digit_sign);
	return suite;
}

CuSuite* CuGetSuite__param_add(void){
	CuSuite* suite = CuSuiteNew();
	//SUITE_ADD_TEST(suite, test_range__dash_switch);
	CuSuiteAddSuite(suite, suitetest_range__dash_switch());
	CuSuiteAddSuite(suite, suitetest_range__name());
	//SUITE_ADD_TEST(suite, test_range__name);

	return suite;
}
