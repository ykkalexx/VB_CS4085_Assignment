// Copyright Epic Games, Inc. All Rights Reserved.

#include "format3d.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshAttributes.h"
#include "MeshDescription.h"
#include "Materials/Material.h"
#include "Modules/ModuleManager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/FeedbackContext.h"
#include "Editor.h"
#include "EditorFramework/AssetImportData.h"


#define LOCTEXT_NAMESPACE "Fformat3dModule"

UCustom3DFactory::UCustom3DFactory()
{
    bCreateNew = false;
    bEditAfterNew = true;
    bEditorImport = true;
    SupportedClass = UStaticMesh::StaticClass();

    Formats.Add(TEXT("c3d;Custom 3D Format File"));
}

bool UCustom3DFactory::FactoryCanImport(const FString& Filename)
{
    const FString Extension = FPaths::GetExtension(Filename).ToLower();
    return Extension == TEXT("c3d");
}

UObject* UCustom3DFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
    // Create a new static mesh
    UStaticMesh* StaticMesh = nullptr;

    // Create the mesh with the provided name in the provided package
    StaticMesh = NewObject<UStaticMesh>(InParent, InName, Flags);
    if (!StaticMesh)
    {
        if (Warn)
            Warn->Log(ELogVerbosity::Error, TEXT("Failed to create static mesh object."));
        bOutOperationCanceled = true;
        return nullptr;
    }

    FString ErrorMessage;
    if (!Fformat3dModule::ImportCustom3DFile(Filename, StaticMesh, ErrorMessage))
    {
        if (Warn)
            Warn->Log(ELogVerbosity::Error, *FString::Printf(TEXT("Failed to import Custom3D file: %s"), *ErrorMessage));
        bOutOperationCanceled = true;
        return nullptr;
    }

    // Return the created mesh
    return StaticMesh;
}


void Fformat3dModule::StartupModule()
{
    // Create and register factory
    if (UCustom3DFactory* Factory = NewObject<UCustom3DFactory>())
    {
        Factory->AddToRoot();
    }
}

void Fformat3dModule::ShutdownModule()
{
    // Nothing special needed here
}


bool Fformat3dModule::ImportCustom3DFile(const FString& FilePath, UObject* Parent, FString& ErrorMessage)
{
    // Read file contents
    TArray<FString> Lines;
    if (!FFileHelper::LoadFileToStringArray(Lines, *FilePath))
    {
        ErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
        return false;
    }

    // Parse file
    TArray<FCustom3DMaterial> Materials;
    TArray<FCustom3DObject> Objects;

    int32 CurrentLine = 0;
    while (CurrentLine < Lines.Num())
    {
        const FString& Line = Lines[CurrentLine];

        if (Line.StartsWith(TEXT("material")))
        {
            FCustom3DMaterial Material;
            if (!ParseMaterial(Lines, CurrentLine, Material))
            {
                ErrorMessage = TEXT("Failed to parse material");
                return false;
            }
            Materials.Add(Material);
        }
        else if (Line.StartsWith(TEXT("node")))
        {
            FCustom3DObject Object;
            if (!ParseObject(Lines, CurrentLine, Object))
            {
                ErrorMessage = TEXT("Failed to parse object");
                return false;
            }
            Objects.Add(Object);
        }
        else
        {
            CurrentLine++;
        }
    }

    // Create meshes
    for (const FCustom3DObject& Object : Objects)
    {
        UStaticMesh* Mesh = nullptr;
        if (!CreateStaticMesh(Object, Parent, Mesh))
        {
            ErrorMessage = FString::Printf(TEXT("Failed to create static mesh for object: %s"), *Object.Name);
            return false;
        }
    }

    return true;
}

