//================================================================================
//!	@file	 FbxConverter.h
//!	@brief	 FBX�ϊ���Class
//! @details 
//!	@author  Kai Araki									@date 2018/12/29
//================================================================================
#ifndef	_FBX_CONVERTER_H_
#define _FBX_CONVERTER_H_



//****************************************
// �C���N���[�h��
//****************************************
#include <fbxsdk.h>
#include <string>

#include "../MdBinDataContainer/MdBinDataContainer.h"



//************************************************************														   
//! @brief   FBX�ϊ���Class
//!
//! @details FBX�ϊ���Class
//************************************************************
class FbxConverter
{
//====================
// �ϐ�
//====================
private:
	FbxManager* manager_ = nullptr;				//!< FBX�}�l�[�W��
	FbxIOSettings* io_settings_ = nullptr;		//!< IO�Z�b�e�B���O
	FbxScene* scene_ = nullptr;					//!< �V�[��
	FbxNode* root_node_ = nullptr;				//!< ���[�g�m�[�h
	FbxAxisSystem axis_system_;					//!< ���W�n�̎��
	bool is_reverse_alpha_ = false;				//!< ���l�̔��]�t���O
	std::string directory_path_;				//!< �f�B���N�g���p�X
	MdBinDataContainer md_bin_data_container_;	//!< MdBin�f�[�^�̓��ꕨ
	FbxTime period_;							//!< 1�t���[���̎���
	int animation_start_frame_num_ = 0;			//!< �A�j���[�V�����J�n�t���[����
	int animation_stop_frame_num_ = 0;			//!< �A�j���[�V������~�t���[����
	int all_animation_frame_num_ = -1;			//!< �A�j���[�V�����t���[����


//====================
// �֐�
//====================
public:
	//----------------------------------------
	//! @brief �������֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval bool �����������̗L��
	//----------------------------------------
	bool Init();

	//----------------------------------------
	//! @brief �I���֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval void �Ȃ�
	//----------------------------------------
	void Uninit();

	//----------------------------------------
	//! @brief MdBin�֕ϊ��֐�
	//! @details
	//! @param *file_path �t�@�C���p�X
	//! @retval bool �ϊ������̗L��
	//----------------------------------------
	bool ConvertToMdBin(std::string* file_path);

private:
	//----------------------------------------
	//! @brief �ǂݍ��݊֐�
	//! @details
	//! @param *file_path �t�@�C���p�X
	//! @retval bool �ǂݍ��ݐ����̗L��
	//----------------------------------------
	bool Load(std::string* file_path);

	//----------------------------------------
	//! @brief �t���[����񒊏o�֐�
	//! @details
	//! @param *importer �C���|�[�^�[
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractFrameData(FbxImporter* importer);

	//----------------------------------------
	//! @brief �f�[�^���o�֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval bool �f�[�^�̒��o�����̗L��
	//----------------------------------------
	bool ExtractData();

	//----------------------------------------
	//! @brief �}�e���A���f�[�^�̒��o�֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractMaterialData();

	//----------------------------------------
	//! @brief ���b�V���f�[�^�̒��o�֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractMeshData();

	//----------------------------------------
	//! @brief �}�e���A�����̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractMaterialName(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �A���r�G���g�f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractAmbientData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �f�B�t���[�Y�f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractDiffuseData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �G�~�b�V�u�f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractEmissiveData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �o���v�f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractBumpData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �g�����X�y�A�����g�f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractTransparentData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �X�y�L�����f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractSpecularData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �p���[(�V���C�l�X)�f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractPowerData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief ���t���N�V�����f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *material       �}�e���A��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractReflectionData(int material_index, FbxSurfaceMaterial* material);

	//----------------------------------------
	//! @brief �e�N�X�`���f�[�^�̒��o�֐�
	//! @details
	//! @param *material_index �}�e���A���̃C���f�b�N�X
	//! @param *property       FBX�v���p�e�B
	//! @param texture_type    �e�N�X�`���^�C�v
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractTextureData(int material_index, FbxProperty property,
							MdBinDataContainer::Material::Texture::Type texture_type);

	//----------------------------------------
	//! @brief �C���f�b�N�X�f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *mesh       ���b�V��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractIndexData(int mesh_index, FbxMesh* mesh);

	//----------------------------------------
	//! @brief ���_�f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *mesh       ���b�V��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractVertexData(int mesh_index, FbxMesh* mesh);

	//----------------------------------------
	//! @brief �@���f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *mesh       ���b�V��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractNormalData(int mesh_index, FbxMesh* mesh);

