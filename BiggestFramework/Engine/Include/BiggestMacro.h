
#pragma once

#define	SAFE_DELETE(p)	if(p)	{delete p; p = NULL;}
#define	SAFE_DELETE_ARRAY(p)	if(p)	{delete [] p; p = NULL;}
#define	SAFE_RELEASE(p)	if(p)	{p->Release(); p = NULL;}

#define	FRIEND_CLASS(Type)	private:friend class Type;

#define	DECLARE_SINGLE(Type)	\
	private:\
		static Type*	m_pInst;\
	public:\
		static Type* GetInst()\
				{\
			if (!m_pInst)\
						{\
				m_pInst = new Type; \
				m_pInst->SetTypeName<Type>();\
						}\
			return m_pInst;\
				}\
		static void DestroyInst()\
				{\
			SAFE_DELETE(m_pInst);\
				}\
	private:\
		Type();\
		~Type();

#define	DEFINITION_SINGLE(Type)	Type*	Type::m_pInst	= NULL;

#define	DECLARE_SINGLE_COUNT(Type, count)	\
	private:\
		static Type*	m_pInst[count];\
	public:\
		static Type* GetInst(int idx = 0)\
				{\
			if (!m_pInst[idx])\
						{\
				m_pInst[idx] = new Type; \
				m_pInst[idx]->SetTypeName<Type>();\
						}\
			return m_pInst[idx]; \
				}\
		static void DestroyInst(int idx = 0)\
				{\
			SAFE_DELETE(m_pInst[idx]); \
				}\
		static void DestroyInstAll()\
				{\
			for (int i = 0; i < count; ++i)\
						{\
				SAFE_DELETE(m_pInst[i]);\
						}\
				}\
	private:\
		Type(); \
		~Type();

#define	DEFINITION_SINGLE_COUNT(Type, count)\
	Type*	Type::m_pInst[count]	= {};

#define	SINGLE(Type)	Type::GetInst()
#define	DESTROY(Type)	Type::DestroyInst()
#define	SINGLE_INDEX(Type, index)	Type::GetInst(index)
#define	DESTROY_INDEX(Type, index)	Type::DestroyInst(index)
#define	DESTROY_ALL(Type)			Type::DestroyInstAll()
#define	DEVICE			SINGLE(CDevice)->GetDevice()
#define DEVICECONTEXT	SINGLE(CDevice)->GetDeviceContext()

#define	SPRITE			SINGLE(CDevice)->GetSprite()

#define	CREATE_INSTANCE(Type)	\
	public:\
		static Type* Create()\
				{\
			Type*	p	= new Type;\
			if(!p->Init())\
						{\
				SAFE_RELEASE(p);\
				return NULL;\
						}\
			p->SetTypeName<Type>();\
			return p;\
				}

#define	CREATE_INSTANCE_TAG(Type)	\
	public:\
		static Type* Create(const string& strTag = "Default")\
				{\
			Type*	p	= new Type;\
			if(!p->Init())\
						{\
				SAFE_RELEASE(p);\
				return NULL;\
						}\
			p->SetTypeName<Type>();\
			p->SetTag(strTag);\
			return p;\
				}

#define	CREATE_INSTANCE_TAG_SCENE(Type)	\
	public:\
		static Type* Create(CScene* pScene, const string& strTag = "Default")\
								{\
			Type*	p	= new Type;\
			if(!p->Init(pScene))\
						{\
				SAFE_RELEASE(p);\
				return NULL;\
						}\
			p->SetTypeName<Type>();\
			p->SetTag(strTag);\
			return p;\
								}

#define	CREATE_CLASS(Type, BaseType)	\
	class JJIENGINE_DLL Type : public BaseType\
		{\
		CREATE_INSTANCE(Type)\
		CREATE_INSTANCE_TAG(Type)\
		public:\
			virtual bool Init();\
		protected:\
			Type();\
		public:\
			~Type();

#define	CREATE_CLASS_SINGLE(Type, BaseType)	\
	class JJIENGINE_DLL Type : public BaseType\
		{\
		DECLARE_SINGLE(Type)\
		public:\
			virtual bool Init();

#define	CREATE_CLASS_SINGLE_COUNT(Type, BaseType, count)	\
	class JJIENGINE_DLL Type : public BaseType\
		{\
		DECLARE_SINGLE_COUNT(Type, count)\
		public:\
			virtual bool Init();

#define	END_CLASS };

#define	CURRENT_OBJPOOL	CSceneManager::GetInst()->GetCurrentObjectPool()
#define	PREV_OBJPOOL	CSceneManager::GetInst()->GetPrevObjectPool()
#define	NEXT_OBJPOOL	CSceneManager::GetInst()->GetNextObjectPool()

#define	CURRENT_RESOURCEMANAGER	CSceneManager::GetInst()->GetCurrentResourceManager()
#define	PREV_RESOURCEMANAGER	CSceneManager::GetInst()->GetPrevResourceManager()
#define	NEXT_RESOURCEMANAGER	CSceneManager::GetInst()->GetNextResourceManager()


#define _VEC2(F)	XMLoadFloat2(F)
#define _VEC3(F)	XMLoadFloat3(F)
#define _VEC4(F)	XMLoadFloat4(F)

#define _MATRIX(M)	XMLoadFloat4x4(M)