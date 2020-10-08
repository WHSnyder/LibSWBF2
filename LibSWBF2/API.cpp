#include "stdafx.h"
#include "API.h"
#include "InternalHelpers.h"
#include "Types/LibString.h"
#include "Chunks/MSH/MSH.h"
#include "Wrappers/Level.h"
#include "Container.h"
#include "Wrappers/Terrain.h"


namespace LibSWBF2
{
#define CheckPtr(obj, ret) if (obj == nullptr) { LOG_ERROR("[API] Given Pointer was NULL!"); return ret; }


	// Memory //
	void Memory_Blit(void *dest, void *src, int numBytes)
	{
		memcpy(dest, src, numBytes);
	}


	// Logging //
	uint8_t LOG_GetNextLog(const char*& msg, ELogType& level, uint32_t& line, const char*& file)
	{
		static Logging::LoggerEntry current;
		bool hasLogEntry = Logging::Logger::GetNextLog(current);
		msg = current.m_Message.Buffer();
		level = current.m_Level;
		line = current.m_Line;
		file = current.m_File.Buffer();
		return hasLogEntry;
	}

	void LOG_SetLogfileLevel(ELogType LogfileLevel)
	{
		Logging::Logger::SetLogfileLevel(LogfileLevel);
	}


	// MSH //
	Chunks::MSH::MSH* MSH_Create()
	{
		return Chunks::MSH::MSH::Create();
	}

	uint8_t MSH_Delete(Chunks::MSH::MSH* msh)
	{
		CheckPtr(msh, false)
		LibSWBF2::Chunks::MSH::MSH::Destroy(msh);
		return true;
	}

	uint8_t MSH_ReadFromFile(Chunks::MSH::MSH* msh, const char* path)
	{
		CheckPtr(msh, false)
		return msh->ReadFromFile(path);
	}

	uint8_t MSH_WriteToFile(Chunks::MSH::MSH* msh, const char* path)
	{
		CheckPtr(msh, false)
		return msh->WriteToFile(path);
	}

	void STRP_CalcPolygons(Chunks::MSH::STRP* strp)
	{
		CheckPtr(strp,)
		strp->CalcPolygons();
	}

	EModelPurpose MODL_GetPurpose(Chunks::MSH::MODL* modl)
	{
		CheckPtr(modl, EModelPurpose::Miscellaneous)
		return modl->GetPurpose();
	}

	const Container* Container_Initialize()
	{
		return Container::Create();
	}

	uint32_t Container_AddLevel(Container* container, const char *path)
	{
		return (uint32_t) container -> AddLevel(path);
	}

	float_t Container_GetProgress(Container* container, uint32_t handleNum)
	{
		CheckPtr(container, -1.0f)
		return container -> GetLevelProgress((uint16_t) handleNum);
	}  

	const bool Container_IsDone(Container* container)
	{
		CheckPtr(container, false);
		return container -> IsDone();
	}

	const Level* Container_GetLevel(Container* container, uint32_t handleNum)
	{
		CheckPtr(container,nullptr);
		return container -> GetLevel((uint16_t) handleNum);
	}

	const void Container_LoadLevels(Container* container)
	{
		CheckPtr(container,);
		container -> StartLoading();
	}

	const void* Container_GetWrapper(Container* container, uint32_t type, const char *name)
	{
		switch (type)
		{
			case 0:
				return static_cast<const void *>(container -> FindModel(name));
			case 1:
				return static_cast<const void *>(container -> FindTexture(name));
			default:
				return nullptr;
		}
	}


	// Wrappers
	Level* Level_FromFile(const char* path)
	{
		CheckPtr(path, nullptr);
		return Level::FromFile(path);
	}

	void Level_Destroy(Level* level)
	{
		CheckPtr(level,);
		Level::Destroy(level);
	}

	uint8_t Level_IsWorldLevel(const Level* level)
	{
		CheckPtr(level, false);
		return level->IsWorldLevel();
	}

	void Level_GetModels(const Level* level, const Model**& modelArr, uint32_t& modelCount)
	{
		modelCount = 0;
		CheckPtr(level, );
		const List<Model>& models = level->GetModels();

		// since level->GetModels() just returns a reference to the actual list
		// member of level, which will persist even after this call ended, we can safely
		// provide the model addresses of the underlying buffer to the inquirer. 
		// The inquirer of course is not allowed to alter the data!
		static List<const Model*> modelPtrs;
		modelPtrs.Clear();

		for (size_t i = 0; i < models.Size(); ++i)
		{
			modelPtrs.Add(&models[i]);
		}

		modelArr = modelPtrs.GetArrayPtr();
		modelCount = (uint32_t)modelPtrs.Size();
	}

