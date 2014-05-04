// reading a text file
#include "FParser.h"
using namespace std;

FParser::FParser ( void ) {
	_query = nullptr;
	_numLines = 0;
	_longestLine = 1;
}
FParser::~FParser ( void ) {
	if ( _query )
		delete _query;
	if ( _file.is_open ( ) )
		_file.close ( );
}

void FParser::Initialize ( const char* filename, Query* qObj ) {
	_file.open ( filename );
	_query = qObj;
	_numLines = 0;
	string str;
	_linePos.push_back ( 0 );
	while ( std::getline ( _file, str ) ) {
		_numLines++;
		_linePos.push_back ( _file.tellg ( ) );
		_longestLine = max ( _longestLine,
			_linePos[_numLines] - _linePos[_numLines - 1] + 1 );
	}
	_file.clear ( );
	_file.seekg ( 0 );
}
unsigned int FParser::GetNumLines ( void ) const {
	return _numLines;
}
Query* FParser::ParseLine ( unsigned int index ) {
	if ( index >= _numLines || !_file.is_open ( ) )
		return nullptr;

	char* lineStr = new char[_longestLine];
	_file.clear ( );
	_file.seekg ( _linePos[index] );
	_file.getline ( lineStr, _longestLine, '\n' );

	unsigned int prevPos = -1, pos = 0, len = strlen ( lineStr ), field = 0;
	bool bValidField = false;
	Query* q = _query->Clone ( );
	while ( pos <= len ) {
		if ( lineStr[pos] == ',' || lineStr[pos] == '\n' || lineStr[pos] == '\0' ) {
			if ( bValidField ) {
				FIELD type = q->GetFieldType ( field );
				if ( type == FIELD_STRING ) {
					char* tmp = new char[pos - prevPos];
					memcpy ( tmp, &lineStr[prevPos + 1], pos - prevPos );
					tmp[pos - prevPos - 1] = '\0';
					q->AddField ( field, tmp, pos - prevPos );
					delete[] tmp;
				} else if ( type == FIELD_INT ) {
					int num = atoi ( &lineStr[prevPos + 1] );
					q->AddField ( field, (void*)num, sizeof ( int ) );
				} else {
					float num = atof ( &lineStr[prevPos + 1] );
					int inum;
					memcpy ( &inum, &num, 4 );
					q->AddField ( field, (void*)inum, 4 );
				}
			}
			prevPos = pos;
			field++;
			bValidField = false;
		}
		if ( lineStr[pos] != ' ' && lineStr[pos] != '\t' && lineStr[pos] != ',' )
			bValidField = true;

		pos++;
		if ( lineStr[pos] == ' ' && pos - 1 == prevPos )
			prevPos++;
	}
	delete[] lineStr;
	return q;
}