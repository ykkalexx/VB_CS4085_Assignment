// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "format3d/Public/format3d.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeformat3d() {}
// Cross Module References
	FORMAT3D_API UClass* Z_Construct_UClass_UCustom3DFactory();
	FORMAT3D_API UClass* Z_Construct_UClass_UCustom3DFactory_NoRegister();
	FORMAT3D_API UScriptStruct* Z_Construct_UScriptStruct_FC3DMaterial();
	FORMAT3D_API UScriptStruct* Z_Construct_UScriptStruct_FC3DMesh();
	FORMAT3D_API UScriptStruct* Z_Construct_UScriptStruct_FC3DMeshLOD();
	FORMAT3D_API UScriptStruct* Z_Construct_UScriptStruct_FC3DNode();
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_format3d();
// End Cross Module References
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_C3DMaterial;
class UScriptStruct* FC3DMaterial::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_C3DMaterial.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_C3DMaterial.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FC3DMaterial, (UObject*)Z_Construct_UPackage__Script_format3d(), TEXT("C3DMaterial"));
	}
	return Z_Registration_Info_UScriptStruct_C3DMaterial.OuterSingleton;
}
template<> FORMAT3D_API UScriptStruct* StaticStruct<FC3DMaterial>()
{
	return FC3DMaterial::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FC3DMaterial_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FC3DMaterial_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "// Material structure\n" },
		{ "ModuleRelativePath", "Public/format3d.h" },
		{ "ToolTip", "Material structure" },
	};
#endif
	void* Z_Construct_UScriptStruct_FC3DMaterial_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FC3DMaterial>();
	}
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FC3DMaterial_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_format3d,
		nullptr,
		&NewStructOps,
		"C3DMaterial",
		sizeof(FC3DMaterial),
		alignof(FC3DMaterial),
		nullptr,
		0,
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FC3DMaterial_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FC3DMaterial_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FC3DMaterial()
	{
		if (!Z_Registration_Info_UScriptStruct_C3DMaterial.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_C3DMaterial.InnerSingleton, Z_Construct_UScriptStruct_FC3DMaterial_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_C3DMaterial.InnerSingleton;
	}
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_C3DMeshLOD;
class UScriptStruct* FC3DMeshLOD::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_C3DMeshLOD.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_C3DMeshLOD.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FC3DMeshLOD, (UObject*)Z_Construct_UPackage__Script_format3d(), TEXT("C3DMeshLOD"));
	}
	return Z_Registration_Info_UScriptStruct_C3DMeshLOD.OuterSingleton;
}
template<> FORMAT3D_API UScriptStruct* StaticStruct<FC3DMeshLOD>()
{
	return FC3DMeshLOD::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FC3DMeshLOD_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "// LOD Data structure\n" },
		{ "ModuleRelativePath", "Public/format3d.h" },
		{ "ToolTip", "LOD Data structure" },
	};
#endif
	void* Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FC3DMeshLOD>();
	}
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_format3d,
		nullptr,
		&NewStructOps,
		"C3DMeshLOD",
		sizeof(FC3DMeshLOD),
		alignof(FC3DMeshLOD),
		nullptr,
		0,
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FC3DMeshLOD()
	{
		if (!Z_Registration_Info_UScriptStruct_C3DMeshLOD.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_C3DMeshLOD.InnerSingleton, Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_C3DMeshLOD.InnerSingleton;
	}
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_C3DMesh;
class UScriptStruct* FC3DMesh::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_C3DMesh.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_C3DMesh.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FC3DMesh, (UObject*)Z_Construct_UPackage__Script_format3d(), TEXT("C3DMesh"));
	}
	return Z_Registration_Info_UScriptStruct_C3DMesh.OuterSingleton;
}
template<> FORMAT3D_API UScriptStruct* StaticStruct<FC3DMesh>()
{
	return FC3DMesh::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FC3DMesh_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FC3DMesh_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "// Mesh structure\n" },
		{ "ModuleRelativePath", "Public/format3d.h" },
		{ "ToolTip", "Mesh structure" },
	};
