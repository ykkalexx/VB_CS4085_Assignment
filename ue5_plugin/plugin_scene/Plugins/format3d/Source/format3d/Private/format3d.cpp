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

// Factory Implementation
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
    UStaticMesh* StaticMesh = NewObject<UStaticMesh>(InParent, InName, Flags);
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

    return StaticMesh;
}

void Fformat3dModule::StartupModule()
{
    if (UCustom3DFactory* Factory = NewObject<UCustom3DFactory>())
    {
        Factory->AddToRoot();
    }
}

void Fformat3dModule::ShutdownModule()
{
}

// Helper Functions
bool Fformat3dModule::ParseTransformMatrix(const TArray<FString>& Lines, int32& CurrentLine, FMatrix& OutMatrix)
{
    if (CurrentLine + 4 >= Lines.Num()) return false;

    for (int32 Row = 0; Row < 4; Row++)
    {
        TArray<FString> Values;
        Lines[CurrentLine + Row].ParseIntoArray(Values, TEXT(" "));
        if (Values.Num() >= 4)
        {
            for (int32 Col = 0; Col < 4; Col++)
            {
                OutMatrix.M[Row][Col] = FCString::Atof(*Values[Col]);
            }
        }
    }
    CurrentLine += 4;
    return true;
}

bool Fformat3dModule::ParseMeshLOD(const TArray<FString>& Lines, int32& CurrentLine, FC3DMeshLOD& OutLOD)
{
    UE_LOG(LogTemp, Log, TEXT("Starting to parse mesh LOD at line %d"), CurrentLine);

    while (CurrentLine < Lines.Num())
    {
        const FString& Line = Lines[CurrentLine];
        UE_LOG(LogTemp, Log, TEXT("Processing line: %s"), *Line);

        if (Line.StartsWith(TEXT("LOD")))
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT(" "));
            OutLOD.LODIndex = FCString::Atoi(*Parts.Last());
            CurrentLine++;
            continue;
        }

        if (Line.StartsWith(TEXT("VERTICES")))
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT(" "));
            int32 VertexCount = FCString::Atoi(*Parts.Last());
            CurrentLine++;

            UE_LOG(LogTemp, Log, TEXT("Parsing %d vertices"), VertexCount);

            for (int32 i = 0; i < VertexCount; i++)
            {
                if (CurrentLine >= Lines.Num()) break;

                const FString& VertexLine = Lines[CurrentLine++];
                TArray<FString> Values;
                VertexLine.ParseIntoArray(Values, TEXT(" "));

                if (Values.Num() >= 7 && Values[0] == TEXT("v"))
                {
                    FVector Position(
                        FCString::Atof(*Values[1]),
                        FCString::Atof(*Values[2]),
                        FCString::Atof(*Values[3])
                    );
                    OutLOD.Vertices.Add(Position);

                    if (Values[4] == TEXT("vn"))
                    {
                        FVector Normal(
                            FCString::Atof(*Values[5]),
                            FCString::Atof(*Values[6]),
                            FCString::Atof(*Values[7])
                        );
                        OutLOD.Normals.Add(Normal);
                    }
                }
            }

            UE_LOG(LogTemp, Log, TEXT("Added %d vertices and normals"), OutLOD.Vertices.Num());
        }
        else if (Line.StartsWith(TEXT("FACES")))
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT(" "));
            int32 FaceCount = FCString::Atoi(*Parts.Last());
            CurrentLine++;

            UE_LOG(LogTemp, Log, TEXT("Starting to parse %d faces"), FaceCount);

            for (int32 i = 0; i < FaceCount && CurrentLine < Lines.Num(); i++)
            {
                const FString& FaceLine = Lines[CurrentLine++];
                TArray<FString> Values;
                FaceLine.ParseIntoArray(Values, TEXT(" "));

                if (Values.Num() >= 4 && Values[0] == TEXT("f"))
                {
                    OutLOD.Indices.Add(FCString::Atoi(*Values[1]));
                    OutLOD.Indices.Add(FCString::Atoi(*Values[2]));
                    OutLOD.Indices.Add(FCString::Atoi(*Values[3]));

                    int32 SmoothingGroup = 0;
                    if (Values.Num() >= 6 && Values[4] == TEXT("sm"))
                    {
                        SmoothingGroup = FCString::Atoi(*Values[5]);
                    }
                    OutLOD.SmoothingGroups.Add(SmoothingGroup);

                    UE_LOG(LogTemp, Log, TEXT("Added face indices: %d, %d, %d"),
                        FCString::Atoi(*Values[1]),
                        FCString::Atoi(*Values[2]),
                        FCString::Atoi(*Values[3]));
                }
            }
        }
        else if (Line.StartsWith(TEXT("UVS")))
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT(" "));
            int32 UVCount = FCString::Atoi(*Parts.Last());
            CurrentLine++;

            UE_LOG(LogTemp, Log, TEXT("Starting to parse %d UVs"), UVCount);

            for (int32 i = 0; i < UVCount && CurrentLine < Lines.Num(); i++)
            {
                const FString& UVLine = Lines[CurrentLine++];
                TArray<FString> Values;
                UVLine.ParseIntoArray(Values, TEXT(" "));

                if (Values.Num() >= 3 && Values[0] == TEXT("uv"))
                {
                    OutLOD.UVs.Add(FVector2D(
                        FCString::Atof(*Values[1]),
                        FCString::Atof(*Values[2])
                    ));
                }
            }

            UE_LOG(LogTemp, Log, TEXT("Added %d UVs"), OutLOD.UVs.Num());
        }
        else if (Line.StartsWith(TEXT("UVFACES")))
        {
            TArray<FString> Parts;
            Line.ParseIntoArray(Parts, TEXT(" "));
            int32 UVFaceCount = FCString::Atoi(*Parts.Last());
            CurrentLine++;

            UE_LOG(LogTemp, Log, TEXT("Parsing %d UV faces"), UVFaceCount);

            for (int32 i = 0; i < UVFaceCount && CurrentLine < Lines.Num(); i++)
            {
                const FString& UVFaceLine = Lines[CurrentLine++];
                TArray<FString> Values;
                UVFaceLine.ParseIntoArray(Values, TEXT(" "));

                if (Values.Num() >= 4 && Values[0] == TEXT("uvf"))
                {
                    OutLOD.UVIndices.Add(FCString::Atoi(*Values[1]));
                    OutLOD.UVIndices.Add(FCString::Atoi(*Values[2]));
                    OutLOD.UVIndices.Add(FCString::Atoi(*Values[3]));
                }
            }

            UE_LOG(LogTemp, Log, TEXT("Added %d UV indices"), OutLOD.UVIndices.Num());
        }
        else if (Line.StartsWith(TEXT("COLLISION")))
        {
            break;
        }
        else
        {
            CurrentLine++;
        }
    }

    bool bValid = (OutLOD.Vertices.Num() > 0) && (OutLOD.Indices.Num() > 0);
    UE_LOG(LogTemp, Log, TEXT("Mesh LOD parsing complete. Valid: %d, Vertices: %d, Indices: %d"),
        bValid, OutLOD.Vertices.Num(), OutLOD.Indices.Num());

    if (!bValid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid mesh data - Vertices: %d, Indices: %d"),
            OutLOD.Vertices.Num(), OutLOD.Indices.Num());
    }

    return bValid;
}