bool Fformat3dModule::ParseMaterial(const TArray<FString>& Lines, int32& CurrentLine, FCustom3DMaterial& OutMaterial)
{
    // Store the return value
    FString Name = Lines[CurrentLine].Replace(TEXT("material "), TEXT("")).Replace(TEXT("\""), TEXT(""));
    OutMaterial.Name = Name;
    CurrentLine++;

    while (CurrentLine < Lines.Num() && !Lines[CurrentLine].Contains(TEXT("}")))
    {
        const FString& Line = Lines[CurrentLine];

        if (Line.Contains(TEXT("diffuse")))
        {
            TArray<FString> Values;
            // Store the return value
            int32 NumElements = Line.ParseIntoArray(Values, TEXT(" "));
            if (Values.Num() >= 4)
            {
                OutMaterial.Diffuse = FLinearColor(
                    FCString::Atof(*Values[1]),
                    FCString::Atof(*Values[2]),
                    FCString::Atof(*Values[3]));
            }
        }

        CurrentLine++;
    }
    CurrentLine++;
    return true;
}

struct FCustomFaceIndex
{
    int32 VertexIndex;
    int32 UVIndex;
    int32 NormalIndex;
};

bool Fformat3dModule::ParseObject(const TArray<FString>& Lines, int32& CurrentLine, FCustom3DObject& OutObject)
{
    // Parse object name and increment line
    FString Name = Lines[CurrentLine].Replace(TEXT("node "), TEXT("")).Replace(TEXT("\""), TEXT(""));
    OutObject.Name = Name;
    CurrentLine++;

    // Skip through transform and material sections until we hit vertices
    while (CurrentLine < Lines.Num() && !Lines[CurrentLine].StartsWith(TEXT("v ")))
    {
        CurrentLine++;
    }

    TArray<FVector> Positions;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;

    // Now parse all vertices, normals, UVs and faces
    while (CurrentLine < Lines.Num())
    {
        const FString& Line = Lines[CurrentLine];

        if (Line.StartsWith(TEXT("v ")))
        {
            TArray<FString> Values;
            int32 NumParsed = Line.ParseIntoArray(Values, TEXT(" "));
            if (Values.Num() >= 4)
            {
                FVector Position(
                    FCString::Atof(*Values[1]),
                    FCString::Atof(*Values[2]),
                    FCString::Atof(*Values[3]));
                Positions.Add(Position);
            }
        }
        else if (Line.StartsWith(TEXT("vn ")))
        {
            TArray<FString> Values;
            int32 NumParsed = Line.ParseIntoArray(Values, TEXT(" "));
            if (Values.Num() >= 4)
            {
                Normals.Add(FVector(
                    FCString::Atof(*Values[1]),
                    FCString::Atof(*Values[2]),
                    FCString::Atof(*Values[3])));
            }
        }
        else if (Line.StartsWith(TEXT("vt ")))
        {
            TArray<FString> Values;
            int32 NumParsed = Line.ParseIntoArray(Values, TEXT(" "));
            if (Values.Num() >= 3)
            {
                UVs.Add(FVector2D(
                    FCString::Atof(*Values[1]),
                    FCString::Atof(*Values[2])));
            }
        }
        else if (Line.StartsWith(TEXT("f ")))
        {
            TArray<FString> FaceElements;
            int32 NumParsed = Line.ParseIntoArray(FaceElements, TEXT(" "));

            // Skip the first element ("f") and the last element ("mat_id X")
            for (int32 i = 1; i < FaceElements.Num() - 2; i++)
            {
                FString Element = FaceElements[i];
                FString NoDecimal = Element.Replace(TEXT(".0"), TEXT(""));

                TArray<FString> Indices;
                int32 NumIndices = NoDecimal.ParseIntoArray(Indices, TEXT("/"));

                if (Indices.Num() > 0)
                {
                    int32 VertexIndex = FCString::Atoi(*Indices[0]) - 1;
                    if (VertexIndex >= 0)
                    {
                        OutObject.Indices.Add(VertexIndex);
                    }
                }
            }
        }
        else if (Line.StartsWith(TEXT("# Collision"))) // Stop when we hit collision section
        {
            break;
        }

        CurrentLine++;
    }

    // Create vertices
    for (int32 i = 0; i < Positions.Num(); i++)
    {
        FCustom3DVertex Vertex;
        Vertex.Position = Positions[i];
        Vertex.Normal = (i < Normals.Num()) ? Normals[i] : FVector::UpVector;
        Vertex.UV = (i < UVs.Num()) ? UVs[i] : FVector2D::ZeroVector;
        OutObject.Vertices.Add(Vertex);
    }

    UE_LOG(LogTemp, Warning, TEXT("Parsed %s: %d vertices, %d indices"),
        *OutObject.Name, OutObject.Vertices.Num(), OutObject.Indices.Num());

    return true;
}

