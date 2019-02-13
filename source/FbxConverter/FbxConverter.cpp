//================================================================================
//!	@file	 FbxConverter.cpp
//!	@brief	 FBX�ϊ���Class
//! @details License : MIT
//!	@author  Kai Araki									@date 2018/12/29
//================================================================================



//****************************************
// �C���N���[�h��
//****************************************
#include <iostream>

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
		printf("���}�l�[�W���̍쐬�Ɏ��s\n");
		return false;
	}

	// IOSettings�I�u�W�F�N�g(�SInport/Export�̐ݒ��ێ�����)�̍쐬
	FbxIOSettings* io_settings_ = FbxIOSettings::Create(manager_, IOSROOT);
	if (!io_settings_)
	{
		printf("��FbxIOSettings�̍쐬�Ɏ��s\n");
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
		printf("�����V�[���̍쐬�Ɏ��s\n");
		return false;
	}

	return true;
}



void FbxConverter::Uninit()
{
	// �}�l�[�W�����(�֘A����S�I�u�W�F�N�g����������)
	manager_->Destroy();

	printf("\n��FBX�̕ϊ��I��...");
}



bool FbxConverter::ConvertToMdBin(std::string* file_path)
{
	// FBX�̓ǂݍ���
	if (!Load(file_path))
	{
		printf("��FBX�̓ǂݍ��ݎ��s\n");
		return false;
	}

	// FBX����f�[�^�[�𒊏o
	if (!ExtractData())
	{
		printf("���f�[�^�̒��o�Ɏ��s\n");
		return false;
	}

	// MdBin�t�@�C���̏o��
	export_file_.Export(file_path);

	return true;
}



bool FbxConverter::Load(std::string* file_path)
{
	printf("\n[Load]\n\n");

	// �C���|�[�^�\�̍쐬
	FbxImporter* importer = FbxImporter::Create(manager_, "");
	if (!importer)
	{
		printf("���C���|�[�^�\�̍쐬�Ɏ��s\n");
		return false;
	}

	// �C���|�[�^�\�̏�����
	std::string temp_file_path = *file_path;
	ConvertToFullPathOfUTF8(&temp_file_path);
	if (!importer->Initialize(temp_file_path.c_str(), -1, manager_->GetIOSettings()))
	{
		printf("���C���|�[�^�\�̏������Ɏ��s\n");
		return false;
	}

	// �V�[���̗�������
	importer->Import(scene_);
	printf("���V�[�����쐬\n");

	// �t���[����񒊏o
	ExtractFrameData(importer);

	// �C���|�[�^�[���
	importer->Destroy();

	// ���W�n�̕ϊ�
	std::string temp;
	printf("\n���ǂ���̍��W�n�ɂ��܂����H DirectX[y], OpenGL[n]\n");
	do
	{
		printf("��");
		std::cin >> temp;
	} while (temp != "y" && temp != "n");

	axis_system_ = temp == "y" ? FbxAxisSystem::DirectX : FbxAxisSystem::OpenGL;

	if (scene_->GetGlobalSettings().GetAxisSystem() != axis_system_)
	{
		//std::cout << "�����f�[�^���w����W�n�ƈقȂ�וϊ�\n��";
		//
		//if (axis_system_ == FbxAxisSystem::DirectX)
		//{
		//	FbxAxisSystem::DirectX.ConvertScene(scene_);
		//	std::cout << "����n���W�֕ϊ�����" << std::endl;
		//}
		//else
		//{
		//	FbxAxisSystem::OpenGL.ConvertScene(scene_);
		//	std::cout << "�E��n���W�֕ϊ�����" << std::endl;
		//}
	}

	// ���l���]�̊m�F
	printf("\n�����l�𔽓]���܂����H Yes[y], No[n]\n");
	do
	{
		printf("��");
		std::cin >> temp;
	} while (temp != "y" && temp != "n");
	is_reverse_alpha_ = temp == "y";

	// �S�|���S�����O�p�`�|���S���֕ϊ�
	FbxGeometryConverter geometry_converter(manager_);
	if (geometry_converter.Triangulate(scene_, true))
	{
		printf("\n���S�|���S�����O�p�`�|���S���֕ϊ�\n");
		geometry_converter.RemoveBadPolygonsFromMeshes(scene_);
	}

	// �}�e���A�����ƂɃ��b�V���𕪊�
	if (geometry_converter.SplitMeshesPerMaterial(scene_, true))
	{
		printf("\n���}�e���A�����ƂɃ��b�V���𕪊�\n");
	}

	// ���[�g�m�[�h�̎擾
	root_node_ = scene_->GetRootNode();
	if (!root_node_)
	{
		printf("\n�����[�g�m�[�h�̎擾�Ɏ��s\n");
		return false;
	}

	printf("\n************************************************************\n");

	// �o�̓t�@�C���̏�����
	export_file_.setMdBinData(&md_bin_data_);
	export_file_.setAxis(axis_system_);

	return true;
}



