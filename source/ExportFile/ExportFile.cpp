//================================================================================
//!	@file	 ExportFile.cpp
//!	@brief	 �t�@�C���o��Class
//! @details 
//!	@author  Kai Araki									@date 2019/01/22
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

#include "ExportFile.h"



//****************************************
// �v���p�e�B��`
//****************************************
void ExportFile::setMdBinData(MdBinData* md_bin_data)
{
	md_bin_data_ = md_bin_data;
}



void ExportFile::setAxis(FbxAxisSystem axis)
{
	axis_ = axis;
}



//****************************************
// �֐���`
//****************************************
void ExportFile::Export(std::string* file_path)
{
	std::cout << "\n[ExportOfMdBinFile]\n" << std::endl;

	// �t�@�C�����쐬
	CreateFileName(file_path);

	// �o�̓f�B���N�g���p�X�쐬
	CreateExportDirectoryPath();

	// �e��t�@�C������
	if (!ExportOfMdBinFile()) return;
	ExportOfLogFile();

	std::cout << "\n�t�@�C���o�͊���\n" << std::endl;
}



bool ExportFile::ExportOfMdBinFile()
{
	// �o�̓t�H���_�쐬
	CreateExportDirectory(&export_directory_path_);

	// �o�̓t�@�C�����쐬
	std::string export_file_name = file_name_;
	CreateExportFileName(&export_file_name);

	// �o�̓p�X�쐬
	std::string export_path;
	CreateExportPath(&export_path, &export_directory_path_, &export_file_name);

	// �㏑���m�F
	std::ifstream ifstream(export_path.c_str(), std::ios::binary);
	if (ifstream)
	{
		ifstream.close();
		std::cout << "\n�����t�@�C�����w" << export_file_name << "�x������܂��B" << std::endl;
		std::cout << "�㏑�����܂����H Yes[y], No[n]" << std::endl;
		std::string temp;
		do
		{
			std::cout << "��";
			std::cin >> temp;
		} while (temp != "y" && temp != "n");

		if (temp == "n") return false;
	}

	// �o��
	std::cout << "\n�o�͒����\n" << std::endl;
	MdBinData::ExportData(md_bin_data_, export_path);
	return true;
}



void ExportFile::ExportOfLogFile()
{
	// �f�B���N�g���p�X�쐬
	std::string material_directory_path;
	std::string mesh_directory_path;
	CreateLogDirectoryPath(&material_directory_path,
						   &mesh_directory_path);

	// �o�̓t�H���_�쐬
	CreateExportDirectory(&material_directory_path);
	CreateExportDirectory(&mesh_directory_path);

	ExportOfMaterialLogFile(&material_directory_path);
	ExportOfMeshLogFile(&mesh_directory_path);
}



void ExportFile::ExportOfMaterialLogFile(std::string* directory_path)
{
	for (int i = 0; i < md_bin_data_->getMaterialArraySize(); i++)
	{
		// �o�̓t�@�C�����쐬
		std::string file_name = "Material";
		CreateIndexLogFileName(&file_name, i);

		// �o�̓p�X�쐬
		std::string export_path;
		CreateExportPath(&export_path, directory_path, &file_name);

		// �t�@�C���I�[�v��
		std::ofstream ofstream(export_path.c_str());

		ofstream << "\n[MaterialData]\n" << std::endl;
		ofstream << "�}�e���A���ԍ� : " << i << "\n" << std::endl;
		ofstream << "Name          : "
			<< *md_bin_data_->getpMaterial(i)->getpName()
			<< "\n" << std::endl;
		ofstream << "Ambient       : "
			<< "{" << *md_bin_data_->getpMaterial(i)->getpAmbient()->getpR()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpAmbient()->getpG()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpAmbient()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Diffuse       : "
			<< "{" << *md_bin_data_->getpMaterial(i)->getpDiffuse()->getpR()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpDiffuse()->getpG()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpDiffuse()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Emissive      : "
			<< "{" << *md_bin_data_->getpMaterial(i)->getpEmissive()->getpR()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpEmissive()->getpG()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpEmissive()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Bump          : "
			<< "{" << *md_bin_data_->getpMaterial(i)->getpBump()->getpR()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpBump()->getpG()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpBump()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Transparent   : "
			<< *md_bin_data_->getpMaterial(i)->getpTransparent()
			<< "\n" << std::endl;
		ofstream << "Specular      : "
			<< "{" << *md_bin_data_->getpMaterial(i)->getpSpecular()->getpR()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpSpecular()->getpG()
			<< ", " << *md_bin_data_->getpMaterial(i)->getpSpecular()->getpB()
			<< "}\n" << std::endl;
		ofstream << "Power         : "
			<< *md_bin_data_->getpMaterial(i)->getpPower()
			<< "\n" << std::endl;
		ofstream << "Reflection    : "
			<< *md_bin_data_->getpMaterial(i)->getpReflection()
			<< "\n" << std::endl;

		for (int j = 0; j < md_bin_data_->getpMaterial(i)->getTextureArraySize(); j++)
		{
			ofstream << "TextureName   : " << *md_bin_data_->getpMaterial(i)->getpTexture(j)->getpFilePath() << std::endl;
		}

		// �t�@�C��Close
		ofstream.close();
	}
}



