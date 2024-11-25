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
    OutObject.Name = Lines[CurrentLine].Replace(TEXT("node "), TEXT("")).Replace(TEXT("\""), TEXT(""));
    CurrentLine++;

    TArray<FVector> Positions;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FCustomFaceIndex> FaceIndices;

    UE_LOG(LogTemp, Warning, TEXT("Starting to parse object: %s"), *OutObject.Name);

    while (CurrentLine < Lines.Num() && !Lines[CurrentLine].Contains(TEXT("}")))
    {
        const FString& Line = Lines[CurrentLine];

        if (Line.StartsWith(TEXT("v ")))
        {
            TArray<FString> Values;
            int32 NumElements = Line.ParseIntoArray(Values, TEXT(" "));
            if (Values.Num() >= 4)
            {
                FVector Position(
                    FCString::Atof(*Values[1]),
                    FCString::Atof(*Values[2]),
                    FCString::Atof(*Values[3]));
                Positions.Add(Position);
                UE_LOG(LogTemp, Warning, TEXT("Added vertex: X=%f, Y=%f, Z=%f"),
                    Position.X, Position.Y, Position.Z);
            }
        }
        else if (Line.StartsWith(TEXT("vn ")))
        {
            TArray<FString> Values;
            int32 NumElements = Line.ParseIntoArray(Values, TEXT(" "));
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
            int32 NumElements = Line.ParseIntoArray(Values, TEXT(" "));
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
            int32 NumElements = Line.ParseIntoArray(FaceElements, TEXT(" "));

            // Process face indices (skip "f" at index 0 and "mat_id X" at the end)
            for (int32 i = 1; i <= 3; i++)
            {
                if (i >= FaceElements.Num()) continue;

                FString VertexData = FaceElements[i];
                TArray<FString> Indices;
                VertexData.ParseIntoArray(Indices, TEXT("/"));

                FCustomFaceIndex FaceIndex;
                FaceIndex.VertexIndex = -1;
                FaceIndex.UVIndex = -1;
                FaceIndex.NormalIndex = -1;

                if (Indices.Num() >= 1)
                {
                    // Remove .0 from the indices
                    FString VertIndexStr = Indices[0].Replace(TEXT(".0"), TEXT(""));
                    FaceIndex.VertexIndex = FCString::Atoi(*VertIndexStr) - 1;
                }
                if (Indices.Num() >= 2)
                {
                    FString UVIndexStr = Indices[1].Replace(TEXT(".0"), TEXT(""));
                    FaceIndex.UVIndex = FCString::Atoi(*UVIndexStr) - 1;
                }
                if (Indices.Num() >= 3)
                {
                    FString NormalIndexStr = Indices[2].Replace(TEXT(".0"), TEXT(""));
                    FaceIndex.NormalIndex = FCString::Atoi(*NormalIndexStr) - 1;
                }

                if (FaceIndex.VertexIndex >= 0)
                {
                    FaceIndices.Add(FaceIndex);
                }
            }
        }

        CurrentLine++;
    }

    // Create vertices
    for (int32 i = 0; i < Positions.Num(); i++)
    {
        FCustom3DVertex Vertex;
        Vertex.Position = Positions[i];
        OutObject.Vertices.Add(Vertex);
    }

    // Process face indices
    for (const FCustomFaceIndex& FaceIndex : FaceIndices)
    {
        OutObject.Indices.Add(FaceIndex.VertexIndex);

        // Update vertex attributes
        if (FaceIndex.VertexIndex >= 0 && FaceIndex.VertexIndex < OutObject.Vertices.Num())
        {
            if (FaceIndex.NormalIndex >= 0 && FaceIndex.NormalIndex < Normals.Num())
            {
                OutObject.Vertices[FaceIndex.VertexIndex].Normal = Normals[FaceIndex.NormalIndex];
            }
            if (FaceIndex.UVIndex >= 0 && FaceIndex.UVIndex < UVs.Num())
            {
                OutObject.Vertices[FaceIndex.VertexIndex].UV = UVs[FaceIndex.UVIndex];
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Parsed vertices: %d, indices: %d"), OutObject.Vertices.Num(), OutObject.Indices.Num());

    CurrentLine++;
    return true;
}

bool Fformat3dModule::CreateStaticMesh(const FCustom3DObject& Object, UObject* Parent, UStaticMesh*& OutMesh)
{
    FString MeshName = Object.Name + TEXT("_Mesh");
    OutMesh = NewObject<UStaticMesh>(Parent, *MeshName, RF_Public | RF_Standalone);

    // Create mesh description
    FMeshDescription* MeshDesc = new FMeshDescription();
    FStaticMeshAttributes Attributes(*MeshDesc);
    Attributes.Register();

    // Add vertices
    TArray<FVertexID> VertexIDs;
    TVertexAttributesRef<FVector3f> VertexPositions =
        MeshDesc->VertexAttributes().GetAttributesRef<FVector3f>(MeshAttribute::Vertex::Position);

    for (const FCustom3DVertex& Vertex : Object.Vertices)
    {
        FVertexID VertexID = MeshDesc->CreateVertex();
        VertexPositions.Set(VertexID, FVector3f(Vertex.Position));
        VertexIDs.Add(VertexID);
    }

    // Create polygon group
    FPolygonGroupID PolygonGroupID = MeshDesc->CreatePolygonGroup();

    // Create triangles
    for (int32 i = 0; i < Object.Indices.Num(); i += 3)
    {
        if (i + 2 >= Object.Indices.Num()) continue;

        TArray<FVertexInstanceID> VertexInstanceIDs;
        TVertexInstanceAttributesRef<FVector3f> InstanceNormals =
            MeshDesc->VertexInstanceAttributes().GetAttributesRef<FVector3f>(MeshAttribute::VertexInstance::Normal);
        TVertexInstanceAttributesRef<FVector2f> InstanceUVs =
            MeshDesc->VertexInstanceAttributes().GetAttributesRef<FVector2f>(MeshAttribute::VertexInstance::TextureCoordinate);

        bool ValidTriangle = true;
        for (int32 j = 0; j < 3; ++j)
        {
            int32 Index = Object.Indices[i + j];
            if (Index < 0 || Index >= Object.Vertices.Num())
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid vertex index: %d"), Index);
                ValidTriangle = false;
                break;
            }

            FVertexID VertexID = VertexIDs[Index];
            FVertexInstanceID InstanceID = MeshDesc->CreateVertexInstance(VertexID);

            // Use the actual normal from the vertex
            InstanceNormals.Set(InstanceID, FVector3f(Object.Vertices[Index].Normal));
            InstanceUVs.Set(InstanceID, 0, FVector2f(Object.Vertices[Index].UV));

            VertexInstanceIDs.Add(InstanceID);
        }

        if (ValidTriangle && VertexInstanceIDs.Num() == 3)
        {
            FPolygonID PolygonID = MeshDesc->CreatePolygon(PolygonGroupID, VertexInstanceIDs);
            MeshDesc->ComputePolygonTriangulation(PolygonID);
        }
    }

    // Add material
    UMaterial* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
    FStaticMaterial StaticMat(DefaultMaterial);
    StaticMat.MaterialInterface = DefaultMaterial;
    StaticMat.UVChannelData = FMeshUVChannelInfo(true);
    OutMesh->GetStaticMaterials().Add(StaticMat);

    // Set up render data
    OutMesh->NeverStream = true;

    // Source model setup
    FStaticMeshSourceModel& SrcModel = OutMesh->AddSourceModel();
    SrcModel.BuildSettings.bRecomputeNormals = true;
    SrcModel.BuildSettings.bRecomputeTangents = true;
    SrcModel.BuildSettings.bUseMikkTSpace = true;
    SrcModel.BuildSettings.bGenerateLightmapUVs = true;
    SrcModel.BuildSettings.bRemoveDegenerates = true;
    SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = true;
    SrcModel.BuildSettings.bUseFullPrecisionUVs = true;

    // Set up lightmap
    OutMesh->LightMapResolution = 64;
    OutMesh->LightMapCoordinateIndex = 1;

    // Build the mesh
    TArray<const FMeshDescription*> MeshDescriptionPtrs;
    MeshDescriptionPtrs.Add(MeshDesc);
    OutMesh->BuildFromMeshDescriptions(MeshDescriptionPtrs);

    // Clean up
    delete MeshDesc;

    // Set up collision and build
    OutMesh->CreateBodySetup();
    OutMesh->Build();
    OutMesh->PostEditChange();
    OutMesh->MarkPackageDirty();

    UE_LOG(LogTemp, Warning, TEXT("Mesh creation completed"));

    return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Fformat3dModule, format3d)