void FbxConverter::ExtractFrameData(FbxImporter* importer)
{
	// 1�t���[���̎��Ԃ��擾
	FbxTime::EMode time_mode = scene_->GetGlobalSettings().GetTimeMode();
	period_.SetTime(0, 0, 0, 1, 0, time_mode);

	int animation_stack_count = importer->GetAnimStackCount();
	printf("\n���e�C�N���F%d\n", animation_stack_count);

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

		break;
	}
	if (all_animation_frame_num_ > 0)
	{
		printf("�˃A�j���[�V�������F�L\n");
		printf("��START_FRAME_NUM�F%d\n", animation_start_frame_num_);
		printf("��STOP_FRAME_NUM�F%d\n", animation_stop_frame_num_);
		printf("��ALL_FRAME_NUM�F%d\n", all_animation_frame_num_);
	}
	else
	{
		printf("�˃A�j���[�V�������F��\n");
	}
}



bool FbxConverter::ExtractData()
{
	// �}�e���A���f�[�^���o
	ExtractMaterialData();

	// ���b�V���f�[�^���o
	ExtractMeshData();

	// �A�j���[�V�������ۑ�
	md_bin_data_.setAnimationFramNum(all_animation_frame_num_);

	return true;
}



void FbxConverter::ExtractMaterialData()
{
	// �}�e���A�����擾
	int material_num = scene_->GetMaterialCount();
	md_bin_data_.setMaterialArraySize(material_num);

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
	md_bin_data_.setMeshArraySize(mesh_num);

	// �C���f�b�N�X�̕��тŊe�f�[�^�𒊏o(����UV�̊֌W��C���f�b�N�X�̍œK���͍s��Ȃ�)
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
	*md_bin_data_.getpMaterial(material_index)->getpName() = material->GetName();
}



void FbxConverter::ExtractAmbientData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sAmbient);
	if (property.IsValid())
	{
		FbxDouble3 ambient = property.Get<FbxDouble3>();
		*md_bin_data_.getpMaterial(material_index)->getpAmbient()->getpR() = (float)ambient[0];
		*md_bin_data_.getpMaterial(material_index)->getpAmbient()->getpG() = (float)ambient[1];
		*md_bin_data_.getpMaterial(material_index)->getpAmbient()->getpB() = (float)ambient[2];

		ExtractTextureData(material_index, property,
						   MdBinData::Material::Texture::Type::AMBIENT);
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpAmbient()->getpR() = 1.0f;
		*md_bin_data_.getpMaterial(material_index)->getpAmbient()->getpG() = 1.0f;
		*md_bin_data_.getpMaterial(material_index)->getpAmbient()->getpB() = 1.0f;
	}
}



void FbxConverter::ExtractDiffuseData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (property.IsValid())
	{
		FbxDouble3 diffuse = property.Get<FbxDouble3>();
		*md_bin_data_.getpMaterial(material_index)->getpDiffuse()->getpR() = (float)diffuse[0];
		*md_bin_data_.getpMaterial(material_index)->getpDiffuse()->getpG() = (float)diffuse[1];
		*md_bin_data_.getpMaterial(material_index)->getpDiffuse()->getpB() = (float)diffuse[2];

		ExtractTextureData(material_index, property,
						   MdBinData::Material::Texture::Type::DIFFUSE);
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpDiffuse()->getpR() = 1.0f;
		*md_bin_data_.getpMaterial(material_index)->getpDiffuse()->getpG() = 1.0f;
		*md_bin_data_.getpMaterial(material_index)->getpDiffuse()->getpB() = 1.0f;
	}
}



void FbxConverter::ExtractEmissiveData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sEmissive);
	if (property.IsValid())
	{
		FbxDouble3 emissive = property.Get<FbxDouble3>();
		*md_bin_data_.getpMaterial(material_index)->getpEmissive()->getpR() = (float)emissive[0];
		*md_bin_data_.getpMaterial(material_index)->getpEmissive()->getpG() = (float)emissive[1];
		*md_bin_data_.getpMaterial(material_index)->getpEmissive()->getpB() = (float)emissive[2];

		ExtractTextureData(material_index, property,
						   MdBinData::Material::Texture::Type::EMISSIVE);
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpEmissive()->getpR() = 0.0f;
		*md_bin_data_.getpMaterial(material_index)->getpEmissive()->getpG() = 0.0f;
		*md_bin_data_.getpMaterial(material_index)->getpEmissive()->getpB() = 0.0f;
	}
}



