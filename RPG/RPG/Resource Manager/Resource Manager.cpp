#include "Resource Manager.h"
#include "..//Entities//Unit.h"

template<typename T>
struct Entry {
	T* resource;
	UINT index;
	UINT refCount;

	void destroy ( void ) {
		HX_SAFE_REMOVEREF ( resource );
	}
};

template<typename T>
class HashTable {
	struct LL_ENTRY {
		LL_ENTRY* next;
		Entry<T> data;
	};

	vector<LL_ENTRY*> hashTable;
	UINT tableSize;

	/*//djb2 hash function by dan bernstein
	unsigned long hash ( LPCSTR str ) {
		unsigned long hash = 5381;
		int c;

		while ( c = *str++ )
			hash = ( ( hash << 5 ) + hash ) + c; / * hash * 33 + c * /

		return hash % HASHTABLE_SIZE;
	}*/
	unsigned long _hash ( UINT index ) {
		return index % HASHTABLE_SIZE;
	}

	void _regulateTableSize ( void ) {
		//if there are exceeding resources, keep removing until we are 1 less
		//than the maximum, or there is nothing to remove
		vector<Entry<T>> sweeped;
		UINT index = 0;
		LL_ENTRY* e = nullptr;
		while ( tableSize >= MAX_NUM_RESOURCES && index < HASHTABLE_SIZE ) {
			e = hashTable[index];
			if ( e ) {
				if ( e->data.refCount == 0 ) {
					sweeped.push_back ( e->data );
					LL_ENTRY* n = e->next;
					delete e;
					hashTable[index] = n;
					tableSize--;
				}
				while ( e->next ) {
					if ( e->next->data.refCount == 0 ) {
						sweeped.push_back ( e->next->data );
						LL_ENTRY* n = e->next->next;
						delete e->next;
						e->next = n;
						tableSize--;
					}
					e = e->next;
				}
			}
			index++;
		}

		for ( UINT i = 0; i < sweeped.size ( ); i++ ) {
			sweeped[i].destroy ( );
		}
	}

public:
	HashTable ( void ) {
		tableSize = 0;
		hashTable.resize ( HASHTABLE_SIZE );
	}

	void cleanup ( void ) {
		for ( UINT i = 0; i < HASHTABLE_SIZE; i++ ) {
			LL_ENTRY* e = hashTable[i];
			while ( e ) {
				LL_ENTRY* next = e->next;
				e->data.destroy ( );
				delete e;
				e = next;
			}
		}
	}

	void addEntry ( Entry<T> entry ) {
		_regulateTableSize ( );

		unsigned long hash = _hash ( entry.index );
		LL_ENTRY* e = new LL_ENTRY;
		e->data = entry;
		e->next = hashTable[hash];
		hashTable[hash] = e;
		tableSize++;
	}
	Entry<T>* removeEntry ( Entry<T> entry ) {
		unsigned long hash = _hash ( entry.index );
		Entry<T>* ret = nullptr;

		LL_ENTRY* e = hashTable[hash];
		if ( e ) {
			if ( e->data.resource == entry.resource ) {
				hashTable[hash] = e->next;
				ret = &e->data;
				delete e;
				tableSize--;
			} else {
				while ( e->next ) {
					if ( e->next->data.resource == entry.resource ) {
						LL_ENTRY* n = e->next->next;
						ret = &e->data;
						delete e->next;
						e->next = n;
						tableSize--;
						break;
					}
					e = e->next;
				}
			}
		}

		return ret;
	}
	Entry<T>* lookupEntry ( UINT index ) {
		unsigned long hash = _hash ( index );

		LL_ENTRY* e = hashTable[hash];
		char tmp[256];
		while ( e ) {
			if ( e->data.index == index )
				return &e->data;
			e = e->next;
		}

		return nullptr;
	}
};

hxCore* ResourceManager::core = nullptr;
hxFile* ResourceManager::dMapFile = nullptr;
FILE* ResourceManager::modelsFile = nullptr;
UINT ResourceManager::curObjID = INT_MAX;
UINT ResourceManager::curUnitAnimID = INT_MAX;

HashTable<hxMesh> _meshes_;
HashTable<hxImage> _images_;
HashTable<hxGBufferMaterial> _materials_;
HashTable<hxEffect> _effects_;
HashTable<hxRigidBody> _rigidBodies_;
HashTable<hxSkeleton> _skeletons_;

