//================================================================================
//!	@file	 main.cpp
//!	@brief	 ���C��
//! @details 
//!	@author  Kai Araki									@date 2018/12/12
//================================================================================



//****************************************
// �C���N���[�h��
//****************************************
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <iostream>
#include <string>
#include <fbxsdk.h>



//****************************************
// �萔��`
//****************************************



//****************************************
// �v���g�^�C�v�錾
//****************************************
bool Init();
bool Load(std::string* file_path, FbxAxisSystem fbx_axis_system);
void ConvertToFullPathOfUTF8(std::string* file_path);

bool ExportOfConvertedFBXFile(std::string* file_path);
void CreateExportDirectory();
void CreateExportFileName(std::string* export_file_path, std::string* file_path);



//****************************************
// �O���[�o���ϐ��錾
//****************************************
FbxManager* manager_ = nullptr;
FbxIOSettings* io_settings_ = nullptr;
FbxScene* scene_ = nullptr;


//--------------------------------------------------
// -�A�v���P�[�V�����̃G���g���[�֐�
//--------------------------------------------------
int main( void )
{
	// ���������[�N���o
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	// ������
	if (!Init())
	{
		std::cout << "�������Ɏ��s" << std::endl;
		rewind(stdin);
		getchar();
		return -1;
	}

	// �ǂݍ���
	std::string file_path = "BoxUnityChan.fbx";
	//std::cout << "FBX�t�@�C���p�X�̓���(Drag&Drop����)\n��";
	//std::cin >> file_path;
	if (!Load(&file_path, FbxAxisSystem::DirectX))
	{
		std::cout << "FBX�̓ǂݍ��ݎ��s" << std::endl;
		rewind(stdin);
		getchar();
		return -1;
	}

	// �}�l�[�W�����(�֘A����S�I�u�W�F�N�g����������)
	manager_->Destroy();

	std::cout << "FBX�̓ǂݍ��ݐ���" << std::endl;

	rewind(stdin);
	getchar();

	return 0;
}



//--------------------------------------------------
// -�������֐�
//--------------------------------------------------
bool Init()
{
	// �}�l�[�W���̐���
	manager_ = FbxManager::Create();
	if (!manager_)
	{
		std::cout << "�}�l�[�W���̍쐬�Ɏ��s" << std::endl;
		return false;
	}

	// IOSettings�I�u�W�F�N�g(�SInport/Export�̐ݒ��ێ�����)�̍쐬
	io_settings_ = FbxIOSettings::Create(manager_, IOSROOT);
	if (!io_settings_)
	{
		std::cout << "FbxIOSettings�̍쐬�Ɏ��s" << std::endl;
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
		std::cout << "�V�[���̍쐬�Ɏ��s" << std::endl;
		return false;
	}

	return true;
}



//--------------------------------------------------
// -�ǂݍ��݊֐�
//--------------------------------------------------
bool Load(std::string* file_path, FbxAxisSystem fbx_axis_system)
{
	// �C���|�[�^�\�̍쐬
	FbxImporter* importer = FbxImporter::Create(manager_, "");
	if (!importer)
	{
		std::cout << "�C���|�[�^�\�̍쐬�Ɏ��s" << std::endl;
		return false;
	}

	// �C���|�[�^�\�̏�����
	std::string temp_file_path = *file_path;
	ConvertToFullPathOfUTF8(&temp_file_path);
	if (!importer->Initialize(temp_file_path.c_str(), -1, manager_->GetIOSettings()))
	{
		std::cout << "�C���|�[�^�\�̏������Ɏ��s" << std::endl;
		return false;
	}

	// �V�[���̗������݂ƃC���|�[�^�[�̉��
	importer->Import(scene_);
	importer->Destroy();

	// ���W�n�̕ϊ�
	if (scene_->GetGlobalSettings().GetAxisSystem() != fbx_axis_system)
	{
		std::cout << "�w����W�n�ƈقȂ�וϊ� �� ";

		if (fbx_axis_system == FbxAxisSystem::DirectX)
		{
			FbxAxisSystem::DirectX.ConvertScene(scene_);
			std::cout << "����n���W�֕ϊ�" << std::endl;
		}
		else
		{
			FbxAxisSystem::OpenGL.ConvertScene(scene_);
			std::cout << "�E��n���W�֕ϊ�" << std::endl;
		}
	}

	// �X�P�[�����p�P�ʂ֕ϊ�
	FbxSystemUnit system_unit = scene_->GetGlobalSettings().GetSystemUnit();
	if (system_unit.GetScaleFactor() != 1.0f)
	{
		FbxSystemUnit::cm.ConvertScene(scene_);
		std::cout << "�X�P�[�����p�P�ʂ֕ϊ�" << std::endl;
	}

	// �S�|���S�����O�p�`�|���S���֕ϊ�
	FbxGeometryConverter geometry_converter(manager_);
	if (geometry_converter.Triangulate(scene_, true))
	{
		std::cout << "�S�|���S�����O�p�`�|���S���֕ϊ�" << std::endl;
		geometry_converter.RemoveBadPolygonsFromMeshes(scene_);
	}

	// �}�e���A�����ƂɃ��b�V���𕪊�
	if (geometry_converter.SplitMeshesPerMaterial(scene_, true))
	{
		std::cout << "�}�e���A�����ƂɃ��b�V���𕪊�" << std::endl;
	}
	

	// ���[�g�m�[�h�̎擾
	FbxNode* root_node = scene_->GetRootNode();
	if (!root_node)
	{
		std::cout << "���[�g�m�[�h�̎擾�Ɏ��s" << std::endl;
		return false;
	}

	// �ϊ��ς�FBX�t�@�C���̏o��
	if (ExportOfConvertedFBXFile(file_path))
	{
		std::cout << "�ϊ��ς�FBX�t�@�C�����o�͂��܂���" << std::endl;
	}
	else
	{
		std::cout << "�ϊ��ς�FBX�t�@�C�����o�͂��܂���ł���" << std::endl;
	}

	return true;
}