bool Fformat3dModule::ParseNode(const TArray<FString>& Lines, int32& CurrentLine, FC3DNode& OutNode)
{
    const FString& Line = Lines[CurrentLine];
    OutNode.Name = Line.Replace(TEXT("NODE "), TEXT("")).Replace(TEXT("\""), TEXT(""));
    CurrentLine++;

    UE_LOG(LogTemp, Log, TEXT("Parsing node: %s"), *OutNode.Name);
    bool bFoundValidMesh = false;
    FMatrix CurrentTransform = FMatrix::Identity;

    while (CurrentLine < Lines.Num())
    {
        const FString& CurrentLine_Str = Lines[CurrentLine];

        if (CurrentLine_Str.StartsWith(TEXT("TRANSFORM")))
        {
            UE_LOG(LogTemp, Log, TEXT("Found TRANSFORM for node %s"), *OutNode.Name);
            CurrentLine++;
            ParseTransformMatrix(Lines, CurrentLine, CurrentTransform);
            OutNode.Transform = CurrentTransform;
        }
        else if (CurrentLine_Str.StartsWith(TEXT("MESH")))
        {
            UE_LOG(LogTemp, Log, TEXT("Found MESH for node %s"), *OutNode.Name);
            FC3DMesh Mesh;
            Mesh.Name = CurrentLine_Str.Replace(TEXT("MESH "), TEXT("")).Replace(TEXT("\""), TEXT(""));
            Mesh.Transform = CurrentTransform; // Use current transform
            CurrentLine++;

            if (Lines[CurrentLine].StartsWith(TEXT("TRANSFORM")))
            {
                CurrentLine++;
                FMatrix MeshTransform;
                ParseTransformMatrix(Lines, CurrentLine, MeshTransform);
                Mesh.Transform = Mesh.Transform * MeshTransform;
            }

            FC3DMeshLOD LOD;
            if (ParseMeshLOD(Lines, CurrentLine, LOD))
            {
                UE_LOG(LogTemp, Log, TEXT("Successfully parsed LOD for mesh %s. Vertices: %d, Indices: %d"),
                    *Mesh.Name, LOD.Vertices.Num(), LOD.Indices.Num());
                Mesh.LODs.Add(LOD);
                OutNode.Mesh = MoveTemp(Mesh);
                bFoundValidMesh = true;
                return true; // Return true immediately when we find a valid mesh
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to parse mesh LOD for node %s"), *OutNode.Name);
            }
        }
        else if (CurrentLine_Str.StartsWith(TEXT("NODE")))
        {
            FC3DNode ChildNode;
            if (ParseNode(Lines, CurrentLine, ChildNode))
            {
                OutNode.Children.Add(MoveTemp(ChildNode));
                bFoundValidMesh = true;
            }
        }
        else
        {
            CurrentLine++;
            if (CurrentLine >= Lines.Num())
            {
                break;
            }
        }
    }

    return bFoundValidMesh;
}

