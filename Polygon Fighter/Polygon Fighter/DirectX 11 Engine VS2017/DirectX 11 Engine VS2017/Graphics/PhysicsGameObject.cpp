//=============================================================================
//
// レンダオブジェクト処理 [RenderableGameObject.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "PhysicsGameObject.h"

//=============================================================================
// 初期化
//=============================================================================
bool PhyGameObject::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, PhysXBasic* phyworld)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	this->phyworld = phyworld;

	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetScale(1.0f, 1.0f, 1.0f);//位置

	auto gScene = phyworld->gScene;
	PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);

		PxShape* shapes[128];
		for (PxU32 i = 0; i < nbActors; i++)
		{
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= 128);
			actors[i]->getShapes(shapes, nbShapes);
			const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

			for (PxU32 j = 0; j < nbShapes; j++)
			{
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				const PxGeometryHolder h = shapes[j]->getGeometry();
				
				auto mat = GetMatrixFromPxMatrix(shapePose);
				//renderGeometry
				meshes.push_back(ProcessDebugMesh(h.any()));
				meshes.at(meshes.size() - 1).transformMatrix = mat;

			}
		}
	}

	this->UpdateMatrix();

	return true;
}

//=============================================================================
//描画
//=============================================================================
void PhyGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	if (!b_use) return;

	//physx debug mesh draw by driectx engine
	for (int i = 0; i < meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		this->cb_vs_vertexshader->data.wvpMatrix = m_GlobalInverseTransform * meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix;//
		this->cb_vs_vertexshader->data.worldMatrix = m_GlobalInverseTransform * meshes[i].GetTransformMatrix() * worldMatrix;//

		this->cb_vs_vertexshader->ApplyChanges();
		meshes[i].Draw();
	}
}

//=============================================================================
// マトリクス更新処理
//=============================================================================
void PhyGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z)
		* XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z)
		* XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

//=============================================================================
// 更新処理
//=============================================================================
void PhyGameObject::Update()
{
	//update 
	
	unsigned int index = 0;
	PxU32 nbActors = this->phyworld->gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		this->phyworld->gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);

		PxShape* shapes[128];
		for (PxU32 i = 0; i < nbActors; i++)
		{
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= 128);
			actors[i]->getShapes(shapes, nbShapes);
			const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

			for (PxU32 j = 0; j < nbShapes; j++)
			{
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				const PxGeometryHolder h = shapes[j]->getGeometry();

				auto mat = GetMatrixFromPxMatrix(shapePose);
				meshes.at(index).transformMatrix = mat;
				if (index == 0)
				{
					XMFLOAT3 scl = XMFLOAT3(10, 0.1, 10);
					auto matscl = XMMatrixScaling(scl.x,scl.y,scl.z);
					meshes.at(index).transformMatrix =  mat * matscl;
				}
				index++;
			}
		}
	}
}

//=============================================================================
// メッシュ情報
//=============================================================================
std::vector<Mesh> PhyGameObject::GetMesh() {
	return meshes;
}

//=============================================================================
// ワールドマトリックスを取る
//=============================================================================
DirectX::XMMATRIX PhyGameObject::GetWorldMatirx() {
	return  this->worldMatrix;
}

//=============================================================================
// 調整マトリックスを設置
//=============================================================================
void PhyGameObject::SetGlobalMatirx(DirectX::XMMATRIX worldmat)
{
	this->m_GlobalInverseTransform = worldmat;
}

