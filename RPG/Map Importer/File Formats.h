#pragma once

#include "FParser.h"

using namespace std;

class HeaderQuery : public Query {
	vector<char*> fields;

public:

	HeaderQuery ( unsigned int numFields ) {
		for ( unsigned int i = 0; i < numFields; i++ )
			fields.push_back ( nullptr );
	}
	~HeaderQuery ( void ) {
		for ( unsigned int i = 0; i < fields.size ( ); i++ )
			if ( fields[i] )
				delete[] fields[i];
	}

	Query* Clone ( void ) {
		return new HeaderQuery ( fields.size ( ) );
	}

	virtual void AddField ( unsigned int index,
									void* data,
									unsigned int len ) {
		if ( index >= fields.size ( ) )
			return;

		if ( fields[index] )
			delete[] fields[index];
		fields[index] = new char[len];
		memcpy ( fields[index], data, len );
	}

	virtual FIELD GetFieldType ( unsigned int index ) const {
		return FIELD_STRING;
	}

	virtual void* GetField ( unsigned int index ) const {
		if ( index >= fields.size ( ) )
			return nullptr;

		return fields[index];
	}
};

#define NUM_UNIT_FIELDS 20

class UnitsQuery : public Query {
	FIELD fields[NUM_UNIT_FIELDS];
	void* fieldsVals[NUM_UNIT_FIELDS];

public:
	UnitsQuery ( void ) {
		FIELD f[NUM_UNIT_FIELDS] = {
			FIELD_INT, FIELD_STRING, FIELD_INT,FIELD_STRING, FIELD_STRING,
			FIELD_STRING, FIELD_FLOAT, FIELD_INT, FIELD_INT,
			FIELD_STRING, FIELD_STRING, FIELD_STRING, FIELD_STRING, FIELD_STRING,
			FIELD_STRING, FIELD_STRING, FIELD_STRING, FIELD_STRING, FIELD_STRING,
			FIELD_STRING
		};
		memcpy ( fields, f, NUM_UNIT_FIELDS * sizeof FIELD );
		for ( unsigned int i = 0; i < NUM_UNIT_FIELDS; i++ )
			fieldsVals[i] = nullptr;
	}
	~UnitsQuery ( void ) {
		for ( unsigned int i = 0; i < NUM_UNIT_FIELDS; i++ )
			if ( fields[i] == FIELD_STRING && fieldsVals[i] )
				delete[] fieldsVals[i];
	}

	Query* Clone ( void ) {
		return new UnitsQuery ( );
	}

	virtual void AddField ( unsigned int index,
									void* data,
									unsigned int len ) {
		if ( index >= NUM_UNIT_FIELDS )
			return;

		if ( fields[index] == FIELD_STRING && fieldsVals[index] )
			delete[] fieldsVals[index];

		if ( fields[index] == FIELD_STRING ) {
			fieldsVals[index] = new char[len];
			memcpy ( fieldsVals[index], data, len );
		} else
			fieldsVals[index] = data;
	}

	virtual FIELD GetFieldType ( unsigned int index ) const {
		if ( index >= NUM_UNIT_FIELDS )
			return FIELD_INT;

		return fields[index];
	}

	virtual void* GetField ( unsigned int index ) const {
		if ( index >= NUM_UNIT_FIELDS )
			return nullptr;

		return fieldsVals[index];
	}
};


#define NUM_EQUIPMENT_FIELDS 10

class EquipmentQuery : public Query {
	FIELD fields[NUM_EQUIPMENT_FIELDS];
	void* fieldsVals[NUM_EQUIPMENT_FIELDS];

public:
	EquipmentQuery ( void ) {
		FIELD f[NUM_EQUIPMENT_FIELDS] = {
			FIELD_INT, FIELD_STRING, FIELD_STRING, FIELD_STRING,
			FIELD_STRING, FIELD_STRING,
			FIELD_INT, FIELD_INT, FIELD_STRING, FIELD_INT
		};
		memcpy ( fields, f, NUM_EQUIPMENT_FIELDS * sizeof FIELD );
		for ( unsigned int i = 0; i < NUM_EQUIPMENT_FIELDS; i++ )
			fieldsVals[i] = nullptr;
	}
	~EquipmentQuery ( void ) {
		for ( unsigned int i = 0; i < NUM_EQUIPMENT_FIELDS; i++ )
			if ( fields[i] == FIELD_STRING && fieldsVals[i] )
				delete[] fieldsVals[i];
	}

	Query* Clone ( void ) {
		return new EquipmentQuery ( );
	}

	virtual void AddField ( unsigned int index,
									void* data,
									unsigned int len ) {
		if ( index >= NUM_EQUIPMENT_FIELDS )
			return;

		if ( fields[index] == FIELD_STRING && fieldsVals[index] )
			delete[] fieldsVals[index];

		if ( fields[index] == FIELD_STRING ) {
			fieldsVals[index] = new char[len];
			memcpy ( fieldsVals[index], data, len );
		} else
			fieldsVals[index] = data;
	}

	virtual FIELD GetFieldType ( unsigned int index ) const {
		if ( index >= NUM_EQUIPMENT_FIELDS )
			return FIELD_INT;

		return fields[index];
	}

	virtual void* GetField ( unsigned int index ) const {
		if ( index >= NUM_EQUIPMENT_FIELDS )
			return nullptr;

		return fieldsVals[index];
	}
};


#define NUM_QUEST_FIELDS 4

class QuestQuery : public Query {
	FIELD fields[NUM_QUEST_FIELDS];
	void* fieldsVals[NUM_QUEST_FIELDS];

public:
	QuestQuery ( void ) {
		FIELD f[NUM_QUEST_FIELDS] = {
			FIELD_INT, FIELD_STRING, FIELD_STRING, FIELD_INT
		};
		memcpy ( fields, f, NUM_QUEST_FIELDS * sizeof FIELD );
		for ( unsigned int i = 0; i < NUM_QUEST_FIELDS; i++ )
			fieldsVals[i] = nullptr;
	}
	~QuestQuery ( void ) {
		for ( unsigned int i = 0; i < NUM_QUEST_FIELDS; i++ )
			if ( fields[i] == FIELD_STRING && fieldsVals[i] )
				delete[] fieldsVals[i];
	}

	Query* Clone ( void ) {
		return new QuestQuery ( );
	}

	virtual void AddField ( unsigned int index,
									void* data,
									unsigned int len ) {
		if ( index >= NUM_QUEST_FIELDS )
			return;

		if ( fields[index] == FIELD_STRING && fieldsVals[index] )
			delete[] fieldsVals[index];

		if ( fields[index] == FIELD_STRING ) {
			fieldsVals[index] = new char[len];
			memcpy ( fieldsVals[index], data, len );
		} else
			fieldsVals[index] = data;
	}

	virtual FIELD GetFieldType ( unsigned int index ) const {
		if ( index >= NUM_QUEST_FIELDS )
			return FIELD_INT;

		return fields[index];
	}

	virtual void* GetField ( unsigned int index ) const {
		if ( index >= NUM_QUEST_FIELDS )
			return nullptr;

		return fieldsVals[index];
	}
};