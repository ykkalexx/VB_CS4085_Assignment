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
    TArray<FCustomMeshData> Meshes;
    TArray<FCustomMaterialData> Materials;

    if (!ParseCustomFormat(Filename, Meshes, Materials))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse custom format file"));
        return nullptr;
    }

    // Debug output
    UE_LOG(LogTemp, Log, TEXT("Parsed %d materials and %d meshes"), Materials.Num(), Meshes.Num());
    
    for (const FCustomMaterialData& Mat : Materials)
    {
        UE_LOG(LogTemp, Log, TEXT("Material: %s, Diffuse: (%f, %f, %f)"), 
            *Mat.Name, 
            Mat.Diffuse.R, 
            Mat.Diffuse.G, 
            Mat.Diffuse.B);
    }

    for (const FCustomMeshData& Mesh : Meshes)
    {
        UE_LOG(LogTemp, Log, TEXT("Mesh has %d vertices"), Mesh.Vertices.Num());
    }

    // We'll implement mesh creation next
    return nullptr;
}

bool UCustomFormatFactory::ParseCustomFormat(const FString& Filename, TArray<FCustomMeshData>& OutMeshes, 
    TArray<FCustomMaterialData>& OutMaterials)
{
    // Read the file
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *Filename))
    {
        return false;
    }

    // Split into lines
    TArray<FString> Lines;
    FileContent.ParseIntoArrayLines(Lines, false);

    // Check format header
    if (!Lines[0].Contains(TEXT("FORMAT Custom3D")))
    {
        return false;
    }

    FCustomMeshData CurrentMesh;
    FCustomMaterialData CurrentMaterial;
    bool bParsingMaterial = false;

    // Parse line by line
    for (int32 i = 0; i < Lines.Num(); i++)
    {
        const FString& Line = Lines[i];

        // Skip empty lines and comments that aren't section headers
        if (Line.IsEmpty() || (Line.StartsWith("#") && !Line.Contains("Materials") && !Line.Contains("Vertices")))
        {
            continue;
        }

        // Start of Materials section
        if (Line.Contains("# Materials"))
        {
            bParsingMaterial = true;
            continue;
        }
        // Start of Vertices section
        else if (Line.Contains("# Vertices"))
        {
            bParsingMaterial = false;
            continue;
        }

        if (bParsingMaterial)
        {
            // Parse material data
            if (Line.StartsWith("material"))
            {
                // If we were parsing a previous material, add it to the list
                if (!CurrentMaterial.Name.IsEmpty())
                {
                    OutMaterials.Add(CurrentMaterial);
                }
                
                // Start new material
                CurrentMaterial = FCustomMaterialData();
                // Extract material name between quotes
                FString MaterialName = Line;
                MaterialName.RemoveFromStart("material ");
                MaterialName.RemoveFromStart("\"");
                MaterialName.RemoveFromEnd(" {");
                MaterialName.RemoveFromEnd("\"");
                CurrentMaterial.Name = MaterialName;
            }
            else if (Line.Contains("diffuse"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 4)
                {
                    CurrentMaterial.Diffuse = FLinearColor(
                        FCString::Atof(*Parts[1]),
                        FCString::Atof(*Parts[2]),
                        FCString::Atof(*Parts[3])
                    );
                }
            }
            else if (Line.Contains("ambient"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 4)
                {
                    CurrentMaterial.Ambient = FLinearColor(
                        FCString::Atof(*Parts[1]),
                        FCString::Atof(*Parts[2]),
                        FCString::Atof(*Parts[3])
                    );
                }
            }
            else if (Line.Contains("specular"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 4)
                {
                    CurrentMaterial.Specular = FLinearColor(
                        FCString::Atof(*Parts[1]),
                        FCString::Atof(*Parts[2]),
                        FCString::Atof(*Parts[3])
                    );
                }
            }
            else if (Line.Contains("roughness"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 2)
                {
                    CurrentMaterial.Roughness = FCString::Atof(*Parts[1]);
                }
            }
            else if (Line.Contains("metallic"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 2)
                {
                    CurrentMaterial.Metallic = FCString::Atof(*Parts[1]);
                }
            }
            else if (Line.Contains("opacity"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 2)
                {
                    CurrentMaterial.Opacity = FCString::Atof(*Parts[1]);
                }
            }
            else if (Line.Contains("twoSided"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 2)
                {
                    CurrentMaterial.TwoSided = FCString::Atoi(*Parts[1]) != 0;
                }
            }
            else if (Line.Contains("materialType"))
            {
                TArray<FString> Parts;
                Line.ParseInto(Parts, TEXT(" "), true);
                if (Parts.Num() >= 2)
                {
                    FString Type = Parts[1];
                    Type.RemoveFromStart("\"");
                    Type.RemoveFromEnd("\"");
                    CurrentMaterial.MaterialType = Type;
                }
            }
        }
        else if (Line.StartsWith("v ")) // Vertex line
        {
            TArray<FString> Parts;
            Line.ParseInto(Parts, TEXT(" "), true);
            if (Parts.Num() >= 4)
            {
                FVector Vertex(
                    FCString::Atof(*Parts[1]),
                    FCString::Atof(*Parts[2]),
                    FCString::Atof(*Parts[3])
                );
                CurrentMesh.Vertices.Add(Vertex);
            }
        }
    }

    // Add the last material if we were parsing one
    if (!CurrentMaterial.Name.IsEmpty())
    {
        OutMaterials.Add(CurrentMaterial);
    }

    // Add the mesh data if we have vertices
    if (CurrentMesh.Vertices.Num() > 0)
    {
        OutMeshes.Add(CurrentMesh);
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