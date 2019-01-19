//================================================================================
//!	@file	 FbxConverter.cpp
//!	@brief	 FBX�ϊ���Class
//! @details 
//!	@author  Kai Araki									@date 2018/12/29
//================================================================================


//****************************************
// �C���N���[�h��
//****************************************
#include <iostream>
#include <fstream>
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#include <shlwapi.h>

#include "FbxConverter.h"



//****************************************
// �֐���`
//****************************************
bool FbxConverter::Init()
{
	// �}�l�[�W���̐���
	manager_ = FbxManager::Create();
	if (!manager_)
	{
		std::cout << "���}�l�[�W���̍쐬�Ɏ��s" << std::endl;
		return false;
	}

	// IOSettings�I�u�W�F�N�g(�SInport/Export�̐ݒ��ێ�����)�̍쐬
	io_settings_ = FbxIOSettings::Create(manager_, IOSROOT);
	if (!io_settings_)
	{
		std::cout << "��FbxIOSettings�̍쐬�Ɏ��s" << std::endl;
		return false;
	}
	manager_->SetIOSettings(io_settings_);

	// ���s�\�ȃf�B���N�g��(�I�v�V����)����v���O�C�������[�h����
	std::string temp = FbxGetApplicationDirectory();
	ConvertToFullPathOfUTF8(&temp);
	FbxString plugins_directory_path = (FbxString)temp.c_str();
	manager_->LoadPluginsDirectory(plugins_directory_path.Buffer());

	// �V�[���̍쐬
	scene_ = FbxScene::Create(manager_, "scene");
	if (!scene_)
	{
		std::cout << "���V�[���̍쐬�Ɏ��s" << std::endl;
		return false;
	}

	return true;
}



void FbxConverter::Uninit()
{
	// �}�l�[�W�����(�֘A����S�I�u�W�F�N�g����������)
	manager_->Destroy();

	std::cout << "\n��FBX�̕ϊ��I��...";
}



bool FbxConverter::ConvertToMdBin(std::string* file_path)
{
	// FBX�̓ǂݍ���
	if (!Load(file_path))
	{
		std::cout << "��FBX�̓ǂݍ��ݎ��s" << std::endl;
		return false;
	}

	// FBX����f�[�^�[�𒊏o
	if (!ExtractData())
	{
		std::cout << "���f�[�^�̒��o�Ɏ��s" << std::endl;
		return false;
	}

	// MdBin�t�@�C���̏o��
	if (!ExportOfMdBinFile(file_path))
	{
		std::cout << "��MdBin�t�@�C���̏o�͂Ɏ��s" << std::endl;
		return false;
	}

	return true;
}



bool FbxConverter::Load(std::string* file_path)
{
	std::cout << "[Load]\n" << std::endl;

	// �C���|�[�^�\�̍쐬
	FbxImporter* importer = FbxImporter::Create(manager_, "");
	if (!importer)
	{
		std::cout << "���C���|�[�^�\�̍쐬�Ɏ��s" << std::endl;
		return false;
	}

	// �C���|�[�^�\�̏�����
	std::string temp_file_path = *file_path;
	ConvertToFullPathOfUTF8(&temp_file_path);
	if (!importer->Initialize(temp_file_path.c_str(), -1, manager_->GetIOSettings()))
	{
		std::cout << "���C���|�[�^�\�̏������Ɏ��s" << std::endl;
		return false;
	}

	// �V�[���̗�������
	importer->Import(scene_);
	std::cout << "���V�[�����쐬" << std::endl;

	// �t���[����񒊏o
	ExtractFrameData(importer);

	// �C���|�[�^�[���
	importer->Destroy();

	// ���W�n�̕ϊ�
	std::string temp;
	std::cout << "\n���ǂ���̍��W�n�ɂ��܂����H DirectX[y], OpenGL[n]" << std::endl;
	do
	{
		std::cout << "��";
		std::cin >> temp;
	} while (temp != "y" && temp != "n");

	axis_system_ = temp == "y" ? FbxAxisSystem::DirectX : FbxAxisSystem::OpenGL;

	if (scene_->GetGlobalSettings().GetAxisSystem() != axis_system_)
	{
		std::cout << "�����f�[�^���w����W�n�ƈقȂ�וϊ�\n��";

		if (axis_system_ == FbxAxisSystem::DirectX)
		{
			FbxAxisSystem::DirectX.ConvertScene(scene_);
			std::cout << "����n���W�֕ϊ�����" << std::endl;
		}
		else
		{
			FbxAxisSystem::OpenGL.ConvertScene(scene_);
			std::cout << "�E��n���W�֕ϊ�����" << std::endl;
		}
	}

	// ���l���]�̊m�F
	std::cout << "\n�����l�𔽓]���܂����H Yes[y], No[n]" << std::endl;
	do
	{
		std::cout << "��";
		std::cin >> temp;
	} while (temp != "y" && temp != "n");

	is_reverse_alpha_ = temp == "y";

	// �X�P�[�����p�P�ʂ֕ϊ�
	FbxSystemUnit system_unit = scene_->GetGlobalSettings().GetSystemUnit();
	if (system_unit.GetScaleFactor() != 1.0f)
	{
		FbxSystemUnit::cm.ConvertScene(scene_);
		std::cout << "\n���X�P�[�����p�P�ʂ֕ϊ�" << std::endl;
	}

	// �S�|���S�����O�p�`�|���S���֕ϊ�
	FbxGeometryConverter geometry_converter(manager_);
	if (geometry_converter.Triangulate(scene_, true))
	{
		std::cout << "\n���S�|���S�����O�p�`�|���S���֕ϊ�" << std::endl;
		geometry_converter.RemoveBadPolygonsFromMeshes(scene_);
	}

	// �}�e���A�����ƂɃ��b�V���𕪊�
	if (geometry_converter.SplitMeshesPerMaterial(scene_, true))
	{
		std::cout << "\n���}�e���A�����ƂɃ��b�V���𕪊�" << std::endl;
	}

	// ���[�g�m�[�h�̎擾
	root_node_ = scene_->GetRootNode();
	if (!root_node_)
	{
		std::cout << "\n�����[�g�m�[�h�̎擾�Ɏ��s" << std::endl;
		return false;
	}

	std::cout << "\n************************************************************" << std::endl;

	return true;
}