	//----------------------------------------
	//! @brief UV�Z�b�g�f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *mesh       ���b�V��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractUVSetData(int mesh_index, FbxMesh* mesh);

	//----------------------------------------
	//! @brief �A�j���[�V�����f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *mesh       ���b�V��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractAnimationData(int mesh_index, FbxMesh* mesh);

	//----------------------------------------
	//! @brief �{�[���f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *cluster    �N���X�^
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractBoneData(int mesh_index, FbxCluster* cluster);

	//----------------------------------------
	//! @brief �{�[���̏d�݃f�[�^�̒��o�֐�
	//! @details
	//! @param *mesh_index             ���b�V���̃C���f�b�N�X
	//! @param *save_bone_weight_array �{�[���̏d�ݕۑ��p�z��
	//! @param *cluster                �N���X�^
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractBoneWeightData(int mesh_index,
							   std::vector<MdBinDataContainer::Mesh::BoneWeight>* save_bone_weight_array,
							   FbxCluster* cluster);

	//----------------------------------------
	//! @brief �s��ϊ��֐�
	//! @details
	//! @param *bone_matrix �{�[���s��
	//! @param *fbx_matrix  FBX�s��
	//! @retval void �Ȃ�
	//----------------------------------------
	void ChangeMatrix(MdBinDataContainer::Matrix* bone_matrix,
					  FbxAMatrix* fbx_matrix);

	//----------------------------------------
	//! @brief �}�e���A���f�[�^�Ƃ̊֘A�t���֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @param *mesh       ���b�V��
	//! @retval void �Ȃ�
	//----------------------------------------
	void AssociateWithMaterialData(int mesh_index, FbxMesh* mesh);

	//----------------------------------------
	//! @brief UV�Z�b�g�f�[�^�ƃe�N�X�`���̊֘A�t���֐�
	//! @details
	//! @param *mesh_index ���b�V���̃C���f�b�N�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void AssociatingUVSetDataAndTexture(int mesh_index);

	//----------------------------------------
	//! @brief MdBin�t�@�C���̏o�͊֐�
	//! @details
	//! @param *file_path �t�@�C���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	bool ExportOfMdBinFile(std::string* file_path);

	//----------------------------------------
	//! @brief �o�̓t�H���_�̍쐬�֐�
	//! @details
	//! @param *directory_path �f�B���N�g���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportDirectory(std::string* directory_path);

	//----------------------------------------
	//! @brief �t�@�C�������o�֐�
	//! @details
	//! @param *export_file_name �o�̓t�@�C����
	//! @param *file_path        �t�@�C���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtractFileName(std::string* export_file_name, std::string* file_path);

	//----------------------------------------
	//! @brief �g���q�����֐�
	//! @details
	//! @param *file_name �t�@�C����
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExtensionRemoval(std::string* file_name);

	//----------------------------------------
	//! @brief �o�̓f�B���N�g�����쐬�֐�
	//! @details
	//! @param *directory_path �f�B���N�g���p�X
	//! @param *file_name      �t�@�C����
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportDirectoryPath(std::string* directory_path,
								   std::string* file_name);

	//----------------------------------------
	//! @brief �o�̓t�@�C�����쐬�֐�
	//! @details
	//! @param *file_name           �t�@�C����
	//! @param *text_data_file_name �e�L�X�g�f�[�^�̃t�@�C����
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportFileName(std::string* file_name,
							  std::string* text_data_file_name);

	//----------------------------------------
	//! @brief �o�̓p�X�쐬�֐�
	//! @details
	//! @param *export_file_path          �o�̓p�X
	//! @param *export_txt_data_file_path �e�L�X�g�f�[�^�̏o�̓p�X
	//! @param *file_name                 �t�@�C����
	//! @param *text_data_file_name       �e�L�X�g�f�[�^�̃t�@�C����
	//! @param *directory_path            �f�B���N�g���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void CreateExportPath(std::string* export_file_path,
						  std::string* export_txt_data_file_path,
						  std::string* file_name,
						  std::string* text_data_file_name,
						  std::string* directory_path);

	//----------------------------------------
	//! @brief �e�L�X�g�f�[�^�o�͊֐�
	//! @details
	//! @param *export_txt_data_file_path �e�L�X�g�f�[�^�̏o�̓p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void ExportTextData(std::string* export_txt_data_file_path);


	//----------------------------------------
	//! @brief UTF8�̃t���p�X�֕ϊ��֐�
	//! @details
	//! @param *file_path �t�@�C���p�X
	//! @retval void �Ȃ�
	//----------------------------------------
	void ConvertToFullPathOfUTF8(std::string* file_path);
};



#endif