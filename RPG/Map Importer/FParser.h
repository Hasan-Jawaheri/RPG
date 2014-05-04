#ifndef _FPARSER_H_
#define _FPARSER_H_


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifndef max
#define max(a,b) (a>b?a:b)
#endif

enum FIELD {
	FIELD_INT,
	FIELD_STRING,
	FIELD_FLOAT
};
class Query {
public:
	virtual ~Query ( void ) {}

	//pure virtual members
	virtual Query* Clone ( void ) = 0;
	virtual void AddField ( unsigned int index,
		void* data,
		unsigned int len ) = 0;
	virtual FIELD GetFieldType ( unsigned int index ) const = 0;
	virtual void* GetField ( unsigned int index ) const = 0;
};

class FParser {
	std::ifstream _file;
	unsigned int _numLines, _longestLine;
	Query* _query;
	std::vector<unsigned int> _linePos;

public:
	FParser ( void );
	~FParser ( void );

	void Initialize ( const char* filename, Query* qObj );
	unsigned int GetNumLines ( void ) const;
	Query* ParseLine ( unsigned int index );
};

#endif //_FPARSER_H_