bool FbxConverter::ExtractData()
{
	// �}�e���A���f�[�^���o
	ExtractMaterialData();

	// ���b�V���f�[�^���o
	ExtractMeshData();

	return true;
}



void FbxConverter::ExtractMaterialData()
{
	// �}�e���A�����擾
	int material_num = scene_->GetMaterialCount();
	md_bin_data_container_.setMaterialArraySize(material_num);

	// �}�e���A�����Ƃ̏���
	for (int i = 0; i < material_num; i++)
	{
		// �}�e���A���擾
		FbxSurfaceMaterial* material = scene_->GetMaterial(i);

		// �e��}�e���A����񒊏o
		ExtractMaterialName(i, material);
		ExtractAmbientData(i, material);
		ExtractDiffuseData(i, material);
		ExtractEmissiveData(i, material);
		ExtractBumpData(i, material);
		ExtractTransparentData(i, material);
		ExtractSpecularData(i, material);
		ExtractPowerData(i, material);
		ExtractReflectionData(i, material);
	}
}



void FbxConverter::ExtractMeshData()
{
	// ���b�V�����擾
	int mesh_num = scene_->GetSrcObjectCount<FbxMesh>();
	md_bin_data_container_.setMeshArraySize(mesh_num);

	// �C���f�b�N�X�̕��тŊe�f�[�^�𒊏o(����UV�̊֌W��C���f�b�N�X�̍œK���͍s��Ȃ�)
	//for (int i = 0; i < 1; i++)
	for (int i = 0; i < mesh_num; i++)
	{
		// ���b�V���擾
		FbxMesh* mesh = scene_->GetSrcObject<FbxMesh>(i);

		// �e�탁�b�V����񒊏o
		ExtractIndexData(i, mesh);
		ExtractVertexData(i, mesh);
		ExtractNormalData(i, mesh);
		ExtractUVSetData(i, mesh);
		ExtractAnimationData(i, mesh);

		// �}�e���A���f�[�^�Ƃ̊֘A�t��
		AssociateWithMaterialData(i, mesh);

		// UV�Z�b�g�f�[�^�ƃe�N�X�`���̊֘A�t��
		AssociatingUVSetDataAndTexture(i);
	}
}



void FbxConverter::ExtractMaterialName(int material_index, FbxSurfaceMaterial* material)
{
	*md_bin_data_container_.getpMaterial(material_index)->getpName() = material->GetName();
}



void FbxConverter::ExtractAmbientData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (property.IsValid())
	{
		FbxDouble3 ambient = property.Get<FbxDouble3>();
		*md_bin_data_container_.getpMaterial(material_index)->getpAmbient()->getpR() = (float)ambient[0];
		*md_bin_data_container_.getpMaterial(material_index)->getpAmbient()->getpG() = (float)ambient[1];
		*md_bin_data_container_.getpMaterial(material_index)->getpAmbient()->getpB() = (float)ambient[2];

		ExtractTextureData(material_index, property,
						   MdBinDataContainer::Material::Texture::Type::AMBIENT);
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpAmbient()->getpR() = 1.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpAmbient()->getpG() = 1.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpAmbient()->getpB() = 1.0f;
	}
}



void FbxConverter::ExtractDiffuseData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (property.IsValid())
	{
		FbxDouble3 diffuse = property.Get<FbxDouble3>();
		*md_bin_data_container_.getpMaterial(material_index)->getpDiffuse()->getpR() = (float)diffuse[0];
		*md_bin_data_container_.getpMaterial(material_index)->getpDiffuse()->getpG() = (float)diffuse[1];
		*md_bin_data_container_.getpMaterial(material_index)->getpDiffuse()->getpB() = (float)diffuse[2];

		ExtractTextureData(material_index, property,
						   MdBinDataContainer::Material::Texture::Type::DIFFUSE);
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpDiffuse()->getpR() = 1.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpDiffuse()->getpG() = 1.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpDiffuse()->getpB() = 1.0f;
	}
}



void FbxConverter::ExtractEmissiveData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sEmissive);
	if (property.IsValid())
	{
		FbxDouble3 emissive = property.Get<FbxDouble3>();
		*md_bin_data_container_.getpMaterial(material_index)->getpEmissive()->getpR() = (float)emissive[0];
		*md_bin_data_container_.getpMaterial(material_index)->getpEmissive()->getpG() = (float)emissive[1];
		*md_bin_data_container_.getpMaterial(material_index)->getpEmissive()->getpB() = (float)emissive[2];

		ExtractTextureData(material_index, property,
						   MdBinDataContainer::Material::Texture::Type::EMISSIVE);
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpEmissive()->getpR() = 0.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpEmissive()->getpG() = 0.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpEmissive()->getpB() = 0.0f;
	}
}