bool Fformat3dModule::CreateStaticMesh(const FCustom3DObject& Object, UObject* Parent, UStaticMesh*& OutMesh)
{
    FString MeshName = FPaths::GetBaseFilename(Parent->GetName());
    OutMesh = NewObject<UStaticMesh>(Parent, *MeshName, RF_Public | RF_Standalone);

    // Initialize render data first
    OutMesh->InitResources();

    // Create mesh description
    FMeshDescription* MeshDesc = new FMeshDescription();
    FStaticMeshAttributes(MeshDesc).Register();

    // Add vertices and faces
    TArray<FVertexID> VertexIDs;
    TVertexAttributesRef<FVector3f> VertexPositions =
        MeshDesc->VertexAttributes().GetAttributesRef<FVector3f>(MeshAttribute::Vertex::Position);

    // Add vertices
    for (const FCustom3DVertex& Vertex : Object.Vertices)
    {
        FVertexID VertexID = MeshDesc->CreateVertex();
        VertexPositions.Set(VertexID, FVector3f(Vertex.Position));
        VertexIDs.Add(VertexID);
    }

    FPolygonGroupID PolygonGroupID = MeshDesc->CreatePolygonGroup();

    // Create triangles
    for (int32 i = 0; i < Object.Indices.Num(); i += 3)
    {
        if (i + 2 >= Object.Indices.Num()) continue;

        TArray<FVertexInstanceID> VertexInstanceIDs;

        for (int32 j = 0; j < 3; ++j)
        {
            int32 Index = Object.Indices[i + j];
            if (Index < 0 || Index >= Object.Vertices.Num()) continue;

            FVertexID VertexID = VertexIDs[Index];
            FVertexInstanceID InstanceID = MeshDesc->CreateVertexInstance(VertexID);

            MeshDesc->VertexInstanceAttributes().GetAttributesRef<FVector3f>(MeshAttribute::VertexInstance::Normal)
                .Set(InstanceID, FVector3f(Object.Vertices[Index].Normal));
            MeshDesc->VertexInstanceAttributes().GetAttributesRef<FVector2f>(MeshAttribute::VertexInstance::TextureCoordinate)
                .Set(InstanceID, 0, FVector2f(Object.Vertices[Index].UV));

            VertexInstanceIDs.Add(InstanceID);
        }

        if (VertexInstanceIDs.Num() == 3)
        {
            MeshDesc->CreatePolygon(PolygonGroupID, VertexInstanceIDs);
        }
    }

    // Setup source model
    FStaticMeshSourceModel& SrcModel = OutMesh->AddSourceModel();
    SrcModel.BuildSettings.bRecomputeNormals = false;
    SrcModel.BuildSettings.bRecomputeTangents = true;
    SrcModel.BuildSettings.bRemoveDegenerates = false;
    SrcModel.BuildSettings.bUseMikkTSpace = false;
    SrcModel.BuildSettings.bUseFullPrecisionUVs = true;

    // Add default material
    OutMesh->GetStaticMaterials().Add(FStaticMaterial(UMaterial::GetDefaultMaterial(MD_Surface)));

    // Build the mesh
    OutMesh->BuildFromMeshDescription(*MeshDesc);
    delete MeshDesc;

    OutMesh->PostEditChange();

    return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Fformat3dModule, format3d)