bool Fformat3dModule::CreateStaticMesh(const FC3DMesh& Mesh, UStaticMesh* StaticMesh)
{
    if (!StaticMesh || Mesh.LODs.Num() == 0) return false;

    for (const FC3DMeshLOD& LOD : Mesh.LODs)
    {
        if (LOD.Vertices.Num() == 0 || LOD.Indices.Num() == 0) continue;

        FMeshDescription MeshDesc;
        FStaticMeshAttributes StaticMeshAttrs(MeshDesc);
        StaticMeshAttrs.Register();

        TArray<FVertexID> VertexIDs;
        TVertexAttributesRef<FVector3f> VertexPositions =
            MeshDesc.VertexAttributes().GetAttributesRef<FVector3f>(MeshAttribute::Vertex::Position);

        // Add vertices
        for (const FVector& Vertex : LOD.Vertices)
        {
            FVertexID VertexID = MeshDesc.CreateVertex();
            FVector TransformedPos = Mesh.Transform.TransformPosition(Vertex);
            VertexPositions[VertexID] = FVector3f(TransformedPos.X, TransformedPos.Y, TransformedPos.Z);
            VertexIDs.Add(VertexID);
        }

        // Pre-calculate smoothed normals
        TArray<FVector> SmoothedNormals;
        SmoothedNormals.SetNum(LOD.Vertices.Num());
        TArray<int32> NormalCounts;
        NormalCounts.SetNum(LOD.Vertices.Num());

        // First pass: Calculate face normals and contribute to vertex normals
        for (int32 i = 0; i < LOD.Indices.Num(); i += 3)
        {
            if (i + 2 >= LOD.Indices.Num()) continue;

            int32 Index0 = LOD.Indices[i];
            int32 Index1 = LOD.Indices[i + 1];
            int32 Index2 = LOD.Indices[i + 2];

            if (Index0 < LOD.Vertices.Num() && Index1 < LOD.Vertices.Num() && Index2 < LOD.Vertices.Num())
            {
                FVector V0 = LOD.Vertices[Index0];
                FVector V1 = LOD.Vertices[Index1];
                FVector V2 = LOD.Vertices[Index2];

                FVector FaceNormal = FVector::CrossProduct(V1 - V0, V2 - V0).GetSafeNormal();

                // Contribute to vertex normals
                SmoothedNormals[Index0] += FaceNormal;
                SmoothedNormals[Index1] += FaceNormal;
                SmoothedNormals[Index2] += FaceNormal;

                NormalCounts[Index0]++;
                NormalCounts[Index1]++;
                NormalCounts[Index2]++;
            }
        }

        // Average and normalize the smoothed normals
        for (int32 i = 0; i < SmoothedNormals.Num(); i++)
        {
            if (NormalCounts[i] > 0)
            {
                SmoothedNormals[i] = (SmoothedNormals[i] / NormalCounts[i]).GetSafeNormal();
            }
        }

        FPolygonGroupID PolygonGroupID = MeshDesc.CreatePolygonGroup();
        int32 TriangleCount = 0;

        // Create triangles
        for (int32 i = 0; i < LOD.Indices.Num(); i += 3)
        {
            if (i + 2 >= LOD.Indices.Num()) continue;

            TArray<FVertexInstanceID> VertexInstanceIDs;
            bool bValidTriangle = true;

            for (int32 j = 2; j >= 0; --j)
            {
                int32 Index = LOD.Indices[i + j];
                int32 UVIndex = (i + j < LOD.UVIndices.Num()) ? LOD.UVIndices[i + j] : Index;

                if (Index < 0 || Index >= LOD.Vertices.Num())
                {
                    bValidTriangle = false;
                    break;
                }

                FVertexID VertexID = VertexIDs[Index];
                FVertexInstanceID InstanceID = MeshDesc.CreateVertexInstance(VertexID);

                // Use smoothed normal combined with original normal if available
                FVector FinalNormal = SmoothedNormals[Index];
                if (Index < LOD.Normals.Num())
                {
                    FinalNormal = (SmoothedNormals[Index] + LOD.Normals[Index]).GetSafeNormal();
                }

                // Transform normal
                FVector TransformedNormal = Mesh.Transform.TransformVector(FinalNormal).GetSafeNormal();

                // Set normal
                MeshDesc.VertexInstanceAttributes().GetAttributesRef<FVector3f>(
                    MeshAttribute::VertexInstance::Normal)[InstanceID] =
                    FVector3f(TransformedNormal.X, TransformedNormal.Y, TransformedNormal.Z);

                // Calculate tangent basis
                FVector TangentX, TangentY;
                if (UVIndex >= 0 && UVIndex < LOD.UVs.Num())
                {
                    TangentX = FVector(1.0f, 0.0f, 0.0f);
                    TangentY = FVector::CrossProduct(TransformedNormal, TangentX);
                    TangentX = FVector::CrossProduct(TangentY, TransformedNormal);
                    TangentX.Normalize();
                }
                else
                {
                    TransformedNormal.FindBestAxisVectors(TangentX, TangentY);
                }

                // Set tangent
                MeshDesc.VertexInstanceAttributes().GetAttributesRef<FVector3f>(
                    MeshAttribute::VertexInstance::Tangent)[InstanceID] =
                    FVector3f(TangentX.X, TangentX.Y, TangentX.Z);

                // Set UV
                if (UVIndex >= 0 && UVIndex < LOD.UVs.Num())
                {
                    MeshDesc.VertexInstanceAttributes().GetAttributesRef<FVector2f>(
                        MeshAttribute::VertexInstance::TextureCoordinate)[InstanceID] =
                        FVector2f(LOD.UVs[UVIndex].X, LOD.UVs[UVIndex].Y);
                }

                VertexInstanceIDs.Add(InstanceID);
            }

            if (bValidTriangle && VertexInstanceIDs.Num() == 3)
            {
                MeshDesc.CreatePolygon(PolygonGroupID, VertexInstanceIDs);
                TriangleCount++;
            }
        }

        // Setup source model
        FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();

        // Configure build settings
        FMeshBuildSettings& BuildSettings = SrcModel.BuildSettings;
        BuildSettings.bRecomputeNormals = true;  // Use computed normals
        BuildSettings.bRecomputeTangents = true;
        BuildSettings.bUseMikkTSpace = true;
        BuildSettings.bRemoveDegenerates = true;
        BuildSettings.bUseFullPrecisionUVs = true;
        BuildSettings.bGenerateLightmapUVs = true;
        BuildSettings.MinLightmapResolution = 128;  // Increased resolution
        BuildSettings.SrcLightmapIndex = 0;
        BuildSettings.DstLightmapIndex = 1;

        // Initialize mesh description
        StaticMesh->CreateMeshDescription(LOD.LODIndex, MoveTemp(MeshDesc));
        StaticMesh->CommitMeshDescription(LOD.LODIndex);
    }

    // Configure static mesh properties
    StaticMesh->SetLightingGuid();
    StaticMesh->SetLightMapResolution(128);
    StaticMesh->SetLightMapCoordinateIndex(1);

    // Add default material
    StaticMesh->GetStaticMaterials().Add(FStaticMaterial(UMaterial::GetDefaultMaterial(MD_Surface)));

    // Setup collision
    StaticMesh->CreateBodySetup();
    UBodySetup* BodySetup = StaticMesh->GetBodySetup();
    BodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;

    // Build the mesh
    StaticMesh->Build();
    StaticMesh->PostEditChange();

    return true;
}