void FbxConverter::ExtractBumpData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sBump);
	if (property.IsValid())
	{
		FbxDouble3 bump = property.Get<FbxDouble3>();
		*md_bin_data_container_.getpMaterial(material_index)->getpBump()->getpR() = (float)bump[0];
		*md_bin_data_container_.getpMaterial(material_index)->getpBump()->getpG() = (float)bump[1];
		*md_bin_data_container_.getpMaterial(material_index)->getpBump()->getpB() = (float)bump[2];

		ExtractTextureData(material_index, property,
						   MdBinDataContainer::Material::Texture::Type::NORMAL);
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpBump()->getpR() = 0.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpBump()->getpG() = 0.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpBump()->getpB() = 0.0f;
	}
}



void FbxConverter::ExtractTransparentData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (property.IsValid())
	{
		FbxDouble transparent = property.Get<FbxDouble>();
		if (is_reverse_alpha_)
		{
			// Blender�Ŋm�F����Ɖ��̂������l���t�ɂȂ��
			*md_bin_data_container_.getpMaterial(material_index)->getpTransparent() = 1.0f - (float)transparent;
		}
		else
		{
			*md_bin_data_container_.getpMaterial(material_index)->getpTransparent() = (float)transparent;
		}
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpTransparent() = 1.0f;
	}
}



void FbxConverter::ExtractSpecularData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sSpecular);
	if (property.IsValid())
	{
		FbxDouble3 specular = property.Get<FbxDouble3>();
		*md_bin_data_container_.getpMaterial(material_index)->getpSpecular()->getpR() = (float)specular[0];
		*md_bin_data_container_.getpMaterial(material_index)->getpSpecular()->getpG() = (float)specular[1];
		*md_bin_data_container_.getpMaterial(material_index)->getpSpecular()->getpB() = (float)specular[2];

		ExtractTextureData(material_index, property,
						   MdBinDataContainer::Material::Texture::Type::SPECULAR);
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpSpecular()->getpR() = 0.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpSpecular()->getpG() = 0.0f;
		*md_bin_data_container_.getpMaterial(material_index)->getpSpecular()->getpB() = 0.0f;
	}
}



void FbxConverter::ExtractPowerData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sShininess);
	if (property.IsValid())
	{
		FbxDouble3 specular = property.Get<FbxDouble3>();
		*md_bin_data_container_.getpMaterial(material_index)->getpPower() = (float)specular[0];
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpPower() = 0.0f;
	}
}



void FbxConverter::ExtractReflectionData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
	if (property.IsValid())
	{
		FbxDouble reflection = property.Get<FbxDouble>();
		*md_bin_data_container_.getpMaterial(material_index)->getpReflection() = (float)reflection;
	}
	else
	{
		*md_bin_data_container_.getpMaterial(material_index)->getpReflection() = 0.0f;
	}
}



void FbxConverter::ExtractTextureData(int material_index, FbxProperty property,
									  MdBinDataContainer::Material::Texture::Type texture_type)
{
	// �e�N�X�`�����擾
	int texture_num = property.GetSrcObjectCount<FbxFileTexture>();
	if (texture_num > 0)
	{
		// �e�N�X�`�����쐬
		MdBinDataContainer::Material::Texture* texture = new MdBinDataContainer::Material::Texture();
		FbxFileTexture* fbx_texture = property.GetSrcObject<FbxFileTexture>(0);
		*texture->getpFilePath() = fbx_texture->GetRelativeFileName();
		ExtractFileName(texture->getpFilePath(), texture->getpFilePath());
		*texture->getpType() = texture_type;

		// �e�N�X�`����ǉ�
		material_index = material_index;
		md_bin_data_container_.getpMaterial(material_index)->AddTextureArray(texture);
	}
}



void FbxConverter::ExtractFrameData(FbxImporter* importer)
{
	// 1�t���[���̎��Ԃ��擾
	FbxTime::EMode time_mode = scene_->GetGlobalSettings().GetTimeMode();
	period_.SetTime(0, 0, 0, 1, 0, time_mode);

	int animation_stack_count = importer->GetAnimStackCount();
	std::cout << "\n���e�C�N���F" << animation_stack_count << std::endl;

	bool is_animation = false;
	for (int i = 0; i < animation_stack_count; i++)
	{
		FbxTakeInfo* take_info = importer->GetTakeInfo(i);
		if (!take_info) continue;	// �f�t�H���g�e�C�N�Ȃ疳��

		// �A�j���[�V�����t���[�������擾
		FbxTime animation_start_time = take_info->mLocalTimeSpan.GetStart();
		FbxTime animation_stop_time = take_info->mLocalTimeSpan.GetStop();
		animation_start_frame_num_ = (int)(animation_start_time.Get() / period_.Get());
		animation_stop_frame_num_ = (int)(animation_stop_time.Get() / period_.Get());
		all_animation_frame_num_ = animation_stop_frame_num_ - animation_start_frame_num_ + 1;

		is_animation = true;
		break;
	}
	if (is_animation)
	{
		std::cout << "�˃A�j���[�V�������F�L" << std::endl;
		std::cout << "��START_FRAME_NUM�F" << animation_start_frame_num_ << std::endl;
		std::cout << "��STOP_FRAME_NUM�F" << animation_stop_frame_num_ << std::endl;
		std::cout << "��ALL_FRAME_NUM�F" << all_animation_frame_num_ << std::endl;
	}
	else
	{
		std::cout << "�˃A�j���[�V�������F��" << std::endl;
	}
}



