#include "CustomFormatFactory.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Misc/FileHelper.h"
#include "StaticMeshAttributes.h"
#include "MeshDescription.h"

UCustomFormatFactory::UCustomFormatFactory()
{
    bCreateNew = false;
    bEditAfterNew = true;
    bEditorImport = true;
    SupportedClass = UStaticMesh::StaticClass();

    // Add the file extension for your format
    Formats.Add(TEXT("c3d;Custom 3D Format Files")
}

bool UCustomFormatFactory::FactoryCanImport(const FString& Filename)
{
    const FString Extension = FPaths::GetExtension(Filename);
    return Extension.Compare(TEXT("c3d"), ESearchCase::IgnoreCase) == 0;
}

UObject* UCustomFormatFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, 
    const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
    // arrays to store our parsed data
    TArray<FCustomMeshData> Meshes;
    TArray<FCustomMaterialData> Materials;

    // parsing the file
    if (!ParseCustomFormat(Filename, Meshes, Materials))
    {
        return nullptr;
    }

    // For now, just create the first mesh
    if (Meshes.Num() > 0)
    {
        return CreateStaticMesh(Meshes[0], InParent, InName);
    }

    return nullptr;
}

bool UCustomFormatFactory::ParseCustomFormat(const FString& Filename, TArray<FCustomMeshData>& OutMeshes, 
    TArray<FCustomMaterialData>& OutMaterials)
{
    // reading the file
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *Filename))
    {
        return false;
    }

    // split into lines
    TArray<FString> Lines;
    FileContent.ParseIntoArrayLines(Lines, false);

    // checking format header
    if (!Lines[0].Contains(TEXT("FORMAT Custom3D")))
    {
        return false;
    }

    return true;
}

UStaticMesh* UCustomFormatFactory::CreateStaticMesh(const FCustomMeshData& MeshData, UObject* InParent, FName InName)
{
    UStaticMesh* StaticMesh = NewObject<UStaticMesh>(InParent, InName, RF_Public | RF_Standalone);
    
    if (StaticMesh)
    {
        // Create mesh description
        FMeshDescription* MeshDescription = StaticMesh->CreateMeshDescription(0);
        if (MeshDescription)
        {
            StaticMesh->CommitMeshDescription(0);
            StaticMesh->Build();
        }
    }

    return StaticMesh;
}

UMaterialInterface* UCustomFormatFactory::CreateMaterial(const FCustomMaterialData& MaterialData, UObject* InParent)
{
    // ill do this later
    return nullptr;
}