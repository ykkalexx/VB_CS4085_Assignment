#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "Engine/StaticMesh.h"
#include "Factories/Factory.h"
#include "EditorReimportHandler.h"
#include "UObject/ObjectMacros.h"
#include "Misc/FeedbackContext.h"
#include "format3d.generated.h"

// Material structure
USTRUCT()
struct FORMAT3D_API FC3DMaterial
{
    GENERATED_BODY()

    FString Name;
    FLinearColor Diffuse;
    FLinearColor Ambient;
    FLinearColor Specular;
    float Roughness;
    float Metallic;
    float Opacity;
    FString MaterialType;
    bool bTwoSided;

    FC3DMaterial()
        : Diffuse(1.0f, 1.0f, 1.0f)
        , Ambient(0.0f, 0.0f, 0.0f)
        , Specular(0.0f, 0.0f, 0.0f)
        , Roughness(0.5f)
        , Metallic(0.0f)
        , Opacity(1.0f)
        , bTwoSided(false)
    {}
};

// LOD Data structure
USTRUCT()
struct FORMAT3D_API FC3DMeshLOD
{
    GENERATED_BODY()

    TArray<FVector> Vertices;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<int32> Indices;
    TArray<int32> UVIndices;
    TArray<int32> SmoothingGroups;
    int32 LODIndex;

    FC3DMeshLOD() : LODIndex(0) {}
};

// Mesh structure
USTRUCT()
struct FORMAT3D_API FC3DMesh
{
    GENERATED_BODY()

    FString Name;
    FMatrix Transform;
    TArray<FC3DMeshLOD> LODs;
    TArray<FC3DMaterial> Materials;

    FC3DMesh() : Transform(FMatrix::Identity) {}
};

// Node structure
USTRUCT()
struct FORMAT3D_API FC3DNode
{
    GENERATED_BODY()

    FString Name;
    FMatrix Transform;
    TArray<FC3DNode> Children;
    TOptional<FC3DMesh> Mesh;

    FC3DNode() : Transform(FMatrix::Identity) {}
};

// Factory class
UCLASS()
class FORMAT3D_API UCustom3DFactory : public UFactory
{
    GENERATED_BODY()

public:
    UCustom3DFactory();
    virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
    virtual bool FactoryCanImport(const FString& Filename) override;
};

// Module class
class FORMAT3D_API Fformat3dModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    static bool ImportCustom3DFile(const FString& FilePath, UStaticMesh* StaticMesh, FString& ErrorMessage);

private:
    static bool ParseTransformMatrix(const TArray<FString>& Lines, int32& CurrentLine, FMatrix& OutMatrix);
    static bool ParseMeshLOD(const TArray<FString>& Lines, int32& CurrentLine, FC3DMeshLOD& OutLOD);
    static bool ParseNode(const TArray<FString>& Lines, int32& CurrentLine, FC3DNode& OutNode);
    static bool CreateStaticMesh(const FC3DMesh& Mesh, UStaticMesh* StaticMesh);
};