//
// Initialization and clean
//
void ResourceManager::Init ( hxCore* c ) {
	core = c;

	dMapFile = new hxFile ( 0, core );
	dMapFile->Initialize ( "media\\Data\\assets", nullptr, true );

	fopen_s ( &modelsFile, "media\\Data\\models", "rb" );

	/*// default texture
	hxImage* defImg;
	(defImg = AllocImage ( "default", true ))->
		Load ( "Media\\Units\\checker.bmp" );

	// default mesh
	AllocMesh ( "default", true )->
		CreateCube ( 10.0f );

	// default animation
	AllocSkeleton ( "default", true )->
		LoadFromHXS ( "Media\\Units\\Character.HXS" );

	//default material
	AllocGBufferMaterial ( "default", true )->
		Texture ( defImg );*/
}
void ResourceManager::Cleanup ( void ) {
	_meshes_.cleanup ( );
	if ( dMapFile ) {
		dMapFile->Close ( );
		HX_SAFE_REMOVEREF ( dMapFile );
	}
	if ( modelsFile ) {
		fclose ( modelsFile );
		modelsFile = nullptr;
	}
}

hxMesh* ResourceManager::_allocMesh ( UINT index ) {
	Entry<hxMesh>* me = _meshes_.lookupEntry ( index );
	if ( !me ) {
		//no yet loaded, load it
		dMapFile->LoadMeshByIndex ( index );

		Entry<hxMesh> entry;
		entry.refCount = 1;
		entry.index = index;
		entry.resource = reinterpret_cast<hxMesh*> ( dMapFile->GetLastLoaded ( ) );

		_meshes_.addEntry ( entry );

		return entry.resource;
	} else
		me->refCount++;

	return me->resource;
}

hxImage* ResourceManager::_allocImage ( UINT index ) {
	Entry<hxImage>* me = _images_.lookupEntry ( index );
	if ( !me ) {
		//no yet loaded, load it
		dMapFile->LoadImageByIndex ( index );

		Entry<hxImage> entry;
		entry.refCount = 1;
		entry.index = index;
		entry.resource = reinterpret_cast<hxImage*> ( dMapFile->GetLastLoaded ( ) );

		_images_.addEntry ( entry );

		return entry.resource;
	} else
		me->refCount++;

	return me->resource;
}

hxEffect* ResourceManager::_allocEffect ( UINT index ) {
	Entry<hxEffect>* me = _effects_.lookupEntry ( index );
	if ( !me ) {
		//no yet loaded, load it
		dMapFile->LoadEffectByIndex ( index );

		Entry<hxEffect> entry;
		entry.refCount = 1;
		entry.index = index;
		entry.resource = reinterpret_cast<hxEffect*> ( dMapFile->GetLastLoaded ( ) );

		_effects_.addEntry ( entry );

		return entry.resource;
	} else
		me->refCount++;

	return me->resource;
}

hxGBufferMaterial* ResourceManager::_allocMaterial ( UINT index ) {
	Entry<hxGBufferMaterial>* me = _materials_.lookupEntry ( index );
	if ( !me ) {
		//no yet loaded, load it
		dMapFile->LoadMaterialByIndex ( index );

		Entry<hxGBufferMaterial> entry;
		entry.refCount = 1;
		entry.index = index;
		entry.resource = reinterpret_cast<hxGBufferMaterial*> ( dMapFile->GetLastLoaded ( ) );

		_materials_.addEntry ( entry );

		return entry.resource;
	} else
		me->refCount++;

	return me->resource;
}

hxRigidBody* ResourceManager::_allocRigidBody ( UINT index ) {
	Entry<hxRigidBody>* me = _rigidBodies_.lookupEntry ( index );
	if ( !me ) {
		//no yet loaded, load it
		dMapFile->LoadRigidBodyByIndex ( index );

		Entry<hxRigidBody> entry;
		entry.refCount = 1;
		entry.index = index;
		entry.resource = reinterpret_cast<hxRigidBody*> ( dMapFile->GetLastLoaded ( ) );

		_rigidBodies_.addEntry ( entry );

		return entry.resource;
	} else
		me->refCount++;

	return me->resource;
}

hxSkeleton* ResourceManager::_allocSkeleton ( UINT index ) {
	Entry<hxSkeleton>* me = _skeletons_.lookupEntry ( index );
	if ( !me ) {
		//no yet loaded, load it
		dMapFile->LoadSkeletonByIndex ( index );

		Entry<hxSkeleton> entry;
		entry.refCount = 1;
		entry.index = index;
		entry.resource = reinterpret_cast<hxSkeleton*> ( dMapFile->GetLastLoaded ( ) );

		_skeletons_.addEntry ( entry );
	} else {
		if ( me->refCount == 0 )
			me->refCount = 1;
	}

	me = _skeletons_.lookupEntry ( index );
	me->refCount++;
	hxSkeleton* sk = new hxSkeleton ( 0, core );
	sk->UseAnimationFrames ( me->resource );

	if ( sk->GetBone ( 0, "backbone" ) ) {
		sk->AddSubAnimation ( );
		sk->SetSubAnimationBaseBone ( 1, sk->GetBone ( 0, "backbone" )->GetIndex ( ), 0 );
	}

	return sk;
}

void ResourceManager::_releaseMesh ( UINT index ) {
	Entry<hxMesh>* e = _meshes_.lookupEntry ( index );
	if ( e )
		if ( e->refCount > 0 )
			e->refCount--;
}

