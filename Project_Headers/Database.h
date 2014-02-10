#ifndef DATABASE_H_
#define DATABASE_H_

typedef enum {		/*   ID   Description        */
	DB_GUGUS,		/*!<  0   Testvariable       */
	DB_TEST,
	DB_NOF_VARS		/*!< Sentinel, must be last! */
} DB_VarID;

typedef enum {
	U8, 
	U16
} DB_DataType;

typedef struct DB_Var {
	void* var_ptr;
	DB_DataType type;
	bool ee;
} DB_Var;

void DB_Init(void);
void DB_RegisterVar(uint8_t varID, void* adr, DB_DataType type, bool eeprom);
DB_DataType DB_GetType(uint8_t varID);
void* DB_GetVar(uint8_t varID);
uint8_t DB_GetVar_u8(uint8_t varID);

#endif