void ExportFile::ExportOfMeshLogFile(std::string* directory_path)
{
	std::ofstream ofstream;
	std::string file_name;
	std::string export_path;
	for (int i = 0; i < md_bin_data_->getMeshArraySize(); i++)
	{
		// �t�H���_�쐬
		std::string mesh_index_directory_path = *directory_path + "\\Mesh";
		CreateIndexDirectoryPath(&mesh_index_directory_path,
								 &mesh_index_directory_path, i);
		CreateExportDirectory(&mesh_index_directory_path);


		// UV�Z�b�g
		std::string uv_set_directory_path = mesh_index_directory_path + "01_UV�Z�b�g\\";
		CreateExportDirectory(&uv_set_directory_path);

		int uv_set_num = md_bin_data_->getpMesh(i)->getUVSetArraySize();
		for (int j = 0; j < uv_set_num; j++)
		{
			file_name = "UV�Z�b�g";
			CreateIndexLogFileName(&file_name, j);
			CreateExportPath(&export_path, &uv_set_directory_path, &file_name);
			ofstream.open(export_path.c_str());
			ofstream << "\n[MeshData]\n" << std::endl;

			int uv_num = md_bin_data_->getpMesh(i)->getpUVSet(j)->getUVArraySize();
			ofstream << "UV�� : " << uv_num << "\n" << std::endl;
			for (int k = 0; k < uv_num; k++)
			{
				ofstream << "UV�ԍ�" << k << " : "
					<< "{ " << *md_bin_data_->getpMesh(i)->getpUVSet(j)->getpU(k)
					<< ", " << *md_bin_data_->getpMesh(i)->getpUVSet(j)->getpV(k)
					<< " }" << std::endl;
			}
			ofstream.close();
		}

		// �{�[��
		std::string bone_directory_path = mesh_index_directory_path + "02_�{�[��\\";
		CreateExportDirectory(&bone_directory_path);

		int bone_num = md_bin_data_->getpMesh(i)->getBoneArraySize();
		for (int j = 0; j < bone_num; j++)
		{
			file_name = "�{�[��";
			CreateIndexLogFileName(&file_name, j);
			CreateExportPath(&export_path, &bone_directory_path, &file_name);
			ofstream.open(export_path.c_str());
			ofstream << "\n[MeshData]\n" << std::endl;

			ofstream << "�{�[���ԍ� : " << j << "\n" << std::endl;
			ofstream << "�{�[���� : "
				<< "{ " << *md_bin_data_->getpMesh(i)->getpBone(j)->getpName()
				<< " }\n" << std::endl;

			ofstream << "�I�t�Z�b�g�s�� : " << std::endl;
			for (int k = 0; k < MdBinData::Matrix::ARRAY_HEIGHT; k++)
			{
				ofstream << "{ ";
				for (int l = 0; l < MdBinData::Matrix::ARRAY_WIDTH; l++)
				{
					ofstream << md_bin_data_->getpMesh(i)
						->getpBone(j)->getpOffsetMatrix()->getMatrixElement(k, l)
						<< " ";
				}
				ofstream << " }" << std::endl;
			}

			ofstream << "\n�A�j���[�V�����s�� : " << std::endl;
			for (int k = 0; k < md_bin_data_->getpMesh(i)->getpBone(j)->getAnimationMatrixArraySize(); k++)
			{
				ofstream << "�t���[���ԍ�" << k << " : " << std::endl;
				for (int l = 0; l < MdBinData::Matrix::ARRAY_HEIGHT; l++)
				{
					ofstream << "{ ";
					for (int m = 0; m < MdBinData::Matrix::ARRAY_WIDTH; m++)
					{
						ofstream << md_bin_data_->getpMesh(i)
							->getpBone(j)->getpAnimationMatrix(k)->getMatrixElement(l, m)
							<< " ";
					}
					ofstream << " }" << std::endl;
				}
				ofstream << std::endl;
			}
			ofstream.close();
		}

		// �C���f�b�N�X
		file_name = "03_�C���f�b�N�X.txt";
		CreateExportPath(&export_path, &mesh_index_directory_path, &file_name);
		ofstream.open(export_path.c_str());
		ofstream << "\n[MeshData]\n" << std::endl;

		int index_num = md_bin_data_->getpMesh(i)->getIndexArraySize();
		ofstream << "�C���f�b�N�X�� : " << index_num << "\n" << std::endl;
		for (int j = 0; j < index_num; j += 3)
		{
			ofstream << "�C���f�b�N�X�ԍ�" << j << " : "
				<< "{ " << *md_bin_data_->getpMesh(i)->getpIndex(j)
				<< ", " << *md_bin_data_->getpMesh(i)->getpIndex(j + 1)
				<< ", " << *md_bin_data_->getpMesh(i)->getpIndex(j + 2)
				<< " }" << std::endl;
		}

		ofstream.close();


		// ���_
		file_name = "04_���_.txt";
		CreateExportPath(&export_path, &mesh_index_directory_path, &file_name);
		ofstream.open(export_path.c_str());
		ofstream << "\n[MeshData]\n" << std::endl;

		int vertex_num = md_bin_data_->getpMesh(i)->getVertexArraySize();
		ofstream << "���_�� : " << vertex_num << "\n" << std::endl;
		for (int j = 0; j < vertex_num; j++)
		{
			ofstream << "���_�ԍ�" << j << " : "
				<< "{ " << *md_bin_data_->getpMesh(i)->getpPosition(j)->getpX()
				<< ", " << *md_bin_data_->getpMesh(i)->getpPosition(j)->getpY()
				<< ", " << *md_bin_data_->getpMesh(i)->getpPosition(j)->getpZ()
				<< " }" << std::endl;
		}

		ofstream.close();

		// �@��
		file_name = "05_�@��.txt";
		CreateExportPath(&export_path, &mesh_index_directory_path, &file_name);
		ofstream.open(export_path.c_str());
		ofstream << "\n[MeshData]\n" << std::endl;

		int normal_num = md_bin_data_->getpMesh(i)->getNormalArraySize();
		ofstream << "�@���� : " << normal_num << "\n" << std::endl;

		for (int j = 0; j < normal_num; j++)
		{
			ofstream << "�@���ԍ�" << j << " : "
				<< "{ " << *md_bin_data_->getpMesh(i)->getpNormal(j)->getpX()
				<< ", " << *md_bin_data_->getpMesh(i)->getpNormal(j)->getpY()
				<< ", " << *md_bin_data_->getpMesh(i)->getpNormal(j)->getpZ()
				<< " }" << std::endl;
		}

		ofstream.close();

		// �{�[���̏d��
		file_name = "06_�{�[���̏d��.txt";
		CreateExportPath(&export_path, &mesh_index_directory_path, &file_name);
		ofstream.open(export_path.c_str());
		ofstream << "\n[MeshData]\n" << std::endl;

		int bone_weight_num = md_bin_data_->getpMesh(i)->getBoneWeightArraySize();
		ofstream << "�{�[���̏d�ݐ� : " << bone_weight_num << "\n" << std::endl;

		for (int j = 0; j < bone_weight_num; j++)
		{
			ofstream << "�{�[���̏d�ݒ��_�ԍ�" << j << " : ";
			for (int k = 0; k < MdBinData::Mesh::BoneWeight::MAX_BONE_NUM; k++)
			{
				ofstream << "{ " << md_bin_data_->getpMesh(i)->getpBoneWeight(j)->getBoneIndex(k)
					<< ", " << md_bin_data_->getpMesh(i)->getpBoneWeight(j)->getBoneWeight(k)
					<< " } ";
			}
			ofstream << std::endl;
		}

		ofstream.close();

		// �֘A�t�����}�e���A���ԍ�
		file_name = "07_�֘A�t�����}�e���A���ԍ�.txt";
		CreateExportPath(&export_path, &mesh_index_directory_path, &file_name);
		ofstream.open(export_path.c_str());
		ofstream << "\n[MeshData]\n" << std::endl;

		int material_index_num = *md_bin_data_->getpMesh(i)->getpMaterialIndex();
		ofstream << "�֘A�t�����}�e���A���ԍ� : " << material_index_num << std::endl;

		ofstream.close();

		// UVSet�Ɗ֘A�t�����e�N�X�`����
		file_name = "08_UVSet�Ɗ֘A�t�����e�N�X�`����.txt";
		CreateExportPath(&export_path, &mesh_index_directory_path, &file_name);
		ofstream.open(export_path.c_str());
		ofstream << "\n[MeshData]\n" << std::endl;

		if (md_bin_data_->getpMesh(i)->getUVSetArraySize() > 0)
		{
			for (int j = 0; j < md_bin_data_->getpMesh(i)->getpUVSet(0)->getTextureArraySize(); j++)
			{
				ofstream << "UVSet�Ɗ֘A�t����Texture�� : " << *md_bin_data_->getpMesh(i)->getpUVSet(0)->getpTexture(j)->getpFilePath() << std::endl;
			}
		}

		ofstream.close();
	}
}