bool Fformat3dModule::ImportCustom3DFile(const FString& FilePath, UStaticMesh* StaticMesh, FString& ErrorMessage)
{
    TArray<FString> Lines;
    if (!FFileHelper::LoadFileToStringArray(Lines, *FilePath))
    {
        ErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Starting to parse C3D file with %d lines"), Lines.Num());

    if (Lines.Num() == 0 || !Lines[0].StartsWith(TEXT("C3D_FILE_FORMAT")))
    {
        ErrorMessage = TEXT("Invalid file format: Missing C3D_FILE_FORMAT header");
        return false;
    }

    int32 CurrentLine = 1;
    if (CurrentLine < Lines.Num() && Lines[CurrentLine].StartsWith(TEXT("VERSION")))
    {
        CurrentLine++;
    }

    while (CurrentLine < Lines.Num())
    {
        const FString& Line = Lines[CurrentLine];

        if (Line.StartsWith(TEXT("NODE")))
        {
            UE_LOG(LogTemp, Log, TEXT("Found NODE at line %d: %s"), CurrentLine, *Line);

            FC3DNode Node;
            if (ParseNode(Lines, CurrentLine, Node))
            {
                if (Node.Mesh.IsSet())
                {
                    UE_LOG(LogTemp, Log, TEXT("Node %s has mesh, attempting to create"), *Node.Name);
                    const FC3DMesh& Mesh = Node.Mesh.GetValue();

                    if (Mesh.LODs.Num() > 0 && Mesh.LODs[0].Vertices.Num() > 0 && Mesh.LODs[0].Indices.Num() > 0)
                    {
                        if (CreateStaticMesh(Mesh, StaticMesh))
                        {
                            UE_LOG(LogTemp, Log, TEXT("Successfully created mesh for node %s"), *Node.Name);
                            return true;
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Failed to create mesh for node %s"), *Node.Name);
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Invalid mesh data in node %s"), *Node.Name);
                    }
                }

                // Check children nodes for meshes
                for (const FC3DNode& Child : Node.Children)
                {
                    if (Child.Mesh.IsSet())
                    {
                        const FC3DMesh& Mesh = Child.Mesh.GetValue();
                        if (Mesh.LODs.Num() > 0 && Mesh.LODs[0].Vertices.Num() > 0 && Mesh.LODs[0].Indices.Num() > 0)
                        {
                            if (CreateStaticMesh(Mesh, StaticMesh))
                            {
                                UE_LOG(LogTemp, Log, TEXT("Successfully created mesh for child node %s"), *Child.Name);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        CurrentLine++;
    }

    UE_LOG(LogTemp, Error, TEXT("No valid meshes found in the file"));
    ErrorMessage = TEXT("No valid meshes found in file");
    return false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(Fformat3dModule, format3d)