void FbxConverter::ExtractIndexData(int mesh_index, FbxMesh* mesh)
{
	// �C���f�b�N�X���擾
	int index_num = mesh->GetPolygonVertexCount();	// �C���f�b�N�X��
	md_bin_data_container_.getpMesh(mesh_index)->setIndexArraySize(index_num);

	// �C���f�b�N�X�擾
	int* index_array = nullptr;
	index_array = mesh->GetPolygonVertices();	// �C���f�b�N�X�z��
	for (int i = 0; i < index_num; i += 3)
	{
		*md_bin_data_container_.getpMesh(mesh_index)->getpIndex(i) = index_array[i];
		*md_bin_data_container_.getpMesh(mesh_index)->getpIndex(i + 1) = index_array[i + 1];
		*md_bin_data_container_.getpMesh(mesh_index)->getpIndex(i + 2) = index_array[i + 2];
	}
}



void FbxConverter::ExtractVertexData(int mesh_index, FbxMesh* mesh)
{
	// ���_�����C���f�b�N�X�����m��
	md_bin_data_container_.getpMesh(mesh_index)->setPositionArraySize((md_bin_data_container_.getpMesh(mesh_index)->getIndexArraySize()));

	// ���_�擾
	FbxVector4* vertex_array = mesh->GetControlPoints();	// ���_�z��
	for (int i = 0; i < md_bin_data_container_.getpMesh(mesh_index)->getPositionArraySize(); i++)
	{
		FbxVector4 vertex = vertex_array[*md_bin_data_container_.getpMesh(mesh_index)->getpIndex(i)];
		*md_bin_data_container_.getpMesh(mesh_index)->getpPosition(i)->getpX() = (float)vertex[0];
		*md_bin_data_container_.getpMesh(mesh_index)->getpPosition(i)->getpY() = (float)vertex[1];
		*md_bin_data_container_.getpMesh(mesh_index)->getpPosition(i)->getpZ() = (float)vertex[2];
	}
}



void FbxConverter::ExtractNormalData(int mesh_index, FbxMesh* mesh)
{
	// �@���z��(�C���f�b�N�X�Ɠ�������)
	FbxArray<FbxVector4> normal_array;
	mesh->GetPolygonVertexNormals(normal_array);

	// �@�������C���f�b�N�X�����m��
	md_bin_data_container_.getpMesh(mesh_index)->setNormalArraySize(normal_array.Size());

	// �@���擾
	for (int i = 0; i < normal_array.Size(); i++)
	{
		*md_bin_data_container_.getpMesh(mesh_index)->getpNormal(i)->getpX() = (float)normal_array[i][0];
		*md_bin_data_container_.getpMesh(mesh_index)->getpNormal(i)->getpY() = (float)normal_array[i][1];
		*md_bin_data_container_.getpMesh(mesh_index)->getpNormal(i)->getpZ() = (float)normal_array[i][2];
	}
}



void FbxConverter::ExtractUVSetData(int mesh_index, FbxMesh* mesh)
{
	// UV�Z�b�g���z��擾
	FbxStringList uv_set_name_array;
	mesh->GetUVSetNames(uv_set_name_array);
	md_bin_data_container_.getpMesh(mesh_index)->setUVSetArraySize(uv_set_name_array.GetCount());

	for (int i = 0; i < uv_set_name_array.GetCount(); i++)
	{
		// UV�z��(�C���f�b�N�X�Ɠ�������)
		FbxArray<FbxVector2> uv_set_array;
		mesh->GetPolygonVertexUVs(uv_set_name_array.GetStringAt(i), uv_set_array);

		// UV�����C���f�b�N�X�����m��
		md_bin_data_container_.getpMesh(mesh_index)->getpUVSet(i)->setUVArraySize(uv_set_array.Size());

		// UV�擾
		for (int j = 0; j < md_bin_data_container_.getpMesh(mesh_index)->getpUVSet(i)->getUVArraySize(); j++)
		{
			*md_bin_data_container_.getpMesh(mesh_index)->getpUVSet(i)->getpU(j) = (float)uv_set_array[j][0];
			if (axis_system_ == FbxAxisSystem::DirectX)
			{
				*md_bin_data_container_.getpMesh(mesh_index)->getpUVSet(i)->getpV(j) = 1.0f - (float)uv_set_array[j][1];
			}
			else
			{
				*md_bin_data_container_.getpMesh(mesh_index)->getpUVSet(i)->getpV(j) = (float)uv_set_array[j][1];
			}
		}
	}
}