	//TEMPORARY: Basic texture handling until I push the other wrappers...
    const bool Level_GetTextureData(const Level* level, const char *texName, const uint8_t*& imageDataOut, int& width, int& height)
    {
    	static const uint8_t *imageData = nullptr;
    	delete imageData;

    	width = height = 0;
    	CheckPtr(level, false);

    	const Texture *tex = level -> GetTexture(texName);
    	if (tex == nullptr)
    	{
    		return false;
    	}

    	uint16_t w,h;
    	tex -> GetImageData(ETextureFormat::R8_G8_B8_A8, 0, w, h, imageData);
    	height = h;
    	width = w;
    	imageDataOut = imageData;
    	return true;
    }


	void Level_GetTerrains(const Level* level, const Terrain**& terrainArr, uint32_t& terrainCount)
	{
		CheckPtr(level, );
		const List<Terrain>& terrains = level->GetTerrains();

		static List<const Terrain*> terrainPtrs;
		terrainPtrs.Clear();

		for (size_t i = 0; i < terrains.Size(); ++i)
		{
			terrainPtrs.Add(&terrains[i]);
		}

		terrainArr = terrainPtrs.GetArrayPtr();
		terrainCount = (uint32_t)terrainPtrs.Size();
	}


	/*
		Terrain
	*/
	
	//Will eventually return pointers to Texture wrappers...
	const void Terrain_GetTexNames(const Terrain *tern, uint32_t& numTextures, const char**& namesOut)
	{
		static const char** nameStrings = nullptr;
		delete nameStrings; 

		numTextures = 0;
		CheckPtr(tern, );

        const List<String>& texNames = tern -> GetLayerTextures();
        numTextures = (uint32_t) texNames.Size();

    	nameStrings = new const char *[numTextures]; 
    	for (uint32_t i = 0; i < numTextures; i++)
        {
        	nameStrings[i] = texNames[i].Buffer(); 
        }

        namesOut = nameStrings;
	}


    const void Terrain_GetHeightMap(const Terrain *ter, uint32_t& dim, uint32_t& dimScale, float_t*& heightData)
    {
    	dim = 0;
    	CheckPtr(ter, );
    	ter -> GetHeightMap(dim, dimScale, heightData);
    }


	const void Terrain_GetBlendMap(const Terrain *ter, uint32_t& dim, uint32_t& numLayers, uint8_t*& data)
	{	
		dim = 0;
    	CheckPtr(ter, );
		ter -> GetBlendMap(dim, numLayers, data);
	}


	const void Terrain_GetHeightBounds(const Terrain *ter, float& floor, float& ceiling)
	{
    	CheckPtr(ter, );
		ter -> GetHeightBounds(floor, ceiling);
	}



	const char* Model_GetName(const Model* model)
	{
		CheckPtr(model, nullptr);

		// model->GetName() returns a ref to the persistent member,
		// char buffers of String's are always null terminated, so we
		// can just return the buffer pointer.
		const String& name = model->GetName();
		return name.Buffer();
	}

	const void Model_GetSegments(const Model* model, Segment*& segmentArr, uint32_t& segmentCount)
	{
		CheckPtr(model,);
		const List<Segment>& segments = model->GetSegments();
		segmentArr = segments.GetArrayPtr();
		segmentCount = (uint32_t)segments.Size();
	}

	uint8_t Model_IsSkeletalMesh(const Model* model)
	{
		CheckPtr(model, false);
		return model->IsSkeletalMesh();
	}

	uint8_t Model_GetSkeleton(const Model* model, Bone*& boneArr, uint32_t& boneCount)
	{
		CheckPtr(model, false);

		// keep this static, so the buffer is valid after the call ends.
		// this of course results in holding a permanent copy of the last queried
		// bone list in memory, and will overwrite the buffer in the next query...
		// TODO: maybe make the bone list a member of Model?
		static List<Bone> bones;
		if (!model->GetSkeleton(bones))
		{
			return false;
		}
		boneArr = bones.GetArrayPtr();
		boneCount = (uint32_t)bones.Size();
		return true;
	}


	const CollisionMesh* Model_GetCollisionMesh(const Model *model)
	{
		const CollisionMesh& mesh = model -> GetCollisionMesh();
		return &mesh;
	}


	const Model* Level_GetModel(const Level* level, const char* modelName)
	{
		CheckPtr(level, nullptr);
		return level->GetModel(modelName);
	}


