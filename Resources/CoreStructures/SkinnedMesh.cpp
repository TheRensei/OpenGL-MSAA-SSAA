#include "SkinnedMesh.h"
#include "TextureLoader.h"
#include <iostream>

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

static glm::mat4 AssimpToGLM(const aiMatrix4x4 &matrix) 
{
	//the documentation says Assimp is row major while GLM and OpenGL is colomn major,
	//but access to elements are the same, so the following is correct.
	return glm::mat4(
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
	);
}

static glm::mat3 AssimpToGLM(const aiMatrix3x3 &matrix) 
{
	//the documentation says Assimp is row major while GLM and OpenGL is colomn major,
	//but access to elements are the same, so the following is correct.
	return glm::mat3(
		matrix[0][0], matrix[1][0], matrix[2][0],
		matrix[0][1], matrix[1][1], matrix[2][1],
		matrix[0][2], matrix[1][2], matrix[2][2]
	);
}

void SkinnedMesh::VertexBoneData::AddBoneData(GLuint BoneID, float Weight)
{
	const GLuint arraySize = sizeof(IDs) / sizeof(GLuint);

    for (GLuint i = 0 ; i < arraySize; i++) 
	{
        if (Weights[i] == 0.0) 
		{
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }        
    }
    
    // should never get here - more bones than we have space for
    //assert(0);
}

SkinnedMesh::SkinnedMesh()
{
    m_VAO = 0;
	memset(m_Buffers, 0, sizeof(m_Buffers));
    m_NumBones = 0;
    m_pScene = NULL;
}


SkinnedMesh::~SkinnedMesh()
{
    Clear();
}


