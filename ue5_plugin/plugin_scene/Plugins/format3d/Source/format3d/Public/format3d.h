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

struct FCustom3DMaterial
{
    FString Name;
    FLinearColor Diffuse;
    FLinearColor Ambient;
    FLinearColor Specular;
    float Roughness;
    float Metallic;
    float Opacity;
    FString MaterialType;
    bool bTwoSided;
};

struct FCustom3DVertex
{
    FVector Position;
    FVector Normal;
    FVector Tangent;
    FVector2D UV;
};

struct FCustom3DObject
{
    FString Name;
    FTransform Transform;
    FString MaterialName;
    TArray<FCustom3DVertex> Vertices;
    TArray<int32> Indices;
};

UCLASS()
class FORMAT3D_API UCustom3DFactory : public UFactory
{
    GENERATED_BODY()

public:
    UCustom3DFactory();

    //~ Begin UFactory Interface
    virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
    virtual bool FactoryCanImport(const FString& Filename) override;
    //~ End UFactory Interface
};

class Fformat3dModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    static bool ImportCustom3DFile(const FString& FilePath, UObject* Parent, FString& ErrorMessage);

private:
    static bool ParseMaterial(const TArray<FString>& Lines, int32& CurrentLine, FCustom3DMaterial& OutMaterial);
    static bool ParseObject(const TArray<FString>& Lines, int32& CurrentLine, FCustom3DObject& OutObject);
    static bool CreateStaticMesh(const FCustom3DObject& Object, UObject* Parent, UStaticMesh*& OutMesh);
};