//--------------------------------------------------
// -UTF8�̃t���p�X�֕ϊ��֐�
//--------------------------------------------------
void ConvertToFullPathOfUTF8(std::string* file_path)
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



//--------------------------------------------------
// -�ϊ��ς�FBX�t�@�C���̏o�͊֐�
//--------------------------------------------------
bool ExportOfConvertedFBXFile(std::string* file_path)
{
	// �o�̓t�H���_�̍쐬
	CreateExportDirectory();

	// �o�̓t�@�C�����쐬
	std::string export_file_path;
	CreateExportFileName(&export_file_path, file_path);

	// �o�̓t�@�C���̏�������
	std::string temp = "mdbin/" + export_file_path;
	FILE* file = fopen(temp.c_str(), "rb");
	if (file != nullptr)
	{
		std::cout << "�����t�@�C�����w" << export_file_path << "�x������܂��B" << std::endl;
		std::cout << "�㏑�����܂����H Yes[y], No[n]" << std::endl;
		do
		{
			std::cout << "��"; 
			std::cin >> temp;
		} while (temp != "y" && temp != "n");
		
		if (temp == "n") return false;
	}

	file = fopen(export_file_path.c_str(), "wb");


	fclose(file);

	return true;
}



//--------------------------------------------------
// -�o�̓t�H���_�̍쐬�֐�
//--------------------------------------------------
void CreateExportDirectory()
{
	// �o�̓t�H���_�̍쐬
	if (_mkdir("mdbin") == 0)
	{
		std::cout << "�o�̓t�H���_���쐬" << std::endl;
	}
	else
	{
		std::cout << "�o�̓t�H���_�͊��ɍ쐬�ς݂ł���" << std::endl;
	}
}



//--------------------------------------------------
// -�o�̓t�@�C�����̍쐬�֐�
//--------------------------------------------------
void CreateExportFileName(std::string* export_file_path, std::string* file_path)
{
	// �t�@�C�����𒊏o(�p�X��؂�"\"��"/"�̗����ɑΉ�)
	auto temp_index = file_path->find_last_of("/");	// �Ō�̃p�X��؂�̃C���f�b�N�X���擾
	if (temp_index != std::string::npos)
	{
		// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
		*export_file_path = file_path->substr(temp_index + 1);
	}
	else
	{
		temp_index = file_path->find_last_of("\\");
		if (temp_index != std::string::npos)
		{
			// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
			*export_file_path = file_path->substr(temp_index + 1);
		}
		else
		{
			// �t�@�C�����𒊏o
			*export_file_path = *file_path;
		}
	}

	// �g���q��ύX
	temp_index = export_file_path->find_last_of(".");
	if (temp_index != std::string::npos)
	{
		// �t�@�C��������g���q������
		*export_file_path = export_file_path->substr(0, temp_index + 1);

		// �V�K�g���q
		*export_file_path += "mdbin";
	}

	std::cout << "�t�@�C�����F" << *export_file_path << std::endl;
}