void ExportFile::CreateFileName(std::string* file_path)
{
	// �t�@�C�����𒊏o(�p�X��؂�"\"��"/"�̗����ɑΉ�)
	auto temp_index = file_path->find_last_of("/");	// �Ō�̃p�X��؂�̃C���f�b�N�X���擾
	if (temp_index != std::string::npos)
	{
		// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
		file_name_ = file_path->substr(temp_index + 1);
	}
	else
	{
		temp_index = file_path->find_last_of("\\");
		if (temp_index != std::string::npos)
		{
			// �t�@�C����(�Ō�̃p�X��؂肩����)�𒊏o
			file_name_ = file_path->substr(temp_index + 1);
		}
		else
		{
			// �t�@�C�����𒊏o
			file_name_ = *file_path;
		}
	}

	temp_index = file_name_.find_last_of(".");
	if (temp_index != std::string::npos)
	{
		// �t�@�C��������g���q������
		file_name_ = file_name_.substr(0, temp_index);
	}
}



void ExportFile::CreateExportDirectoryPath()
{
	export_directory_path_ = (axis_ == FbxAxisSystem::DirectX
							  ? "MdBin\\mdbin_l\\" : "MdBin\\mdbin_r\\");
	export_directory_path_ += file_name_ + "\\";
}



