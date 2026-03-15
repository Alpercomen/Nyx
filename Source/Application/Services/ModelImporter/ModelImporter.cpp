#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Nyx
{
    static Math::Vec3f TransformPosition(const aiMatrix4x4& transform, const aiVector3D& v)
    {
        aiVector3D out = transform * v;
        return { out.x, out.y, out.z };
    }

    static Math::Vec3f TransformDirection(const aiMatrix3x3& transform, const aiVector3D& v)
    {
        aiVector3D out = transform * v;
        out.Normalize();
        return { out.x, out.y, out.z };
    }

    static Texture* LoadMaterialTexture(const aiScene* scene, aiMaterial* material, const String& modelPath)
    {
        aiString texPath;

        if (!(material->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS ||
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS))
        {
            spdlog::warn("No texture on this material.");
            return nullptr;
        }

        const aiTexture* embedded = scene->GetEmbeddedTexture(texPath.C_Str());
        if (embedded)
        {

            if (embedded->mHeight == 0)
            {
                return new Texture(
                    reinterpret_cast<const uint8*>(embedded->pcData),
                    static_cast<uint32>(embedded->mWidth),
                    true
                );
            }

            return new Texture(
                reinterpret_cast<const uint8*>(embedded->pcData),
                static_cast<uint32>(embedded->mWidth),
                static_cast<uint32>(embedded->mHeight),
                4
            );
        }

        std::filesystem::path modelFilePath(modelPath);
        std::filesystem::path directory = modelFilePath.parent_path();
        std::filesystem::path fullPath = directory / texPath.C_Str();

        return new Texture(fullPath.string(), true);
    }

    static void AppendMeshToModel(const aiMesh* aiMeshData, const aiMatrix4x4& globalTransform, Model& model)
    {
        auto& vertices = model.GetVertices();
        auto& indices = model.GetIndices();
        auto& sections = model.GetSections();

        const uint32 baseVertex = static_cast<uint32>(vertices.size());
        const uint32 indexOffset = static_cast<uint32>(indices.size());

        aiMatrix3x3 normalMatrix(globalTransform);

        for (uint32 i = 0; i < aiMeshData->mNumVertices; ++i)
        {
            Vertex vertex{};

            vertex.position = TransformPosition(globalTransform, aiMeshData->mVertices[i]);

            if (aiMeshData->HasNormals())
                vertex.normal = TransformDirection(normalMatrix, aiMeshData->mNormals[i]);
            else
                vertex.normal = { 0.0f, 0.0f, 0.0f };

            if (aiMeshData->HasTangentsAndBitangents())
                vertex.tangent = TransformDirection(normalMatrix, aiMeshData->mTangents[i]);
            else
                vertex.tangent = { 0.0f, 0.0f, 0.0f };

            if (aiMeshData->mTextureCoords[0])
            {
                vertex.texCoord =
                {
                    aiMeshData->mTextureCoords[0][i].x,
                    aiMeshData->mTextureCoords[0][i].y
                };
            }
            else
            {
                vertex.texCoord = { 0.0f, 0.0f };
            }

            vertices.push_back(vertex);
        }

        uint32 indexCount = 0;

        for (uint32 i = 0; i < aiMeshData->mNumFaces; ++i)
        {
            const aiFace& face = aiMeshData->mFaces[i];

            for (uint32 j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(baseVertex + face.mIndices[j]);
                ++indexCount;
            }
        }

        MeshSection section{};
        section.indexOffset = indexOffset;
        section.indexCount = indexCount;
        section.materialIndex = aiMeshData->mMaterialIndex;

        sections.push_back(section);
    }

    static void ProcessNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform, Model& model)
    {
        aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;

        for (uint32 i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* aiMeshData = scene->mMeshes[node->mMeshes[i]];
            AppendMeshToModel(aiMeshData, globalTransform, model);
        }

        for (uint32 i = 0; i < node->mNumChildren; ++i)
        {
            ProcessNode(node->mChildren[i], scene, globalTransform, model);
        }
    }

    static Math::Vec3f LoadMaterialBaseColor(aiMaterial* material)
    {
        aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);

        if (aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &color) == AI_SUCCESS)
            return { color.r, color.g, color.b };

        if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
            return { color.r, color.g, color.b };

        return { 1.0f, 1.0f, 1.0f };
    }

    Model ModelImporter::LoadFromFile(const String& filepath)
    {
        Assimp::Importer importer;

        const String absolutePath = ResourceLocator::Get(filepath);

        const aiScene* scene = importer.ReadFile(
            absolutePath,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices
        );

        if (!scene || !scene->mRootNode || scene->mNumMeshes == 0)
        {
            throw std::runtime_error(
                "Assimp failed to load model: " + absolutePath +
                " | Error: " + importer.GetErrorString()
            );
        }

        Model model;
        model.SetName(filepath);

        auto& materials = model.GetMaterials();
        materials.resize(scene->mNumMaterials);

        for (uint32 i = 0; i < scene->mNumMaterials; ++i)
        {
            aiMaterial* material = scene->mMaterials[i];

            materials[i].baseColor = LoadMaterialBaseColor(material);
            materials[i].texture = LoadMaterialTexture(scene, material, absolutePath);
        }

        aiMatrix4x4 identity;
        ProcessNode(scene->mRootNode, scene, identity, model);

        model.UploadToGpu();
        return model;
    }
}