void FbxConverter::ExtractAnimationData(int mesh_index, FbxMesh* mesh)
{
	// �{�[���̏d�ݕۑ��p�z��𒸓_�����m��
	std::vector<MdBinDataContainer::Mesh::BoneWeight> save_bone_weight_array;
	int save_bone_weight_num = mesh->GetControlPointsCount();
	save_bone_weight_array.resize(save_bone_weight_num);
	for (int i = 0; i < save_bone_weight_num; i++)
	{
		save_bone_weight_array[i].Init();
	}

	// �X�L�����̎擾(�X�L��1�̂ݎ擾)
	int skin_num = mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < skin_num; i++)
	{
		FbxSkin* skin = (FbxSkin*)(mesh->GetDeformer(i, FbxDeformer::eSkin));

		// �N���X�^(�{�[��1�{���̏��̉�)����{�[���f�[�^�𒊏o
		int cluster_num = skin->GetClusterCount();
		for (int j = 0; j < cluster_num; j++)
		{
			FbxCluster* cluster = skin->GetCluster(j);

			// �e��f�[�^���o
			ExtractBoneData(mesh_index, cluster);
			ExtractBoneWeightData(mesh_index, &save_bone_weight_array, cluster);
		}
		break;
	}

	// �{�[���̏d�݂𒸓_�֑��
	int bone_weight_num = md_bin_data_container_.getpMesh(mesh_index)->getIndexArraySize();
	md_bin_data_container_.getpMesh(mesh_index)->setBoneWeightArraySize(bone_weight_num);
	for (int i = 0; i < bone_weight_num; i++)
	{
		int index = *md_bin_data_container_.getpMesh(mesh_index)->getpIndex(i);
		*md_bin_data_container_.getpMesh(mesh_index)->getpBoneWeight(i) = save_bone_weight_array[index];
	}
}



void FbxConverter::ExtractBoneData(int mesh_index, FbxCluster* cluster)
{

	int affected_vertices_num = cluster->GetControlPointIndicesCount();
	if (affected_vertices_num <= 0) return;

	// �{�[�����擾
	MdBinDataContainer::Mesh::Bone bone;
	*bone.getpName() = cluster->GetName();

	// �I�t�Z�b�g�s��擾
	FbxAMatrix init_matrix;
	cluster->GetTransformLinkMatrix(init_matrix);
	FbxAMatrix offset_matrix = init_matrix.Inverse();
	ChangeMatrix(bone.getpOffsetMatrix(), &offset_matrix);

	// �A�j���[�V�����s��擾
	if (all_animation_frame_num_ != -1)
	{
		bone.setAnimationMatrixArraySize(all_animation_frame_num_);
		for (int i = 0; i < all_animation_frame_num_; i++)
		{
			FbxAMatrix animation_matrix;
			FbxTime time = (animation_start_frame_num_ + i) * period_.Get();
			animation_matrix = cluster->GetLink()->EvaluateGlobalTransform(time);
			ChangeMatrix(bone.getpAnimationMatrixArray(i), &animation_matrix);
		}
	}
	else
	{
		int default_array_num = 1;
		bone.setAnimationMatrixArraySize(default_array_num);
		FbxAMatrix default_matrix;
		default_matrix.SetIdentity();
		ChangeMatrix(bone.getpAnimationMatrixArray(0), &default_matrix);
	}
	
	// ���b�V���̃{�[���z��֒ǉ�
	md_bin_data_container_.getpMesh(mesh_index)->AddBoneArray(&bone);
}



void FbxConverter::ExtractBoneWeightData(int mesh_index,
										 std::vector<MdBinDataContainer::Mesh::BoneWeight>* save_bone_weight_array,
										 FbxCluster* cluster)
{
	// ���b�V���̃{�[���z��̖����̃C���f�b�N�X�擾
	int bone_index = md_bin_data_container_.getpMesh(mesh_index)->getBoneArrayEndIndex();

	// �e�����钸�_�z�񐔎擾
	int affected_vertices_num = cluster->GetControlPointIndicesCount();

	// �e�����钸�_�z��&�d�ݔz��擾
	int* affected_vertices_array = cluster->GetControlPointIndices();
	double* weight_array_ = cluster->GetControlPointWeights();

	// �{�[���̏d�݂��擾
	for (int i = 0; i < affected_vertices_num; i++)
	{
		int vertex_index = affected_vertices_array[i];
		float bone_weight = (float)weight_array_[i];

		save_bone_weight_array->at(vertex_index)
			.setBoneIndexAndWeight(bone_index,
								   bone_weight);
	}
}



void FbxConverter::ChangeMatrix(MdBinDataContainer::Matrix* bone_matrix,
								FbxAMatrix* fbx_matrix)
{
	for (int i = 0; i < MdBinDataContainer::Matrix::ARRAY_HEIGHT; i++)
	{
		for (int j = 0; j < MdBinDataContainer::Matrix::ARRAY_WIDTH; j++)
		{
			bone_matrix
				->setMatrixElement((float)fbx_matrix->Get(i, j), i, j);
		}
	}
}



void FbxConverter::AssociateWithMaterialData(int mesh_index, FbxMesh* mesh)
{
	// ���C���[�ƃ}�e���A�������֘A�t��
	int layer_num = mesh->GetLayerCount();
	if (layer_num > 0)
	{
		// ���b�V������������m�[�h���̃}�e���A���z��擾
		// ��{���C���[0�ԂɃ}�e���A���Ƃ̊֘A��񂪂���
		FbxLayerElementMaterial* material_array = mesh->GetLayer(0)->GetMaterials();

		// ���b�V���ɃA�^�b�`���Ă���}�e���A�����擾
		FbxLayerElementArrayTemplate<int>* material_index_array
			= &material_array->GetIndexArray();
		int material_index_array_num = material_index_array->GetCount();

		for (int i = 0; i < material_index_array_num; i++)
		{
			// �C���f�b�N�X�ԍ�����}�e���A���擾
			int index_num = material_index_array->GetAt(i);
			FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(index_num);

			// �}�e���A������p���ă}�e���A���z��Ɗ֘A�t��
			std::string material_name = material->GetName();
			for (int j = 0; j < md_bin_data_container_.getMaterialArraySize(); j++)
			{
				if (material_name
					!= *md_bin_data_container_.getpMaterial(j)->getpName()) continue;

				*md_bin_data_container_.getpMesh(mesh_index)->getpMaterialIndex()
					= j;
				break;
			}
		}

	}
	else
	{
		// �}�e���A���Ƃ̊֘A��񂪂Ȃ�
		*md_bin_data_container_.getpMesh(mesh_index)->getpMaterialIndex() = -1;
	}
}



