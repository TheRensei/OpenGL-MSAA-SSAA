#ifndef SKINNED_MESH_H
#define	SKINNED_MESH_H

#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>   
#include <assimp/Importer.hpp>     
#include <assimp/postprocess.h>

#include "Mesh.h"

class SkinnedMesh
{
public:
    SkinnedMesh();

    ~SkinnedMesh();

    bool LoadMesh(const std::string& Filename);

    void Render(GLuint shader);
	
    GLuint NumBones() const
    {
        return m_NumBones;
    }
    
    void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms, unsigned int animationNumber);
	int getNumberOfAnimations();
    
private:
    #define NUM_BONES_PER_VEREX 4

    struct BoneInfo
    {
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

        BoneInfo()
        {
            BoneOffset = glm::mat4(0.0);
            FinalTransformation = glm::mat4(0.0);
        }
    };
    
    struct VertexBoneData
    {        
        GLuint IDs[NUM_BONES_PER_VEREX];
        float Weights[NUM_BONES_PER_VEREX];

        VertexBoneData()
        {
            Reset();
        };
        
        void Reset()
        {
        	memset(IDs, 0, sizeof(IDs));
        	memset(Weights, 0, sizeof(Weights));
        }
        
        void AddBoneData(GLuint BoneID, float Weight);
    };

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);    
    GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, unsigned int animationNumber);
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(GLuint MeshIndex,
                  const aiMesh* paiMesh,
                  std::vector<glm::vec3>& Positions,
                  std::vector<glm::vec3>& Normals,
                  std::vector<glm::vec2>& TexCoords,
                  std::vector<VertexBoneData>& Bones,
                  std::vector<unsigned int>& Indices);
    void LoadBones(GLuint MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF
  
enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs            
};

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry {
        MeshEntry()
        {
            NumIndices    = 0;
            BaseVertex    = 0;
            BaseIndex     = 0;
            MaterialIndex = INVALID_MATERIAL;
        }
        
        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

	std::vector<MeshEntry> m_Entries;
	std::vector<Texture> m_Textures;

	std::map<std::string, GLuint> m_BoneMapping; // maps a bone name to its index
	GLuint m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;
    glm::mat4 m_GlobalInverseTransform;
    
    const aiScene* m_pScene;
    Assimp::Importer m_Importer;
};


#endif