	const void Model_GetPrimitivesMasked(const Model* model, uint32_t mask, int& numPrims,
										CollisionPrimitive**& primArrayPtr)
	{
		static List<CollisionPrimitive> primsList;
		static List<CollisionPrimitive *> primPtrs;
		
		numPrims = 0;
		CheckPtr(model,);

		primsList = model -> GetCollisionPrimitives((ECollisionMaskFlags) mask);
		primPtrs.Clear();

		for (size_t i = 0; i < primsList.Size(); i++)
		{
			primPtrs.Add(&primsList[i]);
		}		

		primArrayPtr = primPtrs.GetArrayPtr();
		numPrims = (uint32_t) primPtrs.Size();		
	}


	const void CollisionMesh_GetIndexBuffer(const CollisionMesh *collMesh, uint32_t& count, uint16_t*& outBuffer)
	{
		//static int* tempBuffer = nullptr;
		//delete tempBuffer;

		//uint32_t* meshBuffer;

		collMesh -> GetIndexBuffer(ETopology::TriangleList, count, outBuffer);

		//tempBuffer = new int[count];

		//for (int i = 0; i < (int)count; i++)
		//{
		//	tempBuffer[i] = (int) meshBuffer[i];
		//}

		//outBuffer = tempBuffer;
	}
    

    const void CollisionMesh_GetVertexBuffer(const CollisionMesh *collMesh, uint32_t& count, float_t*& buffer)
    {
    	static float_t *tempBuffer = nullptr;
    	delete tempBuffer;

    	Vector3 *verts;
    	collMesh -> GetVertexBuffer(count, verts);

    	tempBuffer = new float_t[count * 3];

    	for (int i = 0; i < (int)count; i++)
    	{
    		Vector3& curVec = verts[i];

    		tempBuffer[3*i]     = curVec.m_X; 
    		tempBuffer[3*i + 1] = curVec.m_Y; 
    		tempBuffer[3*i + 2] = curVec.m_Z; 
    	}

    	buffer = tempBuffer;
    }


    //CollisionPrimitive

    const void CollisionPrimitive_FetchAllFields(CollisionPrimitive *primPtr,
                                            float_t& f1, float_t& f2, float_t& f3,
                                            const char *& namePtr, const char *& parentNamePtr,
                                            uint32_t& maskFlags, uint32_t& primitiveType,
                                            Vector3*& pos, Vector4*& rot)
    {
    	static String name, parentName;
    	static Vector3 posTemp;
    	static Vector4 rotTemp;

    	f1 = f2 = f3 = 0.0f;

    	name = primPtr -> GetName();
    	parentName = primPtr -> GetParentName();

    	namePtr = name.Buffer();
    	parentNamePtr = parentName.Buffer();

    	maskFlags = (uint32_t) primPtr -> GetMaskFlags();
    	primitiveType = (uint32_t) primPtr -> GetPrimitiveType();

    	rotTemp = primPtr -> GetRotation();
    	posTemp = primPtr -> GetPosition();

    	pos = &posTemp; 
    	rot = &rotTemp; 

    	switch (primitiveType)
    	{
    		case 1:
    			primPtr -> GetSphereRadius(f1);
    			return;
    		case 2:
    			primPtr -> GetCylinderDims(f1,f2);
    			return;
    		case 4:
    			primPtr -> GetCubeDims(f1,f2,f3);
    			return;
    		default:
    			return;
    	}
    }



    const void Vector4_FromPtr(const Vector4* vec, float& x, float& y, float& z, float &w)
    {
    	x = vec -> m_X;
    	y = vec -> m_Y;
    	z = vec -> m_Z;
       	w = vec -> m_W;
    } 

    const void Vector3_FromPtr(const Vector3* vec, float& x, float& y, float& z)
    {
    	x = vec -> m_X;
    	y = vec -> m_Y;
    	z = vec -> m_Z;
    } 

    const void Vector2_FromPtr(const Vector2* vec, float& x, float& y)
    {
    	x = vec -> m_X;
    	y = vec -> m_Y;
    } 





	const char* ENUM_TopologyToString(ETopology topology)
	{
		static Types::String lastToString;
		lastToString = TopologyToString(topology);
		return lastToString.Buffer();
	}

	const char* ENUM_MaterialFlagsToString(EMaterialFlags flags)
	{
		static Types::String lastToString;
		lastToString = MaterialFlagsToString(flags);
		return lastToString.Buffer();
	}

	const char* ENUM_VBUFFlagsToString(EVBUFFlags flags)
	{
		static Types::String lastToString;
		lastToString = VBUFFlagsToString(flags);
		return lastToString.Buffer();
	}
}