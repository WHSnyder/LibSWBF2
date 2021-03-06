#include "stdafx.h"
#include "API.h"
#include "InternalHelpers.h"
#include "Types/LibString.h"
#include "Chunks/MSH/MSH.h"

#include <string.h>

namespace LibSWBF2
{
#define CheckPtr(obj, ret) if (obj == nullptr) { LOG_ERROR("[API] Given Pointer was NULL!"); return ret; }

	// Logging //
	void LOG_SetCallbackMethod(const LogCallback Callback)
	{
		CheckPtr(Callback,)
		Logging::Logger::SetLogCallback(Callback);
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


	const Model* Level_GetModel(const Level* level, const char* modelName)
	{
		CheckPtr(level, nullptr);
		return level->GetModel(modelName);
	}


	void Level_GetModels(const Level* level, const Model**& modelArr, uint32_t& modelCount)
	{
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


    const bool Level_GetTextureData(const Level* level, const char *texName, const uint8_t*& imgData, int& width, int& height)
    {
    	const Texture *tex = level -> GetTexture(texName);
    	if (tex == nullptr)
    	{
    		return false;
    	}

    	uint16_t w,h;
    	tex -> GetImageData(ETextureFormat::R8_G8_B8_A8, 0, w, h, imgData);
    	height = h;
    	width = w;
    	//Will add another null check
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


	void Level_GetWorlds(const Level* level, const World**& worldArr, uint32_t& worldCount)
	{
		CheckPtr(level, );
		const List<World>& worlds = level->GetWorlds();

		// since level->GetModels() just returns a reference to the actual list
		// member of level, which will persist even after this call ended, we can safely
		// provide the model addresses of the underlying buffer to the inquirer.
		// The inquirer of course is not allowed to alter the data!
		static List<const World*> worldPtrs;
		worldPtrs.Clear();

		for (size_t i = 0; i < worlds.Size(); ++i)
		{
			worldPtrs.Add(&worlds[i]);
		}

		worldArr = worldPtrs.GetArrayPtr();
		worldCount = (uint32_t) worldPtrs.Size();
	}


	void Level_GetLights(const Level* level, const Light**& LightArr, uint32_t& LightCount)
	{
		CheckPtr(level, );
		const List<Light>& Lights = level->GetLights();

		// since level->GetModels() just returns a reference to the actual list
		// member of level, which will persist even after this call ended, we can safely
		// provide the model addresses of the underlying buffer to the inquirer.
		// The inquirer of course is not allowed to alter the data!
		static List<const Light*> LightPtrs;
		LightPtrs.Clear();

		for (size_t i = 0; i < Lights.Size(); ++i)
		{
			LightPtrs.Add(&Lights[i]);
		}

		LightArr = LightPtrs.GetArrayPtr();
		LightCount = (uint32_t) LightPtrs.Size();
	}


	const Light* Level_GetLight(const Level* level, const char* lightName)
	{
		CheckPtr(level, nullptr);
		return level->GetLight(lightName);
	}


	bool Level_GetGlobalLighting(const Level* level, Vector3 *& topColor, Vector3 *& bottomColor, 
								const char*& light1Name, const char*& light2Name)
	{
		const auto* config = level -> GetGlobalLighting();
		static Vector3 topCol, bottomCol;
		static String name1, name2;

		if (config != nullptr)
		{	
			topColor    = config -> GetTopColor(topCol) ? &topCol : nullptr;
			bottomColor = config -> GetBottomColor(bottomCol) ? &bottomCol : nullptr;

			light1Name  = config -> GetLight1(name1) ? name1.Buffer() : "";
			light2Name  = config -> GetLight2(name2) ? name2.Buffer() : "";

			return true;
		}

		return false;
	}


	const char* ENUM_TopologyToString(ETopology topology)
	{
		static Types::String lastToString;
		lastToString = TopologyToString(topology);
		return lastToString.Buffer();
	}


	const char* Model_GetName(const Model* model)
	{
		static String nameString;
		CheckPtr(model, "");

		// model->GetName() returns a ref to the persistent member,
		// char buffers of String's are always null terminated, so we
		// can just return the buffer pointer.
		//const String& name = model->GetName();

		nameString = model -> GetName();
		return nameString.Buffer();
	}


	const void Model_GetSegments(const Model* model, const Segment**& segmentArr, uint32_t& segmentCount)
	{	
		CheckPtr(model, );
		const List<Segment>& segments = model->GetSegments();

		static List<const Segment*> segmentPtrs;
		segmentPtrs.Clear();

		for (size_t i = 0; i < segments.Size(); ++i)
		{
			segmentPtrs.Add(&segments[i]);
		}

		segmentArr = segmentPtrs.GetArrayPtr();
		segmentCount = (uint32_t)segmentPtrs.Size();
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


	const void Segment_GetVertexBuffer(const Segment* segment, uint32_t& numVerts, float*& vertBuffer)
	{
		Vector3 *verts;
		segment -> GetVertexBuffer(numVerts, verts);

		vertBuffer = new float[numVerts * 3];

		for (int i = 0; i < (int) numVerts; i++)
		{
			Vector3& curVec = verts[i];
			vertBuffer[i * 3] = curVec.m_X;
			vertBuffer[i * 3 + 1] = curVec.m_Y;
			vertBuffer[i * 3 + 2] = curVec.m_Z;
		}
	}


	const void Segment_GetNormalBuffer(const Segment* segment, uint32_t& numNormals, float*& normalsBuffer)
	{
		Vector3 *normals;
		segment -> GetNormalBuffer(numNormals, normals);

		normalsBuffer = new float[numNormals * 3];

		for (int i = 0; i < (int) numNormals; i++)
		{
			Vector3& curVec = normals[i];
			normalsBuffer[i * 3] = curVec.m_X;
			normalsBuffer[i * 3 + 1] = curVec.m_Y;
			normalsBuffer[i * 3 + 2] = curVec.m_Z;
		}
	}


	const void Segment_GetUVBuffer(const Segment* segment, uint32_t& numUVs, float*& UVBuffer)
	{
		Vector2 *UVs;
		segment -> GetUVBuffer(numUVs, UVs);

		UVBuffer = new float[numUVs * 2];

		for (int i = 0; i < (int) numUVs; i++)
		{
			Vector2& curVec = UVs[i];
			UVBuffer[i * 2] = curVec.m_X;
			UVBuffer[i * 2 + 1] = curVec.m_Y;
		}
	}


	const void Segment_GetIndexBuffer(const Segment* segment, uint32_t& numInds, int*& indexBuffer)
	{
		uint16_t *indicies;
		segment -> GetIndexBuffer(numInds, indicies, ETopology::TriangleList);

		indexBuffer = new int[numInds];

		for (int i = 0; i < (int) numInds; i++)
		{
			indexBuffer[i] = (int) indicies[i];
		}
	}


	const char* Segment_GetMaterial(const Segment* segment)
	{
		//static const char *missing = "TEXTURE_MISSING";
		const Material& segmentMat = segment -> GetMaterial();
		const Texture* segmentTex = segmentMat.GetTexture(0);//?

		if (segmentTex == nullptr)
		{
			String *missing = new String("");
			return missing -> Buffer();
		}

		String *segmentTexName = new String(segmentTex -> GetName());
		return segmentTexName -> Buffer();
	}

	
	const char* Segment_GetMaterialTexName(const Segment* segment)
	{
		//static const char *missing = "TEXTURE_MISSING";
		const Material& segmentMat = segment -> GetMaterial();
		const Texture* segmentTex = segmentMat.GetTexture(0);//?

		if (segmentTex == nullptr)
		{
			String *missing = new String("");
			return missing -> Buffer();
		}

		String *segmentTexName = new String(segmentTex -> GetName());
		return segmentTexName -> Buffer();
	}


	const int32_t Segment_GetTopology(const Segment* segment)
	{
		return (int32_t) segment -> GetTopology();
	}


	//scraped together test
	const void Terrain_GetTexNames(const Terrain *tern, uint32_t& numTexes, char**& nameStrings)
	{
		//CheckPtr(tern, nullptr);
        const List<String>& texNames = tern -> GetLayerTextures();

        int numTextures = texNames.Size();

        if (numTextures > 0)
        {
        	nameStrings = new char *[numTextures];

        	for (int i = 0; i < numTextures; i++)
	        {
	        	const String& temp = texNames[i];
	        	nameStrings[i] = new char[temp.Length() + 1]();
	        	strcpy(nameStrings[i], temp.Buffer());
	        }
        }

        numTexes = (uint32_t) numTextures;
	}


    const void Terrain_GetHeightMap(const Terrain *ter, uint32_t& dim, uint32_t& dimScale, float_t*& heightData)
    {
    	ter -> GetHeightMap(dim, dimScale, heightData);
    }


	const void Terrain_GetBlendMap(const Terrain *ter, uint32_t& dim, uint32_t& numLayers, uint8_t*& data)
	{
		ter -> GetBlendMap(dim, numLayers, data);
	}


	const void Terrain_GetHeightBounds(const Terrain *ter, float& floor, float& ceiling)
	{
		ter -> GetHeightBounds(floor, ceiling);
	}


	const char* ENUM_MaterialFlagsToString(EMaterialFlags flags)
	{
		static Types::String lastToString;
		lastToString = MaterialFlagsToString(flags);
		return lastToString.Buffer();
	}

	const char* ENUM_VBUFFlagsToString(EVBUFFlags flags)
	{
		static String lastToString;
		lastToString = VBUFFlagsToString(flags);
		return lastToString.Buffer();
	}

    const char* World_GetName(const World* world)
    {
		CheckPtr(world, "");
		const String& name = world->GetName();
		//char *buffer = new char[strlen(name.Buffer()) + 1]();
		//strcpy(buffer, name.Buffer());
		return name.Buffer();
    }

    const void World_GetInstances(const World* model, const Instance**& instanceArr, uint32_t& instCount)
    {
    	CheckPtr(model, );
		const List<Instance>& segments = model->GetInstances();

		static List<const Instance*> segmentPtrs;
		segmentPtrs.Clear();

		for (size_t i = 0; i < segments.Size(); ++i)
		{
			segmentPtrs.Add(&segments[i]);
		}

		instanceArr = segmentPtrs.GetArrayPtr();
		instCount = (uint32_t)segmentPtrs.Size();
    }
    

    // Wrappers - Instance
    const char* Instance_GetName(const Instance* instance)
    {
        CheckPtr(instance,"")

    	static String instanceName;
		instanceName = instance->GetName();
		return instanceName.Buffer();
    }

    const Vector4* Instance_GetRotation(const Instance* instance)
    {
    	static Vector4 tempVec;
    	tempVec = instance -> GetRotation();
    	return &tempVec;
    }

    const Vector3* Instance_GetPosition(const Instance* instance)
    {
        static Vector3 tempVec;
    	tempVec = instance -> GetPosition();
    	return &tempVec;
    }

    const char * Instance_GetModelName(const Instance* instance)
    {
    	CheckPtr(instance,"")
    	static const String geometryNameProperty("GeometryName");
    	static String geometryName; 

		const EntityClass *instanceClass = instance -> GetEntityClass();

		if (instanceClass != nullptr && instanceClass -> GetProperty(geometryNameProperty,geometryName))
		{
			return geometryName.Buffer();
		}

		return "";
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


    const char* Light_GetAllFields(const Light* lightPtr, Vector4*& rotPtr,
                                    Vector3*& posPtr, uint32_t& lightType, 
                                    Vector3*& colPtr, float_t& range,
                                    Vector2*& conePtr)
    {
    	CheckPtr(lightPtr, nullptr);

    	static Vector3 lastPos, lastCol;
    	static Vector4 lastRot; 
    	static Vector2 lastCone(0,0);	
    	float_t inner=0,outer=0;

    	lastRot  = lightPtr -> GetRotation();
    	lastPos  = lightPtr -> GetPosition();
    	lastCol  = lightPtr -> GetColor();

    	lightPtr -> GetSpotAngles(inner,outer);
    	lastCone = Vector2(inner,outer);

    	lightType = (uint32_t) lightPtr -> GetType();
    	lightPtr -> GetRange(range);

    	rotPtr  = &lastRot;
    	colPtr  = &lastCol;
    	posPtr  = &lastPos;
    	conePtr = &lastCone;

    	const String& name = lightPtr -> GetName();
    	return name.Buffer();
    }
}
