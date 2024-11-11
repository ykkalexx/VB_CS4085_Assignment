#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CustomFormatFactory.generated.h" 

USTRUCT()
struct FCustomMaterialData 
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
    bool TwoSided;
}

USTRUCT()
struct FCustomMeshData
{
    GENERATED_BODY()

    TArray<FVector> Vertices;
    TArray<FVector> Normals;
    TArray<FVector> Tangents;
    TArray<FVector2D> UVs;
    TArray<int32> Indices;
    FString MaterialName;
};

class UCustomFormatFactory : public UFactory
{
    GENERATED_BODY()

public:
    UCustomFormatFactory();

    virtual bool FactoryCanImport(const FString& filename) override;
    virutal UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, 
        const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

private:
    bool ParseCustomFormat(const FString& Filename, TArray<FCustomMeshData>& OutMeshes, 
        TArray<FCustomMaterialData>& OutMaterials);
    UStaticMesh* CreateStaticMesh(const FCustomMeshData& MeshData, UObject* InParent, FName InName);
    UMaterialInterface* CreateMaterial(const FCustomMaterialData& MaterialData, UObject* InParent);
}