#endif
	void* Z_Construct_UScriptStruct_FC3DMesh_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FC3DMesh>();
	}
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FC3DMesh_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_format3d,
		nullptr,
		&NewStructOps,
		"C3DMesh",
		sizeof(FC3DMesh),
		alignof(FC3DMesh),
		nullptr,
		0,
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FC3DMesh_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FC3DMesh_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FC3DMesh()
	{
		if (!Z_Registration_Info_UScriptStruct_C3DMesh.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_C3DMesh.InnerSingleton, Z_Construct_UScriptStruct_FC3DMesh_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_C3DMesh.InnerSingleton;
	}
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_C3DNode;
class UScriptStruct* FC3DNode::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_C3DNode.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_C3DNode.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FC3DNode, (UObject*)Z_Construct_UPackage__Script_format3d(), TEXT("C3DNode"));
	}
	return Z_Registration_Info_UScriptStruct_C3DNode.OuterSingleton;
}
template<> FORMAT3D_API UScriptStruct* StaticStruct<FC3DNode>()
{
	return FC3DNode::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FC3DNode_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FC3DNode_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "// Node structure\n" },
		{ "ModuleRelativePath", "Public/format3d.h" },
		{ "ToolTip", "Node structure" },
	};
#endif
	void* Z_Construct_UScriptStruct_FC3DNode_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FC3DNode>();
	}
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FC3DNode_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_format3d,
		nullptr,
		&NewStructOps,
		"C3DNode",
		sizeof(FC3DNode),
		alignof(FC3DNode),
		nullptr,
		0,
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FC3DNode_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FC3DNode_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FC3DNode()
	{
		if (!Z_Registration_Info_UScriptStruct_C3DNode.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_C3DNode.InnerSingleton, Z_Construct_UScriptStruct_FC3DNode_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_C3DNode.InnerSingleton;
	}
	void UCustom3DFactory::StaticRegisterNativesUCustom3DFactory()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCustom3DFactory);
	UClass* Z_Construct_UClass_UCustom3DFactory_NoRegister()
	{
		return UCustom3DFactory::StaticClass();
	}
	struct Z_Construct_UClass_UCustom3DFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UCustom3DFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_format3d,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UCustom3DFactory_Statics::Class_MetaDataParams[] = {
		{ "Comment", "// Factory class\n" },
		{ "IncludePath", "format3d.h" },
		{ "ModuleRelativePath", "Public/format3d.h" },
		{ "ToolTip", "Factory class" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UCustom3DFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCustom3DFactory>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UCustom3DFactory_Statics::ClassParams = {
		&UCustom3DFactory::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UCustom3DFactory_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UCustom3DFactory_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UCustom3DFactory()
	{
		if (!Z_Registration_Info_UClass_UCustom3DFactory.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCustom3DFactory.OuterSingleton, Z_Construct_UClass_UCustom3DFactory_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UCustom3DFactory.OuterSingleton;
	}
	template<> FORMAT3D_API UClass* StaticClass<UCustom3DFactory>()
	{
		return UCustom3DFactory::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UCustom3DFactory);
	UCustom3DFactory::~UCustom3DFactory() {}
	struct Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics
	{
		static const FStructRegisterCompiledInInfo ScriptStructInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FStructRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ScriptStructInfo[] = {
		{ FC3DMaterial::StaticStruct, Z_Construct_UScriptStruct_FC3DMaterial_Statics::NewStructOps, TEXT("C3DMaterial"), &Z_Registration_Info_UScriptStruct_C3DMaterial, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FC3DMaterial), 3464006033U) },
		{ FC3DMeshLOD::StaticStruct, Z_Construct_UScriptStruct_FC3DMeshLOD_Statics::NewStructOps, TEXT("C3DMeshLOD"), &Z_Registration_Info_UScriptStruct_C3DMeshLOD, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FC3DMeshLOD), 1682050113U) },
		{ FC3DMesh::StaticStruct, Z_Construct_UScriptStruct_FC3DMesh_Statics::NewStructOps, TEXT("C3DMesh"), &Z_Registration_Info_UScriptStruct_C3DMesh, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FC3DMesh), 283338206U) },
		{ FC3DNode::StaticStruct, Z_Construct_UScriptStruct_FC3DNode_Statics::NewStructOps, TEXT("C3DNode"), &Z_Registration_Info_UScriptStruct_C3DNode, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FC3DNode), 1659452646U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UCustom3DFactory, UCustom3DFactory::StaticClass, TEXT("UCustom3DFactory"), &Z_Registration_Info_UClass_UCustom3DFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCustom3DFactory), 516352219U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_256634100(TEXT("/Script/format3d"),
		Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ClassInfo),
		Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ScriptStructInfo),
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