//=============================================================================
//デイバッグメッシュを読み込む
//=============================================================================
//debug block is a simple suqare mesh which represent a bone node position
Mesh PhyGameObject::ProcessDebugMesh(const PxGeometry& geom)//, 
{
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	switch (geom.getType())
	{
	case PxGeometryType::eBOX:
	{
		const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
		Mesh mesh = Mesh::DefaultGeometry(Mesh::PrimitiveGeomtry::box, this->device, this->deviceContext,
			"Date\\Textures\\fade_black.png", "PhysicGameobject");
		auto scale = XMMatrixScaling(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);
		mesh.transformMatrix = scale * mesh.transformMatrix;
		return mesh;
	}
	break;

	case PxGeometryType::eSPHERE:
	{
		const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
		//glutSolidSphere(GLdouble(sphereGeom.radius), 10, 10);
	}
	break;

	case PxGeometryType::eCAPSULE:
	{
		const PxCapsuleGeometry& capsuleGeom = static_cast<const PxCapsuleGeometry&>(geom);
		const PxF32 radius = capsuleGeom.radius;
		const PxF32 halfHeight = capsuleGeom.halfHeight;

		//Sphere
		/*glPushMatrix();
		glTranslatef(halfHeight, 0.0f, 0.0f);
		glScalef(radius, radius, radius);
		glutSolidSphere(1, 10, 10);
		glPopMatrix();
*/
		//Sphere
		/*glPushMatrix();
		glTranslatef(-halfHeight, 0.0f, 0.0f);
		glScalef(radius, radius, radius);
		glutSolidSphere(1, 10, 10);
		glPopMatrix();*/

		//Cylinder
		/*glPushMatrix();
		glTranslatef(-halfHeight, 0.0f, 0.0f);
		glScalef(2.0f*halfHeight, radius, radius);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gCylinderData);
		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gCylinderData + 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 13 * 2);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glPopMatrix();*/
	}
	break;

	case PxGeometryType::eCONVEXMESH:
	{
		const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);

		//Compute triangles for each polygon.
		const PxVec3& scale = convexGeom.scale.scale; 
		PxConvexMesh* mesh = convexGeom.convexMesh;
		const PxU32 nbPolys = mesh->getNbPolygons();
		const PxU8* polygons = mesh->getIndexBuffer();
		const PxVec3* verts = mesh->getVertices();
		PxU32 nbVerts = mesh->getNbVertices();
		PX_UNUSED(nbVerts);

		PxU32 numTotalTriangles = 0;

		//バーテックス情報処理
		//Get vertices
		for (UINT i = 0; i < nbVerts; i++)
		{
			Vertex3D vertex;

			vertex.pos.x = verts[i].x;
			vertex.pos.y = verts[i].y;
			vertex.pos.z = verts[i].z;

			vertex.texCoord = XMFLOAT2(1, 1);

			//ここのboneindexにはshaderに入力する骨の情報です。
			//もし下のbone_weights値が-1じゃないならない情報が有効になります。
			//４つbone_weightsの値を掛け算で合わせてメッシュの最後変化値を導き出す(HLSLshaderファイルに行います。)
			vertex.bone_index.w = -1;
			vertex.bone_index.x = -1;
			vertex.bone_index.y = -1;
			vertex.bone_index.z = -1;

			//we judge bone transform by bone weight if bone weight is -1 we dont calculate in shader transform
			vertex.bone_weights.w = -1;
			vertex.bone_weights.x = -1;
			vertex.bone_weights.y = -1;
			vertex.bone_weights.z = -1;

			vertices.push_back(vertex);
		}

		//インデックス情報
		//Get indices
		//for (UINT i = 0; i < nbPolys; i++)
		//{
		//	indices.push_back(polygons[i]);
		//	/*PxHullPolygon data;
		//	mesh->getPolygonData(i, data);

		//	for (UINT j = 0; j < data.mNbVerts - 2; j++)
		//	{
		//		int index = data.mIndexBase + j;
		//		indices.push_back(index);
		//	}*/
		//}
		for (PxU32 i = 0; i < nbPolys; i++)
		{
			PxHullPolygon data;
			mesh->getPolygonData(i, data);

			const PxU32 nbTris = PxU32(data.mNbVerts - 2);
			const PxU8 vref0 = polygons[data.mIndexBase + 0];
			PX_ASSERT(vref0 < nbVerts);
			for (PxU32 j = 0; j < nbTris; j++)
			{
				const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
				const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

				//generate face normal:
				PxVec3 e0 = verts[vref1] - verts[vref0];
				PxVec3 e1 = verts[vref2] - verts[vref0];

				PX_ASSERT(vref1 < nbVerts);
				PX_ASSERT(vref2 < nbVerts);

				PxVec3 fnormal = e0.cross(e1);
				fnormal.normalize();

				indices.push_back(vref0);
				indices.push_back(vref1);
				indices.push_back(vref2);

				vertices.at(vref0).normal = XMFLOAT3(fnormal.x, fnormal.y, fnormal.z);
				vertices.at(vref1).normal = XMFLOAT3(fnormal.x, fnormal.y, fnormal.z);
				vertices.at(vref2).normal = XMFLOAT3(fnormal.x, fnormal.y, fnormal.z);

			}
		}

		std::vector<Texture> textures;
		Texture diskTexture(this->device, "Date\\Textures\\fade_black.png", aiTextureType::aiTextureType_NONE);
		textures.push_back(diskTexture);

		return Mesh(this->device, this->deviceContext, vertices, indices, textures, DirectX::XMMatrixIdentity(), "Debugmesh");
	}
	break;

	case PxGeometryType::eTRIANGLEMESH:
	{
		const PxTriangleMeshGeometry& triGeom = static_cast<const PxTriangleMeshGeometry&>(geom);

		const PxTriangleMesh& mesh = *triGeom.triangleMesh;
		const PxVec3 scale = triGeom.scale.scale;

		const PxU32 triangleCount = mesh.getNbTriangles();
		const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
		const void* indexBuffer = mesh.getTriangles();

		const PxVec3* vertexBuffer = mesh.getVertices();

		const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
		const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);
		PxU32 numTotalTriangles = 0;
		for (PxU32 i = 0; i < triangleCount; ++i)
		{
			PxVec3 triVert[3];

			if (has16BitIndices)
			{
				triVert[0] = vertexBuffer[*shortIndices++];
				triVert[1] = vertexBuffer[*shortIndices++];
				triVert[2] = vertexBuffer[*shortIndices++];
			}
			else
			{
				triVert[0] = vertexBuffer[*intIndices++];
				triVert[1] = vertexBuffer[*intIndices++];
				triVert[2] = vertexBuffer[*intIndices++];
			}

			PxVec3 fnormal = (triVert[1] - triVert[0]).cross(triVert[2] - triVert[0]);
			fnormal.normalize();

			/*if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
			{
				gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 1] = triVert[0];
				gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 3] = triVert[1];
				gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 5] = triVert[2];
				numTotalTriangles++;
			}*/
		}
		/*glPushMatrix();
		glScalef(scale.x, scale.y, scale.z);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
		glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glPopMatrix();*/
	}
	break;

	case PxGeometryType::eINVALID:
	case PxGeometryType::eHEIGHTFIELD:
	case PxGeometryType::eGEOMETRY_COUNT:
	case PxGeometryType::ePLANE:
		return Mesh::DefaultGeometry(Mesh::PrimitiveGeomtry::box, this->device, this->deviceContext,
			"Date\\Textures\\fade_black.png","PhysicGameobject");
		break;
	}
}