void FbxConverter::ExtractBumpData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sBump);
	if (property.IsValid())
	{
		FbxDouble3 bump = property.Get<FbxDouble3>();
		*md_bin_data_.getpMaterial(material_index)->getpBump()->getpR() = (float)bump[0];
		*md_bin_data_.getpMaterial(material_index)->getpBump()->getpG() = (float)bump[1];
		*md_bin_data_.getpMaterial(material_index)->getpBump()->getpB() = (float)bump[2];

		ExtractTextureData(material_index, property,
						   MdBinData::Material::Texture::Type::NORMAL);
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpBump()->getpR() = 0.0f;
		*md_bin_data_.getpMaterial(material_index)->getpBump()->getpG() = 0.0f;
		*md_bin_data_.getpMaterial(material_index)->getpBump()->getpB() = 0.0f;
	}
}



void FbxConverter::ExtractTransparentData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (property.IsValid())
	{
		FbxDouble transparent = property.Get<FbxDouble>();
		// �����l���t�ɂȂ��Ă���ꍇ�ɑΉ�
		if (is_reverse_alpha_)
		{
			*md_bin_data_.getpMaterial(material_index)->getpTransparent() = 1.0f - (float)transparent;
		}
		else
		{
			*md_bin_data_.getpMaterial(material_index)->getpTransparent() = (float)transparent;
		}
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpTransparent() = 1.0f;
	}
}



void FbxConverter::ExtractSpecularData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sSpecular);
	if (property.IsValid())
	{
		FbxDouble3 specular = property.Get<FbxDouble3>();
		*md_bin_data_.getpMaterial(material_index)->getpSpecular()->getpR() = (float)specular[0];
		*md_bin_data_.getpMaterial(material_index)->getpSpecular()->getpG() = (float)specular[1];
		*md_bin_data_.getpMaterial(material_index)->getpSpecular()->getpB() = (float)specular[2];

		ExtractTextureData(material_index, property,
						   MdBinData::Material::Texture::Type::SPECULAR);
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpSpecular()->getpR() = 0.0f;
		*md_bin_data_.getpMaterial(material_index)->getpSpecular()->getpG() = 0.0f;
		*md_bin_data_.getpMaterial(material_index)->getpSpecular()->getpB() = 0.0f;
	}
}



void FbxConverter::ExtractPowerData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sShininess);
	if (property.IsValid())
	{
		FbxDouble3 specular = property.Get<FbxDouble3>();
		*md_bin_data_.getpMaterial(material_index)->getpPower() = (float)specular[0];
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpPower() = 0.0f;
	}
}



void FbxConverter::ExtractReflectionData(int material_index, FbxSurfaceMaterial* material)
{
	FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
	if (property.IsValid())
	{
		FbxDouble reflection = property.Get<FbxDouble>();
		*md_bin_data_.getpMaterial(material_index)->getpReflection() = (float)reflection;
	}
	else
	{
		*md_bin_data_.getpMaterial(material_index)->getpReflection() = 0.0f;
	}
}



void FbxConverter::ExtractTextureData(int material_index, FbxProperty property,
									  MdBinData::Material::Texture::Type texture_type)
{
	// �e�N�X�`�����擾
	int texture_num = property.GetSrcObjectCount<FbxFileTexture>();
	if (texture_num > 0)
	{
		// �e�N�X�`�����쐬
		MdBinData::Material::Texture* texture = new MdBinData::Material::Texture();
		FbxFileTexture* fbx_texture = property.GetSrcObject<FbxFileTexture>(0);
		*texture->getpFilePath() = fbx_texture->GetRelativeFileName();
		ExtractFileName(texture->getpFilePath(), texture->getpFilePath());
		*texture->getpType() = texture_type;

		// �e�N�X�`����ǉ�
		material_index = material_index;
		md_bin_data_.getpMaterial(material_index)->AddTextureArray(texture);
	}
}



void FbxConverter::ExtractFileName(std::string* file_name, std::string* file_path)
{
	// �t�@�C�����𒊏o(�p�X��؂�"\"��"/"�̗����ɑΉ�)
	auto temp_index = file_path->find_last_of("/");	// �Ō�̃p�X��؂�̃C���f�b�N�X���擾
	if (temp_index != std::string::npos)
	{
		// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
		*file_name = file_path->substr(temp_index + 1);
	}
	else
	{
		temp_index = file_path->find_last_of("\\");
		if (temp_index != std::string::npos)
		{
			// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
			*file_name = file_path->substr(temp_index + 1);
		}
		else
		{
			// �t�@�C�����𒊏o
			*file_name = *file_path;
		}
	}
}



