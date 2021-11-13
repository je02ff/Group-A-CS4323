#include "Roden.h"
#include "readDatabaseIntoArray.h"

int writeBackProductStruct(struct csvProductInfo table[]);
int writeBackBillingStruct(struct csvBillingInfo table[]);
int writeBackClientStruct(char* db, struct csvClientInfo table[]);
int writeBackCustomerOrderStruct(struct csvCustomerOrderInfo table[]);