//=============================================================================
//マテリアル情報処理
//=============================================================================
std::vector<Texture> PhyGameObject::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(this->device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		case aiTextureType_SPECULAR:
			materialTextures.push_back(Texture(this->device, Color(255, 255, 255), textureType));
			return materialTextures;

		case aiTextureType_HEIGHT:
			materialTextures.push_back(Texture(this->device, Color(128, 128, 255), textureType));
			return materialTextures;

		case aiTextureType_DISPLACEMENT:
			materialTextures.push_back(Texture(this->device, Color(0, 0, 0), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(this->device,
					reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
					pScene->mTextures[index]->mWidth,
					textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(this->device,
					reinterpret_cast<uint8_t*>(pTexture->pcData),
					pTexture->mWidth,
					textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(this->device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}

	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;

}

//=============================================================================
//テクスチャ情報処理
//=============================================================================
TextureStorageType PhyGameObject::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture existsZ
}

//=============================================================================
//テクスチャインデックス情報
//=============================================================================
int PhyGameObject::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

//=============================================================================
//copy function
//=============================================================================
void PhyGameObject::DeepCopy(const PhyGameObject& go)
{

	this->device = go.device;
	this->deviceContext = go.deviceContext;
	this->cb_vs_vertexshader = go.cb_vs_vertexshader;
	this->path = go.path;

	this->SetPosition(go.pos);
	this->SetRotation(go.pos);
	this->SetScale(go.scale);

	this->worldMatrix = go.worldMatrix;
}

void PhyGameObject::RefreshVertexPosition(const PxGeometry& geom, unsigned int index)
{
	// Data to fill
	std::vector<Vertex3D> vertices = meshes.at(index).vertices;
	std::vector<DWORD> indices;

	switch (geom.getType())
	{
	case PxGeometryType::eBOX:
	{
		const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
		/*glScalef(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);
		glutSolidCube(2);*/
	}
	break;

	case PxGeometryType::eSPHERE:
	{
		const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
		//glutSolidSphere(GLdouble(sphereGeom.radius), 10, 10);
	}
	break;

	case PxGeometryType::eCAPSULE:
	{
		const PxCapsuleGeometry& capsuleGeom = static_cast<const PxCapsuleGeometry&>(geom);
		const PxF32 radius = capsuleGeom.radius;
		const PxF32 halfHeight = capsuleGeom.halfHeight;
		
	}
	break;

	case PxGeometryType::eCONVEXMESH:
	{
		const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);

		//Compute triangles for each polygon.
		const PxVec3& scale = convexGeom.scale.scale;
		PxConvexMesh* mesh = convexGeom.convexMesh;
		const PxU32 nbPolys = mesh->getNbPolygons();
		const PxU8* polygons = mesh->getIndexBuffer();
		const PxVec3* verts = mesh->getVertices();
		PxU32 nbVerts = mesh->getNbVertices();
		PX_UNUSED(nbVerts);

		PxU32 numTotalTriangles = 0;

		//バーテックス情報処理
		//Get vertices
		for (UINT i = 0; i < nbVerts; i++)
		{
			Vertex3D vertex;

			vertex.pos.x = verts[i].x;
			vertex.pos.y = verts[i].y;
			vertex.pos.z = verts[i].z;

			vertex.texCoord = XMFLOAT2(1, 1);

			//ここのboneindexにはshaderに入力する骨の情報です。
			//もし下のbone_weights値が-1じゃないならない情報が有効になります。
			//４つbone_weightsの値を掛け算で合わせてメッシュの最後変化値を導き出す(HLSLshaderファイルに行います。)
			vertex.bone_index.w = -1;
			vertex.bone_index.x = -1;
			vertex.bone_index.y = -1;
			vertex.bone_index.z = -1;

			//we judge bone transform by bone weight if bone weight is -1 we dont calculate in shader transform
			vertex.bone_weights.w = -1;
			vertex.bone_weights.x = -1;
			vertex.bone_weights.y = -1;
			vertex.bone_weights.z = -1;

			vertices.push_back(vertex);
		}

		meshes.at(index).RefreshVerTexBuffer(this->deviceContext, vertices, vertices.size());
		//return Mesh(this->device, this->deviceContext, vertices, indices, textures, DirectX::XMMatrixIdentity(), "Debugmesh");
	}
	break;

	case PxGeometryType::eTRIANGLEMESH:
	{
		const PxTriangleMeshGeometry& triGeom = static_cast<const PxTriangleMeshGeometry&>(geom);

		const PxTriangleMesh& mesh = *triGeom.triangleMesh;
		const PxVec3 scale = triGeom.scale.scale;

		const PxU32 triangleCount = mesh.getNbTriangles();
		const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
		const void* indexBuffer = mesh.getTriangles();

		const PxVec3* vertexBuffer = mesh.getVertices();

		const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
		const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);
		PxU32 numTotalTriangles = 0;
		for (PxU32 i = 0; i < triangleCount; ++i)
		{
			PxVec3 triVert[3];

			if (has16BitIndices)
			{
				triVert[0] = vertexBuffer[*shortIndices++];
				triVert[1] = vertexBuffer[*shortIndices++];
				triVert[2] = vertexBuffer[*shortIndices++];
			}
			else
			{
				triVert[0] = vertexBuffer[*intIndices++];
				triVert[1] = vertexBuffer[*intIndices++];
				triVert[2] = vertexBuffer[*intIndices++];
			}

			PxVec3 fnormal = (triVert[1] - triVert[0]).cross(triVert[2] - triVert[0]);
			fnormal.normalize();

			/*if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
			{
				gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 1] = triVert[0];
				gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 3] = triVert[1];
				gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 5] = triVert[2];
				numTotalTriangles++;
			}*/
		}
		/*glPushMatrix();
		glScalef(scale.x, scale.y, scale.z);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
		glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glPopMatrix();*/
	}
	break;

	case PxGeometryType::eINVALID:
	case PxGeometryType::eHEIGHTFIELD:
	case PxGeometryType::eGEOMETRY_COUNT:
	case PxGeometryType::ePLANE:
		/*return Mesh::DefaultGeometry(Mesh::PrimitiveGeomtry::box, this->device, this->deviceContext,
			"Date\\Textures\\fade_black.png", "PhysicGameobject");*/
		break;
	}
}
DirectX::XMMATRIX PhyGameObject::GetMatrixFromPxMatrix(PxMat44 mat) {
	auto rtn = XMMatrixSet(	mat.column0.x, mat.column0.y, mat.column0.z, mat.column0.w,
							mat.column1.x, mat.column1.y, mat.column1.z, mat.column1.w,
							mat.column2.x, mat.column2.y, mat.column2.z, mat.column2.w,
							mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w);
	return rtn;
}