void FbxConverter::ExtractIndexData(int mesh_index, FbxMesh* mesh)
{
	// �C���f�b�N�X���擾
	int index_num = mesh->GetPolygonVertexCount();
	md_bin_data_.getpMesh(mesh_index)->setIndexArraySize(index_num);

	// �C���f�b�N�X�擾
	int* index_array = nullptr;
	index_array = mesh->GetPolygonVertices();
	for (int i = 0; i < index_num; i++)
	{
		*md_bin_data_.getpMesh(mesh_index)->getpIndex(i) = index_array[i];
	}
}



void FbxConverter::ExtractVertexData(int mesh_index, FbxMesh* mesh)
{
	// ���_�����C���f�b�N�X�����m��
	int vertex_num = md_bin_data_.getpMesh(mesh_index)->getIndexArraySize();
	md_bin_data_.getpMesh(mesh_index)->setPositionArraySize(vertex_num);

	// ���_�擾
	FbxVector4* vertex_array = mesh->GetControlPoints();
	for (int i = 0; i < vertex_num; i++)
	{
		FbxVector4 vertex = vertex_array[*md_bin_data_.getpMesh(mesh_index)->getpIndex(i)];
		*md_bin_data_.getpMesh(mesh_index)->getpPosition(i)->getpX() = (float)vertex[0];
		*md_bin_data_.getpMesh(mesh_index)->getpPosition(i)->getpY() = (float)vertex[1];
		*md_bin_data_.getpMesh(mesh_index)->getpPosition(i)->getpZ() = (float)vertex[2];
	}
}



void FbxConverter::ExtractNormalData(int mesh_index, FbxMesh* mesh)
{
	// �@���z��(�C���f�b�N�X�Ɠ�������)
	FbxArray<FbxVector4> normal_array;
	mesh->GetPolygonVertexNormals(normal_array);

	// �@�������C���f�b�N�X�����m��
	md_bin_data_.getpMesh(mesh_index)->setNormalArraySize(normal_array.Size());

	// �@���擾
	for (int i = 0; i < normal_array.Size(); i++)
	{
		*md_bin_data_.getpMesh(mesh_index)->getpNormal(i)->getpX() = (float)normal_array[i][0];
		*md_bin_data_.getpMesh(mesh_index)->getpNormal(i)->getpY() = (float)normal_array[i][1];
		*md_bin_data_.getpMesh(mesh_index)->getpNormal(i)->getpZ() = (float)normal_array[i][2];
	}
}



void FbxConverter::ExtractUVSetData(int mesh_index, FbxMesh* mesh)
{
	// UV�Z�b�g���z��擾
	FbxStringList uv_set_name_array;
	mesh->GetUVSetNames(uv_set_name_array);
	md_bin_data_.getpMesh(mesh_index)->setUVSetArraySize(uv_set_name_array.GetCount());

	for (int i = 0; i < uv_set_name_array.GetCount(); i++)
	{
		// UV�z��(�C���f�b�N�X�Ɠ�������)
		FbxArray<FbxVector2> uv_set_array;
		mesh->GetPolygonVertexUVs(uv_set_name_array.GetStringAt(i), uv_set_array);

		// UV�����C���f�b�N�X�����m��
		md_bin_data_.getpMesh(mesh_index)->getpUVSet(i)->setUVArraySize(uv_set_array.Size());

		// UV�擾(�����Ƃ�V�l��ϊ�)
		for (int j = 0; j < md_bin_data_.getpMesh(mesh_index)->getpUVSet(i)->getUVArraySize(); j++)
		{
			*md_bin_data_.getpMesh(mesh_index)->getpUVSet(i)->getpU(j) = (float)uv_set_array[j][0];
			if (axis_system_ == FbxAxisSystem::DirectX)
			{
				*md_bin_data_.getpMesh(mesh_index)->getpUVSet(i)->getpV(j) = 1.0f - (float)uv_set_array[j][1];
			}
			else
			{
				*md_bin_data_.getpMesh(mesh_index)->getpUVSet(i)->getpV(j) = (float)uv_set_array[j][1];
			}
		}
	}
}