void ResourceManager::_releaseImage ( UINT index ) {
	Entry<hxImage>* e = _images_.lookupEntry ( index );
	if ( e )
		if ( e->refCount > 0 )
			e->refCount--;
}

void ResourceManager::_releaseEffect ( UINT index ) {
	Entry<hxEffect>* e = _effects_.lookupEntry ( index );
	if ( e )
		if ( e->refCount > 0 )
			e->refCount--;
}

void ResourceManager::_releaseMaterial ( UINT index ) {
	Entry<hxGBufferMaterial>* e = _materials_.lookupEntry ( index );
	if ( e )
		if ( e->refCount > 0 )
			e->refCount--;
}

void ResourceManager::_releaseRigidBody ( UINT index ) {
	Entry<hxRigidBody>* e = _rigidBodies_.lookupEntry ( index );
	if ( e )
		if ( e->refCount > 0 )
			e->refCount--;
}

void ResourceManager::_releaseSkeleton ( hxSkeleton* s, UINT index ) {
	Entry<hxSkeleton>* e = _skeletons_.lookupEntry ( index );
	if ( e ) {
		if ( e->refCount > 0 )
			e->refCount--;
		if ( e->refCount == 1 )
			e->refCount = 0;
	}
	HX_SAFE_REMOVEREF ( s );
}

void ResourceManager::GetModelName ( UINT modelID, LPSTR name, UINT ccMax ) {
	fseek ( modelsFile, 4 + modelID * sizeof MODEL, SEEK_SET );
	MODEL model;
	fread ( &model, sizeof MODEL, 1, modelsFile );
	strcpy_s ( name, ccMax, model.name );
}
ALLOCATED_MODEL ResourceManager::AllocModel ( UINT modelID ) {
	hxObject* obj = new hxObject ( curObjID++, core );
	fseek ( modelsFile, 4 + modelID * sizeof MODEL, SEEK_SET );
	MODEL model;
	fread ( &model, sizeof MODEL, 1, modelsFile );
	hxMesh* mesh = nullptr;
	hxImage* tex = nullptr;
	hxEffect* fx = nullptr;
	hxGBufferMaterial* mat = nullptr;
	hxRigidBody* rb = nullptr;
	if ( model.meshes[0] != -1 )
		mesh = _allocMesh ( model.meshes[0] );
	if ( model.textures[0] != -1 )
		tex = _allocImage ( model.textures[0] );
	if ( model.effect != -1 )
		fx = _allocEffect ( model.effect );
	if ( model.material != -1 )
		mat = _allocMaterial ( model.material );
	else
		mat = new hxGBufferMaterial ( 0, core );
	if ( model.rigidBody != -1 )
		rb = _allocRigidBody ( model.rigidBody );

	if ( fx )
		mat->SetEffect ( fx );
	mat->Texture ( tex );
	obj->SetMesh ( mesh );
	obj->SetGBufferMaterial ( mat );
	if ( rb )
		rb->BindObject ( obj );
	obj->SetName ( model.name );

	ALLOCATED_MODEL ret;
	ret.obj = obj;
	ret.rb = rb;
	return ret;
}
void ResourceManager::ReleaseModel ( hxObject* m, UINT modelID ) {
	if ( m ) {
		fseek ( modelsFile, 4 + modelID * sizeof MODEL, SEEK_SET );
		MODEL model;
		fread ( &model, sizeof MODEL, 1, modelsFile );
		if ( model.material != 0 && model.material != -1 )
			_releaseMaterial ( model.material );
		else {
			hxGBufferMaterial* mat = m->GetGBufferMaterial ( );
			HX_SAFE_REMOVEREF ( mat );
		}
		if ( model.meshes[0] != 0 && model.meshes[0] != -1 )
			_releaseMesh ( model.meshes[0] );
		if ( model.textures[0] != 0 && model.textures[0] != -1 )
			_releaseImage ( model.textures[0] );
		if ( model.effect != 0 && model.effect != -1 )
			_releaseEffect ( model.effect );
		if ( model.rigidBody != 0 && model.rigidBody != -1 )
			_releaseRigidBody ( model.rigidBody );

		HX_SAFE_REMOVEREF ( m );
	}
}
ALLOCATED_UNIT ResourceManager::AllocUnit ( UINT modelID, UINT skeletonID ) {
	ALLOCATED_UNIT u;
	ZeroMemory ( &u, sizeof ALLOCATED_UNIT );

	u.obj = AllocModel ( modelID ).obj;
	u.ID = u.obj->GetID ( );

	u.skeleton = _allocSkeleton ( skeletonID );
	u.obj->AttachAnimation ( u.skeleton );

	return u;
}
void ResourceManager::ReleaseUnit ( ALLOCATED_UNIT unit, UINT modelID, UINT skeletonID ) {
	_releaseSkeleton ( unit.skeleton, skeletonID );
	ReleaseModel ( unit.obj, modelID );
}