void ExportFile::CreateLogDirectoryPath(std::string* material_directory_path,
										std::string* mesh_directory_path)
{
	*material_directory_path += export_directory_path_ + "Log\\Material\\";
	*mesh_directory_path += export_directory_path_ + "Log\\Mesh\\";
}



void ExportFile::CreateIndexDirectoryPath(std::string* index_directory_path,
										  std::string* directory_path,
										  int index)
{
	*index_directory_path = *directory_path + std::to_string(index) + "\\";
}



void ExportFile::CreateExportDirectory(std::string *directory_path)
{
	// �o�̓t�H���_�̍쐬
	if (PathIsDirectory(directory_path->c_str())) return;
	MakeSureDirectoryPathExists(directory_path->c_str());
}



void ExportFile::CreateExportFileName(std::string* file_name)
{
	*file_name += (axis_ == FbxAxisSystem::DirectX
				   ? ".mdbin_l" : ".mdbin_r");
}



void ExportFile::CreateIndexLogFileName(std::string* file_name,
										int index)
{
	*file_name += std::to_string(index) + ".txt";
}



void ExportFile::CreateExportPath(std::string* export_file_path,
								  std::string* directory_path,
								  std::string* export_file_name)
{
	*export_file_path = *directory_path + *export_file_name;
}