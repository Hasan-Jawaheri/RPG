#ifndef _QUESTS_H_
#define _QUESTS_H_

#include "..//Common.h"

class QuestPhase {
	enum {

	};

public:
	QuestPhase ( void );
	~QuestPhase ( void );
};

class Quest {
protected:
	enum QSTATE {
		QAVAILABLE,
		QUNAVAILABLE,
		QINLOG
	} _state;
	UINT _curPhase;
	vector<QuestPhase*> _phases;
	hxObject* _mark;

public:
	Quest ( void );
	~Quest ( void );

	void Load ( UINT questID );
};

class QuestManager {
	static hxMesh *_qmark, *_excl;
	static hxGBufferMaterial* _mat;
	static vector<Quest*> _quests;

public:
	static void Initialize ( void );
	static void Cleanup ( void );
};

#endif //_QUESTS_H_