void FbxConverter::ExtractAnimationData(int mesh_index, FbxMesh* mesh)
{
	// �{�[���̏d�ݕۑ��p�z��𒸓_�����m��
	std::vector<MdBinData::Mesh::BoneWeight> save_bone_weight_array;
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
	int bone_weight_num = md_bin_data_.getpMesh(mesh_index)->getIndexArraySize();
	md_bin_data_.getpMesh(mesh_index)->setBoneWeightArraySize(bone_weight_num);
	for (int i = 0; i < bone_weight_num; i++)
	{
		int index = *md_bin_data_.getpMesh(mesh_index)->getpIndex(i);
		*md_bin_data_.getpMesh(mesh_index)->getpBoneWeight(i) = save_bone_weight_array[index];
	}
}



void FbxConverter::ExtractBoneData(int mesh_index, FbxCluster* cluster)
{

	int affected_vertices_num = cluster->GetControlPointIndicesCount();
	if (affected_vertices_num <= 0) return;

	// �{�[�����擾
	MdBinData::Mesh::Bone bone;
	*bone.getpName() = cluster->GetName();

	// �I�t�Z�b�g�s��擾
	FbxAMatrix init_matrix;
	cluster->GetTransformLinkMatrix(init_matrix);
	ChangeMatrix(bone.getpOffsetMatrix(), &init_matrix);

	// �A�j���[�V�����s��擾
	if (all_animation_frame_num_ > 0)
	{
		bone.setAnimationMatrixArraySize(all_animation_frame_num_);
		for (int i = 0; i < all_animation_frame_num_; i++)
		{
			FbxAMatrix animation_matrix;
			FbxTime time = (animation_start_frame_num_ + i) * period_.Get();
			animation_matrix = cluster->GetLink()->EvaluateGlobalTransform(time);
			ChangeMatrix(bone.getpAnimationMatrix(i), &animation_matrix);
		}
	}
	else
	{
		all_animation_frame_num_ = 1;
		bone.setAnimationMatrixArraySize(all_animation_frame_num_);
		ChangeMatrix(bone.getpAnimationMatrix(0), &init_matrix);
	}

	// ���b�V���̃{�[���z��֒ǉ�
	md_bin_data_.getpMesh(mesh_index)->AddBoneArray(&bone);
}



void FbxConverter::ExtractBoneWeightData(int mesh_index,
										 std::vector<MdBinData::Mesh::BoneWeight>* save_bone_weight_array,
										 FbxCluster* cluster)
{
	// ���b�V���̃{�[���z��̖����̃C���f�b�N�X�擾
	int bone_index = md_bin_data_.getpMesh(mesh_index)->getBoneArrayEndIndex();

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

		save_bone_weight_array->at(vertex_index).setBoneIndexAndWeight(bone_index,
																	   bone_weight);
	}
}



void FbxConverter::ChangeMatrix(MdBinData::Matrix* bone_matrix,
								FbxAMatrix* fbx_matrix)
{
	for (int i = 0; i < MdBinData::Matrix::ARRAY_HEIGHT; i++)
	{
		for (int j = 0; j < MdBinData::Matrix::ARRAY_WIDTH; j++)
		{
			bone_matrix->setMatrixElement((float)fbx_matrix->Get(i, j), i, j);
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
			for (int j = 0; j < md_bin_data_.getMaterialArraySize(); j++)
			{
				if (material_name
					!= *md_bin_data_.getpMaterial(j)->getpName()) continue;

				*md_bin_data_.getpMesh(mesh_index)->getpMaterialIndex()	= j;
				break;
			}
		}
	}
	else
	{
		// �}�e���A���Ƃ̊֘A��񂪂Ȃ�
		*md_bin_data_.getpMesh(mesh_index)->getpMaterialIndex() = -1;
	}
}



void FbxConverter::AssociatingUVSetDataAndTexture(int mesh_index)
{
	// UVSet�����邩�ǂ���
	if (md_bin_data_.getpMesh(mesh_index)->getUVSetArraySize() <= 0) return;

	// �}�e���A���擾
	int material_index = *md_bin_data_.getpMesh(mesh_index)->getpMaterialIndex();
	MdBinData::Material* material = md_bin_data_.getpMaterial(material_index);

	// �}�e���A���̃e�N�X�`������f�B�q���[�Y�e�N�X�`����UV��0�ԖڂƊ֘A�t����
	for (int i = 0; i < (int)material->getTextureArraySize(); i++)
	{
		if (*material->getpTexture(i)->getpType()
			!= MdBinData::Material::Texture::Type::DIFFUSE) continue;

		md_bin_data_.getpMesh(mesh_index)->getpUVSet(0)
			->AddTextureArray(material->getpTexture(i));
	}
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