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
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_format3d();
// End Cross Module References
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
		{ "IncludePath", "format3d.h" },
		{ "ModuleRelativePath", "Public/format3d.h" },
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
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UCustom3DFactory, UCustom3DFactory::StaticClass, TEXT("UCustom3DFactory"), &Z_Registration_Info_UClass_UCustom3DFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCustom3DFactory), 628369213U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_1697003261(TEXT("/Script/format3d"),
		Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Assignments_VB_CS4085_Assignment_ue5_plugin_plugin_scene_Plugins_format3d_Source_format3d_Public_format3d_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
