#include "Quests.h"


hxMesh* QuestManager::_qmark = nullptr;
hxMesh* QuestManager::_excl = nullptr;
hxGBufferMaterial* QuestManager::_mat = nullptr;
vector<Quest*> QuestManager::_quests;

void QuestManager::Initialize ( void ) {
	_qmark = new hxMesh ( 0, APPHANDLE->core );
	_qmark->LoadFromHXM ( "Media\\qmark.HXM" );
	_excl = new hxMesh ( 0, APPHANDLE->core );
	_excl->LoadFromHXM ( "Media\\exclamation.HXM" );
	_mat = new hxGBufferMaterial ( 0, APPHANDLE->core );
}
void QuestManager::Cleanup ( void ) {
	for ( UINT i = 0; i < _quests.size ( ); i++ )
		delete _quests[i];

	HX_SAFE_REMOVEREF ( _qmark );
	HX_SAFE_REMOVEREF ( _excl );
	HX_SAFE_REMOVEREF ( _mat );
}

Quest::Quest ( void ) {

}
Quest::~Quest ( void ) {

}