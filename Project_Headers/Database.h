#ifndef DATABASE_H_
#define DATABASE_H_

typedef enum {		/*   ID   Description        */
	DB_MOT_ROTARY,
	DB_MOT_KNEE,
	DB_MOT_LIFT,
	DB_DBGBUFFER,
	DB_NOF_VARS		/*!< Sentinel, must be last! */
} DB_VarID;

typedef enum {
	U8, 
	U16, 
	MOT,
	T_DBGBUFFER
} DB_DataType;

typedef struct DB_Var {
	void* var_ptr;
	DB_DataType type;
	bool nvm;
} DB_Var;

#define DB_NVM_BASE_ADDR		0x1FC00

void DB_Init(void);
void DB_RegisterVar(uint8_t varID, void* adr, DB_DataType type, bool eeprom);
DB_DataType DB_GetType(uint8_t varID);
void* DB_GetVar(uint8_t varID);
uint8_t DB_GetVar_u8(uint8_t varID);
void DB_LoadNVM(void);
void DB_SaveNVM(void); 

#endif