void FbxConverter::AssociatingUVSetDataAndTexture(int mesh_index)
{
	// UVSet�����邩�ǂ���
	if (md_bin_data_container_.getpMesh(mesh_index)->getUVSetArraySize() <= 0) return;

	// �}�e���A���擾
	int material_index = *md_bin_data_container_.getpMesh(mesh_index)->getpMaterialIndex();
	MdBinDataContainer::Material* material = md_bin_data_container_.getpMaterial(material_index);

	// �}�e���A���̃e�N�X�`������f�B�q���[�Y�e�N�X�`����UV��0�ԖڂƊ֘A�t����
	for (int i = 0; i < (int)material->getTextureArraySize(); i++)
	{
		if (*material->getpTexture(i)->getpType()
			!= MdBinDataContainer::Material::Texture::Type::DIFFUSE) continue;

		md_bin_data_container_.getpMesh(mesh_index)->getpUVSet(0)
			->AddTextureArray(material->getpTexture(i));
	}
}



bool FbxConverter::ExportOfMdBinFile(std::string* file_path)
{
	std::cout << "\n[ExportOfMdBinFile]\n" << std::endl;

	// �t�@�C�����𒊏o&�g���q����
	std::string file_name;
	ExtractFileName(&file_name, file_path);
	ExtensionRemoval(&file_name);

	// �o�̓f�B���N�g���p�X�쐬
	std::string directory_path;
	CreateExportDirectoryPath(&directory_path, &file_name);

	// �o�̓t�H���_�쐬
	CreateExportDirectory(&directory_path);

	// �o�̓t�@�C�����쐬
	std::string txt_data_file_name;
	CreateExportFileName(&file_name, &txt_data_file_name);

	// �o�̓p�X�쐬
	std::string export_file_path;
	std::string export_txt_data_file_path;
	CreateExportPath(&export_file_path, &export_txt_data_file_path,
					 &file_name, &txt_data_file_name, &directory_path);

	// �o��
	std::ifstream ifstream(export_file_path.c_str(), std::ios::binary);
	if (ifstream)
	{
		ifstream.close();
		std::cout << "\n�����t�@�C�����w" << file_name << "�x������܂��B" << std::endl;
		std::cout << "�㏑�����܂����H Yes[y], No[n]" << std::endl;
		std::string temp;
		do
		{
			std::cout << "��";
			std::cin >> temp;
		} while (temp != "y" && temp != "n");

		if (temp == "n") return true;
	}
	MdBinDataContainer::ExportData(&md_bin_data_container_, export_file_path);
	ExportTextData(&export_txt_data_file_path);

	std::cout << "\n���L�̃t�@�C�����o�͂��܂����B\n" << std::endl;
	std::cout << "�w" << export_file_path << "�x" << std::endl;
	std::cout << "�w" << export_txt_data_file_path << "�x" << std::endl;

	return true;
}



void FbxConverter::ExtractFileName(std::string* export_file_name, std::string* file_path)
{
	// �t�@�C�����𒊏o(�p�X��؂�"\"��"/"�̗����ɑΉ�)
	auto temp_index = file_path->find_last_of("/");	// �Ō�̃p�X��؂�̃C���f�b�N�X���擾
	if (temp_index != std::string::npos)
	{
		// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
		*export_file_name = file_path->substr(temp_index + 1);
	}
	else
	{
		temp_index = file_path->find_last_of("\\");
		if (temp_index != std::string::npos)
		{
			// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
			*export_file_name = file_path->substr(temp_index + 1);
		}
		else
		{
			// �t�@�C�����𒊏o
			*export_file_name = *file_path;
		}
	}
}



void FbxConverter::ExtensionRemoval(std::string* file_name)
{
	auto temp_index = file_name->find_last_of(".");
	if (temp_index != std::string::npos)
	{
		// �t�@�C��������g���q������
		*file_name = file_name->substr(0, temp_index);
	}
}



void FbxConverter::CreateExportDirectory(std::string *directory_path)
{
	// �o�̓t�H���_�̍쐬
	if (!PathIsDirectory(directory_path->c_str()))
	{
		if (MakeSureDirectoryPathExists(directory_path->c_str()))
		{
			std::cout << "�o�̓t�H���_�w" << *directory_path << "�x���쐬���܂����B" << std::endl;
		}
	}
	else
	{
		std::cout << "�o�̓t�H���_�w" << *directory_path << "�x�͍쐬�ς݂ł����B" << std::endl;
	}
}



void FbxConverter::CreateExportDirectoryPath(std::string* directory_path,
											 std::string* file_name)
{
	*directory_path = (axis_system_ == FbxAxisSystem::DirectX ?
					   "MdBin\\mdbin_l\\" : "MdBin\\mdbin_r\\");
	*directory_path += *file_name + "\\";
}



