//================================================================================
//!	@file	 ExportFile.h
//!	@brief	 �t�@�C���o��Class
//! @details License : MIT
//!	@author  Kai Araki									@date 2019/01/22
//================================================================================
#ifndef	_EXPORT_FILE_H_
#define _EXPORT_FILE_H_



//****************************************
// �C���N���[�h��
//****************************************
#include <fbxsdk.h>
#include <string>

#include "../MdBinData/MdBinData.h"



//************************************************************														   
//! @brief   �t�@�C���o��Class
//!
//! @details �t�@�C���o�͗pClass
//************************************************************
class ExportFile
{
//====================
// �ϐ�
//====================
private:
	MdBinData* md_bin_data_;				//!< MdBin�f�[�^
	FbxAxisSystem axis_;					//!< ��
	std::string file_name_;					//!< �o�̓f�B���N�g���p�X
	std::string export_directory_path_;		//!< �o�̓f�B���N�g���p�X


//====================
// �v���p�e�B
//====================
public:
	//----------------------------------------
	//! @brief MdBin�f�[�^�ݒ�֐�
	//! @details
	//! @param *md_bin_data MdBin�f�[�^
	//! @retval void �Ȃ�
	//----------------------------------------
	void setMdBinData(MdBinData* md_bin_data);

	//----------------------------------------
	//! @brief ���ݒ�֐�
	//! @details
	//! @param axis ��
	//! @retval void �Ȃ�
	//----------------------------------------
	void setAxis(FbxAxisSystem axis);


//====================
// �֐�
//====================
public:
	//----------------------------------------
	//! @brief �o�͊֐�
	//! @details
	//! @param *file_path �t�@�C���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void Export(std::string* file_path);

private:
	//----------------------------------------
	//! @brief MdBin�t�@�C���̏o�͊֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval bool �㏑���̗L��
	//----------------------------------------
	bool ExportOfMdBinFile();

	//----------------------------------------
	//! @brief ���O�t�@�C���̏o�͊֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExportOfLogFile();

	//----------------------------------------
	//! @brief �}�e���A�����O�t�@�C���̏o�͊֐�
	//! @details
	//! @param *directory_path �f�B���N�g���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExportOfMaterialLogFile(std::string* directory_path);

	//----------------------------------------
	//! @brief ���b�V�����O�t�@�C���̏o�͊֐�
	//! @details
	//! @param *directory_path �f�B���N�g���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExportOfMeshLogFile(std::string* directory_path);

	//----------------------------------------
	//! @brief �t�@�C�����쐬�֐�
	//! @details
	//! @param *file_path �t�@�C���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateFileName(std::string* file_path);

	//----------------------------------------
	//! @brief �o�̓f�B���N�g�����쐬�֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportDirectoryPath();

	//----------------------------------------
	//! @brief ���O�f�B���N�g�����쐬�֐�
	//! @details
	//! @param *material_directory_path �}�e���A���f�B���N�g���p�X
	//! @param *mesh_directory_path     ���b�V���f�B���N�g���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateLogDirectoryPath(std::string* material_directory_path,
								std::string* mesh_directory_path);

	//----------------------------------------
	//! @brief �C���f�b�N�X�f�B���N�g�����쐬�֐�
	//! @details
	//! @param *index_directory_path �C���f�b�N�X�f�B���N�g���p�X
	//! @param *directory_path       �f�B���N�g���p�X
	//! @param index				 �C���f�b�N�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateIndexDirectoryPath(std::string* index_directory_path,
								  std::string* directory_path,
								  int index);

	//----------------------------------------
	//! @brief �o�̓f�B���N�g���̍쐬�֐�
	//! @details
	//! @param *directory_path �f�B���N�g���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportDirectory(std::string* directory_path);

	//----------------------------------------
	//! @brief �o�̓t�@�C�����쐬�֐�
	//! @details
	//! @param *file_name �t�@�C����
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportFileName(std::string* file_name);

	//----------------------------------------
	//! @brief �C���f�b�N�X���O�t�@�C�����쐬�֐�
	//! @details
	//! @param *file_name �t�@�C����
	//! @param index	  �C���f�b�N�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateIndexLogFileName(std::string* file_name,
								int index);

	//----------------------------------------
	//! @brief �o�̓p�X�쐬�֐�
	//! @details
	//! @param *export_file_path �o�̓p�X
	//! @param *directory_path   �f�B���N�g���p�X
	//! @param *export_file_name �o�̓t�@�C����
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportPath(std::string* export_file_path,
						  std::string* directory_path,
						  std::string* export_file_name);
};



#endif