void SkinnedMesh::Clear()
{
    if (m_Buffers[0] != 0) {
		const GLuint arraySize = sizeof(m_Buffers) / sizeof(GLuint);
        glDeleteBuffers(arraySize, m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}


bool SkinnedMesh::LoadMesh(const string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
 
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(sizeof(m_Buffers) / sizeof(GLuint), m_Buffers);

    bool Ret = false;    
  
    m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
    
    if (m_pScene) 
	{  
        m_GlobalInverseTransform = AssimpToGLM(m_pScene->mRootNode->mTransformation);
        m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
        Ret = InitFromScene(m_pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	

    return Ret;
}


bool SkinnedMesh::InitFromScene(const aiScene* pScene, const string& Filename)
{  
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.reserve(pScene->mNumMaterials);

    vector<glm::vec3> Positions;
    vector<glm::vec3> Normals;
    vector<glm::vec2> TexCoords;
    vector<VertexBoneData> Bones;
    vector<GLuint> Indices;
       
    GLuint NumVertices = 0;
    GLuint NumIndices = 0;
    
    // Count the number of vertices and indices
    for (GLuint i = 0 ; i < m_Entries.size() ; i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;        
        m_Entries[i].NumIndices    = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex    = NumVertices;
        m_Entries[i].BaseIndex     = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i].NumIndices;
    }
    
    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
        
    // Initialize the meshes in the scene one by one
    for (GLuint i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
	   
   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);    
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    return glGetError() == 0 ? true:false;
}


void SkinnedMesh::InitMesh(GLuint MeshIndex,
                    const aiMesh* paiMesh,
                    vector<glm::vec3>& Positions,
                    vector<glm::vec3>& Normals,
                    vector<glm::vec2>& TexCoords,
                    vector<VertexBoneData>& Bones,
                    vector<GLuint>& Indices)
{    
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    
    // Populate the vertex attribute vectors
    for (GLuint i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Positions.emplace_back(pPos->x, pPos->y, pPos->z);
        Normals.emplace_back(pNormal->x, pNormal->y, pNormal->z);
        TexCoords.emplace_back(pTexCoord->x, pTexCoord->y);
    }
    
    LoadBones(MeshIndex, paiMesh, Bones);
    
    // Populate the index buffer
    for (GLuint i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}


void SkinnedMesh::LoadBones(GLuint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
    for (GLuint i = 0 ; i < pMesh->mNumBones ; i++) 
	{                
        GLuint BoneIndex = 0;        
        string BoneName(pMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) 
		{
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;            
	        BoneInfo bi;			
			m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = AssimpToGLM(pMesh->mBones[i]->mOffsetMatrix);            
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else 
		{
            BoneIndex = m_BoneMapping[BoneName];
        }                      
        
        for (GLuint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) 
		{
            GLuint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;                   
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }    
}


bool SkinnedMesh::InitMaterials(const aiScene* pScene, const string& Filename)
{
    // Extract the directory part from the file name
    string::size_type SlashIndex = Filename.find_last_of("/\\");
    string Dir;

    if (SlashIndex == string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/\\";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (GLuint i = 0 ; i < pScene->mNumMaterials ; i++) 
	{
        const aiMaterial* pMaterial = pScene->mMaterials[i];

		aiString str;
		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &str, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
		{
			Texture texture;
			string fullPath = Dir;
			fullPath.append("\\" + string(str.C_Str()));
			texture.id = TextureLoader::loadTexture(fullPath);
			texture.type = "texture_diffuse";
			texture.path = str.C_Str();
			m_Textures.push_back(texture);
		}
    }
    return Ret;
}


void SkinnedMesh::Render(GLuint shader)
{
    glBindVertexArray(m_VAO);
	unsigned int diffuseNr = 1;

    for (GLuint i = 0 ; i < m_Entries.size() ; i++) 
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = m_Textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);

												 // now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);


		glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 
                                 (void*)(sizeof(GLuint) * m_Entries[i].BaseIndex), 
                                 m_Entries[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}


GLuint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{    
    for (GLuint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < static_cast<float>(pNodeAnim->mPositionKeys[i + 1].mTime)) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


GLuint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (GLuint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < static_cast<float>(pNodeAnim->mRotationKeys[i + 1].mTime)) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


GLuint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (GLuint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < static_cast<float>(pNodeAnim->mScalingKeys[i + 1].mTime)) 
		{
            return i;
        }
    }
    
    assert(0);

    return 0;
}


void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
            
    GLuint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    GLuint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = static_cast<float>(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - static_cast<float>(pNodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    GLuint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    GLuint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = static_cast<float>(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - static_cast<float>(pNodeAnim->mRotationKeys[RotationIndex].mTime)) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    GLuint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    GLuint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = static_cast<float>(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - static_cast<float>(pNodeAnim->mScalingKeys[ScalingIndex].mTime)) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, unsigned int animationNumber)
{    
    string NodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = m_pScene->mAnimations[animationNumber];
        
	glm::mat4 NodeTransformation(AssimpToGLM(pNode->mTransformation));
     
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    
    if (pNodeAnim) 
	{
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM;
        ScalingM = glm::scale(glm::mat4(1.0),glm::vec3(Scaling.x, Scaling.y, Scaling.z));
        
        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
    	
		glm::mat4 RotationM = glm::mat4(AssimpToGLM(RotationQ.GetMatrix()));

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM(1.0);
        TranslationM = glm::translate(glm::mat4(1.0), glm::vec3(Translation.x, Translation.y, Translation.z));
        
        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }
       
	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        GLuint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (GLuint i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, animationNumber);
    }
}


void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms, unsigned int animationNumber)
{
	glm::mat4 Identity(1.0);
    
    float TicksPerSecond = static_cast<float>(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, static_cast<float>(m_pScene->mAnimations[0]->mDuration));


    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity, animationNumber);

	printf("Animation time: %.3f Ticks Per Second: %.3f Time In Ticks: %.3f\n", AnimationTime, TicksPerSecond, TimeInTicks);

    Transforms.resize(m_NumBones);

    for (GLuint i = 0 ; i < m_NumBones ; i++) 
	{
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

int SkinnedMesh::getNumberOfAnimations()
{
	return m_pScene->mNumAnimations;
}


const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    for (GLuint i = 0 ; i < pAnimation->mNumChannels ; i++) 
	{
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (strcmp(NodeName.c_str(), pNodeAnim->mNodeName.data) == 0)
		{
            return pNodeAnim;
        }
    }
    
    return nullptr;
}