void FbxConverter::CreateExportFileName(std::string* file_name,
										std::string* text_data_file_name)
{
	*text_data_file_name = *file_name + ".txt";
	*file_name += (axis_system_ == FbxAxisSystem::DirectX ?
				   ".mdbin_l" : ".mdbin_r");
}



void FbxConverter::CreateExportPath(std::string* export_file_path,
									std::string* export_txt_data_file_path,
									std::string* file_name,
									std::string* text_data_file_name,
									std::string* directory_path)
{
	*export_file_path = *directory_path + *file_name;
	*export_txt_data_file_path = *directory_path + *text_data_file_name;
}



void FbxConverter::ExportTextData(std::string* export_txt_data_file_path)
{
	std::ofstream ofstream(export_txt_data_file_path->c_str());

	// �}�e���A��
	ofstream << "[MaterialData]\n" << std::endl;
	int material_num = md_bin_data_container_.getMaterialArraySize();
	ofstream << "���}�e���A���� : " << material_num << "\n" << std::endl;
	for (int i = 0; i < material_num; i++)
	{
		ofstream << "==============================" << std::endl;
		ofstream << "���}�e���A���ԍ� : " << i << "\n" << std::endl;
		ofstream << "Name        : "
			<< *md_bin_data_container_.getpMaterial(i)->getpName()
			<< "\n" << std::endl;
		ofstream << "Ambient     : "
			<< "{" << *md_bin_data_container_.getpMaterial(i)->getpAmbient()->getpR()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpAmbient()->getpG()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpAmbient()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Diffuse     : "
			<< "{" << *md_bin_data_container_.getpMaterial(i)->getpDiffuse()->getpR()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpDiffuse()->getpG()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpDiffuse()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Emissive    : "
			<< "{" << *md_bin_data_container_.getpMaterial(i)->getpEmissive()->getpR()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpEmissive()->getpG()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpEmissive()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Bump        : "
			<< "{" << *md_bin_data_container_.getpMaterial(i)->getpBump()->getpR()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpBump()->getpG()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpBump()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Transparent : "
			<< *md_bin_data_container_.getpMaterial(i)->getpTransparent()
			<< "\n" << std::endl;
		ofstream << "Specular    : "
			<< "{" << *md_bin_data_container_.getpMaterial(i)->getpSpecular()->getpR()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpSpecular()->getpG()
			<< ", " << *md_bin_data_container_.getpMaterial(i)->getpSpecular()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Power       : "
			<< *md_bin_data_container_.getpMaterial(i)->getpPower()
			<< "\n" << std::endl;
		ofstream << "Reflection  : "
			<< *md_bin_data_container_.getpMaterial(i)->getpReflection()
			<< "\n" << std::endl;

		for (int j = 0; j < md_bin_data_container_.getpMaterial(i)->getTextureArraySize(); j++)
		{
			ofstream << "TextureName : " << *md_bin_data_container_.getpMaterial(i)->getpTexture(j)->getpFilePath() << std::endl;
		}

		ofstream << "==============================\n" << std::endl;
	}
	ofstream << "************************************************************" << std::endl;

	// ���b�V��
	ofstream << "\n[MeshData]\n" << std::endl;
	int mesh_num = md_bin_data_container_.getMeshArraySize();
	ofstream << "�����b�V���� : " << mesh_num << "\n" << std::endl;
	for (int i = 0; i < mesh_num; i++)
	{
		ofstream << "==============================" << std::endl;
		ofstream << "�����b�V���ԍ� : " << i << "\n" << std::endl;

		// �C���f�b�N�X
		int index_num = md_bin_data_container_.getpMesh(i)->getIndexArraySize();
		ofstream << "�C���f�b�N�X�� : " << index_num << std::endl;
		for (int j = 0; j < index_num; j += 3)
		{
			ofstream << "�C���f�b�N�X�ԍ� : "
				<< "{ " << *md_bin_data_container_.getpMesh(i)->getpIndex(j)
				<< ", " << *md_bin_data_container_.getpMesh(i)->getpIndex(j + 1)
				<< ", " << *md_bin_data_container_.getpMesh(i)->getpIndex(j + 2)
				<< " }" << std::endl;
		}
		ofstream << std::endl;

		// ���_
		int vertex_num = md_bin_data_container_.getpMesh(i)->getVertexArraySize();
		ofstream << "���_�� : " << vertex_num << std::endl;
		for (int j = 0; j < vertex_num; j++)
		{
			ofstream << "���_�ԍ�" << j << " : "
				<< "{ " << *md_bin_data_container_.getpMesh(i)->getpPosition(j)->getpX()
				<< ", " << *md_bin_data_container_.getpMesh(i)->getpPosition(j)->getpY()
				<< ", " << *md_bin_data_container_.getpMesh(i)->getpPosition(j)->getpZ()
				<< " }" << std::endl;
		}
		ofstream << std::endl;

		// �@��
		int normal_num = md_bin_data_container_.getpMesh(i)->getNormalArraySize();
		ofstream << "�@���� : " << normal_num << std::endl;

		for (int j = 0; j < normal_num; j++)
		{
			ofstream << "�@���ԍ�" << j << " : "
				<< "{ " << *md_bin_data_container_.getpMesh(i)->getpNormal(j)->getpX()
				<< ", " << *md_bin_data_container_.getpMesh(i)->getpNormal(j)->getpY()
				<< ", " << *md_bin_data_container_.getpMesh(i)->getpNormal(j)->getpZ()
				<< " }" << std::endl;
		}
		ofstream << std::endl;

		// UV�Z�b�g
		int uv_set_num = md_bin_data_container_.getpMesh(i)->getUVSetArraySize();
		ofstream << "UV�Z�b�g�� : " << uv_set_num << std::endl;

		for (int j = 0; j < uv_set_num; j++)
		{
			int uv_num = md_bin_data_container_.getpMesh(i)->getpUVSet(j)->getUVArraySize();
			ofstream << "UV�� : " << uv_num << std::endl;

			for (int k = 0; k < uv_num; k++)
			{
				ofstream << "UV�ԍ�" << k << " : "
					<< "{ " << *md_bin_data_container_.getpMesh(i)->getpUVSet(j)->getpU(k)
					<< ", " << *md_bin_data_container_.getpMesh(i)->getpUVSet(j)->getpV(k)
					<< " }" << std::endl;
			}
		}
		ofstream << std::endl;

		// �{�[��
		int bone_num = md_bin_data_container_.getpMesh(i)->getBoneArraySize();
		ofstream << "�{�[���� : " << bone_num << std::endl;

		for (int j = 0; j < bone_num; j++)
		{
			ofstream << "�{�[���ԍ�" << j << " : " << std::endl;
			ofstream << "�{�[���� : "
				<< "{ " << *md_bin_data_container_.getpMesh(i)->getpBone(j)->getpName()
				<< " }" << std::endl;

			ofstream << "�I�t�Z�b�g�s�� : " << std::endl;
			for (int k = 0; k < MdBinDataContainer::Matrix::ARRAY_HEIGHT; k++)
			{
				ofstream << "{ ";
				for (int l = 0; l < MdBinDataContainer::Matrix::ARRAY_WIDTH; l++)
				{
					ofstream << md_bin_data_container_.getpMesh(i)
						->getpBone(j)->getpOffsetMatrix()->getMatrixElement(k, l)
						<< " ";
				}
				ofstream << " }" << std::endl;
			}

			ofstream << "�A�j���[�V�����s�� : " << std::endl;
			for (int k = 0; k < md_bin_data_container_.getpMesh(i)->getpBone(j)->getAnimationMatrixArraySize(); k++)
			{
				ofstream << k << "�t���[���� : " << std::endl;
				for (int l = 0; l < MdBinDataContainer::Matrix::ARRAY_HEIGHT; l++)
				{
					ofstream << "{ ";
					for (int m = 0; m < MdBinDataContainer::Matrix::ARRAY_WIDTH; m++)
					{
						ofstream << md_bin_data_container_.getpMesh(i)
							->getpBone(j)->getpOffsetMatrix()->getMatrixElement(l, m)
							<< " ";
					}
					ofstream << " }" << std::endl;
				}
			}
			
		}
		ofstream << std::endl;

		// �{�[���̏d��
		int bone_weight_num = md_bin_data_container_.getpMesh(i)->getBoneWeightArraySize();
		ofstream << "�{�[���̏d�ݐ� : " << bone_weight_num << std::endl;

		for (int j = 0; j < bone_weight_num; j++)
		{
			ofstream << "�{�[���̏d�ݒ��_�ԍ�" << j << " : ";
			for (int k = 0; k < MdBinDataContainer::Mesh::BoneWeight::MAX_BONE_NUM; k++)
			{
				ofstream << "{ " << md_bin_data_container_.getpMesh(i)->getpBoneWeight(j)->getBoneIndex(k)
					<< ", " << md_bin_data_container_.getpMesh(i)->getpBoneWeight(j)->getBoneWeight(k)
					<< " } ";
			}
			ofstream << std::endl;
		}
		ofstream << std::endl;

		// �֘A�t�����}�e���A���ԍ�
		int material_index_num = *md_bin_data_container_.getpMesh(i)->getpMaterialIndex();
		ofstream << "�֘A�t�����}�e���A���ԍ� : " << material_index_num << std::endl;
		ofstream << std::endl;

		// �֘A�t�����e�N�X�`����
		if (md_bin_data_container_.getpMesh(i)->getUVSetArraySize() > 0)
		{
			for (int j = 0; j < md_bin_data_container_.getpMesh(i)->getpUVSet(0)->getTextureArraySize(); j++)
			{
				ofstream << "UVSet�Ɗ֘A�t����Texture�� : " << *md_bin_data_container_.getpMesh(i)->getpUVSet(0)->getpTexture(j)->getpFilePath() << std::endl;
			}
		}
		ofstream << "==============================\n" << std::endl;
	}

	ofstream.close();
}



void FbxConverter::ConvertToFullPathOfUTF8(std::string* file_path)
{
	// �t���p�X�֕ύX
	char full_path[_MAX_PATH];
	_fullpath(full_path, file_path->c_str(), _MAX_PATH);

	// �t���p�X����UTF8�p�X�̃|�C���^���擾
	char* utf8_path;
	FbxAnsiToUTF8(full_path, utf8_path);

	// UTF8�p�X��string�֕ϊ�
	*file_path = utf8_path;

	// FBXSDK���Ŋm�ۂ���Ă���UTF8�p�X�̃����������
	FbxFree(utf8_path);
}