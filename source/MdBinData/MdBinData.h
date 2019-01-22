//================================================================================
//!	@file	 MdBinData.h
//!	@brief	 �o�C�i���[���f���f�[�^Class
//! @details 
//!	@author  Kai Araki									@date 2018/12/29
//================================================================================
#ifndef	_MD_BIN_DATA_H_
#define _MD_BIN_DATA_H_



//****************************************
// �C���N���[�h��
//****************************************
#include <stdio.h>
#include <vector>
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>



//************************************************************														   
//! @brief   �o�C�i���[���f���f�[�^Class
//!
//! @details �o�C�i���[���f���̃f�[�^Class
//************************************************************
class MdBinData
{
//====================
// �N���X��`
//====================
public:
	//****************************************											   
	//! @brief   �x�N�^�[2Class
	//!
	//! @details float�ϐ���2����Class
	//****************************************
	class Vector2
	{
	//====================
	// �ϐ�
	//====================
	private:
		float x_;		//!< X����
		float y_;		//!< Y����


	//====================
	// �v���p�e�B
	//====================
	public:
		//----------------------------------------
		//! @brief X�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* X����
		//----------------------------------------
		float* getpX();

		//----------------------------------------
		//! @brief Y�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* Y����
		//----------------------------------------
		float* getpY();

	//====================
	// �V���A���C�Y
	//====================
	private:
		//----------------------------------------
		//! @brief �V���A���C�Y(I/O)�֐�
		//! @details
		//! @param archive �A�[�J�C�u�p�N���X
		//! @param version �o�[�W����
		//! @retval void �Ȃ�
		//----------------------------------------
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& archive, const unsigned version)
		{
			if (version) {}

			archive & x_;
			archive & y_;
		}
	};


	//****************************************											   
	//! @brief   �x�N�^�[3Class
	//!
	//! @details float�ϐ���3����Class
	//****************************************
	class Vector3
	{
	//====================
	// �ϐ�
	//====================
	private:
		Vector2 vector2_;	//!< �x�N�^�[2
		float z_;			//!< Z����


	//====================
	// �v���p�e�B
	//====================
	public:
		//----------------------------------------
		//! @brief X�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* X����
		//----------------------------------------
		float* getpX();

		//----------------------------------------
		//! @brief Y�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* Y����
		//----------------------------------------
		float* getpY();

		//----------------------------------------
		//! @brief Z�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* Z����
		//----------------------------------------
		float* getpZ();


	//====================
	// �V���A���C�Y
	//====================
	private:
		//----------------------------------------
		//! @brief �V���A���C�Y(I/O)�֐�
		//! @details
		//! @param archive �A�[�J�C�u�p�N���X
		//! @param version �o�[�W����
		//! @retval void �Ȃ�
		//----------------------------------------
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& archive, const unsigned version)
		{
			if (version) {}

			archive & vector2_;
			archive & z_;
		}
	};


	//****************************************											   
	//! @brief   �FClass
	//!
	//! @details �F��Class
	//****************************************
	class Color
	{
	//====================
	// �ϐ�
	//====================
	private:
		float r_;	//!< �Ԑ���
		float g_;	//!< �ΐ���
		float b_;	//!< ����


	//====================
	// �v���p�e�B
	//====================
	public:
		//----------------------------------------
		//! @brief �Ԑ����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* �Ԑ���
		//----------------------------------------
		float* getpR();

		//----------------------------------------
		//! @brief �ΐ����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* �ΐ���
		//----------------------------------------
		float* getpG();

		//----------------------------------------
		//! @brief �����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* ����
		//----------------------------------------
		float* getpB();


	//====================
	// �V���A���C�Y
	//====================
	private:
		//----------------------------------------
		//! @brief �V���A���C�Y(I/O)�֐�
		//! @details
		//! @param archive �A�[�J�C�u�p�N���X
		//! @param version �o�[�W����
		//! @retval void �Ȃ�
		//----------------------------------------
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& archive, const unsigned version)
		{
			if (version) {}

			archive & r_;
			archive & g_;
			archive & b_;
		}
	};


	//****************************************											   
	//! @brief   �s��Class
	//!
	//! @details �s���Class
	//****************************************
	class Matrix
	{
	//====================
	// �萔
	//====================
	public:
		static const unsigned ARRAY_WIDTH = 4;
		static const unsigned ARRAY_HEIGHT = 4;


	//====================
	// �ϐ�
	//====================
	private:
		float array_[ARRAY_HEIGHT][ARRAY_WIDTH];	//!< 4x4�s��


	//====================
	// �v���p�e�B
	//====================
	public:
		//----------------------------------------
		//! @brief �z��v�f�擾�֐�
		//! @details
		//! @param index0 0�Ԗڂ̃C���f�b�N�X
		//! @param index1 1�Ԗڂ̃C���f�b�N�X
		//! @retval float �z��v�f
		//----------------------------------------
		float getMatrixElement(unsigned index0, unsigned index1);

		//----------------------------------------
		//! @brief �z��v�f�ݒ�֐�
		//! @details
		//! @param value  �l
		//! @param index0 0�Ԗڂ̃C���f�b�N�X
		//! @param index1 1�Ԗڂ̃C���f�b�N�X
		//! @retval void �Ȃ�
		//----------------------------------------
		void setMatrixElement(float value, unsigned index0, unsigned index1);


	//====================
	// �V���A���C�Y
	//====================
	private:
		//----------------------------------------
		//! @brief �V���A���C�Y(I/O)�֐�
		//! @details
		//! @param archive �A�[�J�C�u�p�N���X
		//! @param version �o�[�W����
		//! @retval void �Ȃ�
		//----------------------------------------
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& archive, const unsigned version)
		{
			if (version) {}

			archive & array_;
		}
	};


	//****************************************											   
	//! @brief   �}�e���A��Class
	//!
	//! @details �}�e���A����Class
	//****************************************
	class Material
	{
	//====================
	// �N���X��`
	//====================
	public:
		//****************************************											   
		//! @brief   �e�N�X�`��Class
		//!
		//! @details �e�N�X�`���̎�ނ��Ƃ�Class
		//****************************************
		class Texture
		{
		//====================
		// �񋓌^��`
		//====================
		public:
			enum class Type
			{
				NONE,
				AMBIENT,
				DIFFUSE,
				EMISSIVE,
				NORMAL,
				SPECULAR,
				MAX
			};


		//====================
		// �ϐ�
		//====================
		private:
			std::string file_path_;		//!< �t�@�C���p�X
			Type type_;					//!< �e�N�X�`���^�C�v


		//====================
		// �v���p�e�B
		//====================
		public:
			//----------------------------------------
			//! @brief �t�@�C���p�X�擾�֐�
			//! @details
			//! @param void �Ȃ�
			//! @retval std::string* �t�@�C���p�X
			//----------------------------------------
			std::string* getpFilePath();

			//----------------------------------------
			//! @brief �e�N�X�`���^�C�v�擾�֐�
			//! @details
			//! @param void �Ȃ�
			//! @retval Type* �e�N�X�`���^�C�v
			//----------------------------------------
			Type* getpType();


		//====================
		// �V���A���C�Y
		//====================
		private:
			//----------------------------------------
			//! @brief �V���A���C�Y(I/O)�֐�
			//! @details
			//! @param archive �A�[�J�C�u�p�N���X
			//! @param version �o�[�W����
			//! @retval void �Ȃ�
			//----------------------------------------
			friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& archive, const unsigned version)
			{
				if (version) {}

				archive & file_path_;
				archive & type_;
			}
		};


	//====================
	// �ϐ�
	//====================
	private:
		std::string name_;						//!< ���O
		Color ambient_;							//!< �A���r�G���g(����)
		Color diffuse_;							//!< �f�B�q���[�Y(�g�U���ˌ�)
		Color emissive_;						//!< �G�~�b�V�u(���ˌ�)
		Color bump_;							//!< �o���v(���ʋ)
		float transparent_;						//!< �g�����X�y�A�����g(���ߓx)
		Color specular_;						//!< �X�y�L����(���ʔ���)
		float power_;							//!< �p���[(�X�y�L�����̌������x)
		float reflection_;						//!< ���t���N�V����(���ˋ��x)
		std::vector<Texture*> texture_array_;	//!< �e�N�X�`���z��


	//====================
	// �v���p�e�B
	//====================
	public:
		//----------------------------------------
		//! @brief ���O�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval std::string* ���O
		//----------------------------------------
		std::string* getpName();

		//----------------------------------------
		//! @brief �A���r�G���g�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval Color* �A���r�G���g
		//----------------------------------------
		Color* getpAmbient();

		//----------------------------------------
		//! @brief �f�B�t���[�Y�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval Color* �f�B�t���[�Y
		//----------------------------------------
		Color* getpDiffuse();

		//----------------------------------------
		//! @brief �G�~�b�V�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval Color* �G�~�b�V����
		//----------------------------------------
		Color* getpEmissive();

		//----------------------------------------
		//! @brief �o���v�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval Color* �o���v
		//----------------------------------------
		Color* getpBump();

		//----------------------------------------
		//! @brief �g�����X�y�A�����g�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* �g�����X�y�A�����g
		//----------------------------------------
		float* getpTransparent();

		//----------------------------------------
		//! @brief �X�y�L�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval Color* �X�y�L�����[
		//----------------------------------------
		Color* getpSpecular();

		//----------------------------------------
		//! @brief �p���[�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* �p���[
		//----------------------------------------
		float* getpPower();

		//----------------------------------------
		//! @brief ���t���N�V�����擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval float* ���t���N�V����
		//----------------------------------------
		float* getpReflection();

		//----------------------------------------
		//! @brief �e�N�X�`���z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getTextureArraySize();

		//----------------------------------------
		//! @brief �e�N�X�`���擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval Texture �e�N�X�`��
		//----------------------------------------
		Texture* getpTexture(int index);


	//====================
	// �֐�
	//====================
	public:
		//----------------------------------------
		//! @brief �e�N�X�`������֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval void �Ȃ�
		//----------------------------------------
		void ReleaseTextureArray();

		//----------------------------------------
		//! @brief �e�N�X�`���z��ǉ��֐�
		//! @details
		//! @param *texture �e�N�X�`��
		//! @retval void �Ȃ�
		//----------------------------------------
		void AddTextureArray(Texture* texture);


	//====================
	// �V���A���C�Y
	//====================
	private:
		//----------------------------------------
		//! @brief �V���A���C�Y(I/O)�֐�
		//! @details
		//! @param archive �A�[�J�C�u�p�N���X
		//! @param version �o�[�W����
		//! @retval void �Ȃ�
		//----------------------------------------
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& archive, const unsigned version)
		{
			if (version) {}

			archive & name_;
			archive & ambient_;
			archive & diffuse_;
			archive & emissive_;
			archive & bump_;
			archive & transparent_;
			archive & specular_;
			archive & power_;
			archive & reflection_;
			archive & texture_array_;
		}
	};


	//****************************************											   
	//! @brief   ���b�V��Class
	//!
	//! @details ���b�V����Class
	//****************************************
	class Mesh
	{
	//====================
	// �N���X��`
	//====================
	public:
		//****************************************											   
		//! @brief   UV�Z�b�gClass
		//!
		//! @details �e�N�X�`���̎�ނ��Ƃ�UV��Class
		//****************************************
		class UVSet
		{
		//====================
		// �ϐ�
		//====================
		private:
			std::vector<Vector2> uv_array_;				//!< UV�z��
			std::vector<Material::Texture*> texture_;	//!< �e�N�X�`���z��


		//====================
		// �v���p�e�B
		//====================
		public:
			//----------------------------------------
			//! @brief UV�z��T�C�Y�擾�֐�
			//! @details
			//! @param void �Ȃ� 
			//! @retval int �T�C�Y
			//----------------------------------------
			int getUVArraySize();

			//----------------------------------------
			//! @brief UV�z��T�C�Y�ݒ�֐�
			//! @details
			//! @param value �T�C�Y 
			//! @retval void �Ȃ�
			//----------------------------------------
			void setUVArraySize(int value);

			//----------------------------------------
			//! @brief U�����擾�֐�
			//! @details
			//! @param index �C���f�b�N�X
			//! @retval float* U����
			//----------------------------------------
			float* getpU(int index);

			//----------------------------------------
			//! @brief V�����擾�֐�
			//! @details
			//! @param index �C���f�b�N�X
			//! @retval float* V����
			//----------------------------------------
			float* getpV(int index);

			//----------------------------------------
			//! @brief �e�N�X�`���z��T�C�Y�擾�֐�
			//! @details
			//! @param void �Ȃ� 
			//! @retval int �T�C�Y
			//----------------------------------------
			int getTextureArraySize();

			//----------------------------------------
			//! @brief �e�N�X�`���擾�֐�
			//! @details
			//! @param index �C���f�b�N�X
			//! @retval Texture �e�N�X�`��
			//----------------------------------------
			Material::Texture* getpTexture(int index);


		//====================
		// �֐�
		//====================
		public:
			//----------------------------------------
			//! @brief �e�N�X�`���z��ǉ��֐�
			//! @details
			//! @param *texture �e�N�X�`��
			//! @retval void �Ȃ�
			//----------------------------------------
			void AddTextureArray(Material::Texture* texture);


		//====================
		// �V���A���C�Y
		//====================
		private:
			//----------------------------------------
			//! @brief �V���A���C�Y(I/O)�֐�
			//! @details
			//! @param archive �A�[�J�C�u�p�N���X
			//! @param version �o�[�W����
			//! @retval void �Ȃ�
			//----------------------------------------
			friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& archive, const unsigned version)
			{
				if (version) {}

				archive & uv_array_;
				archive & texture_;
			}
		};


		//****************************************											   
		//! @brief   �{�[��Class
		//!
		//! @details �{�[����Class
		//****************************************
		class Bone
		{
		//====================
		// �ϐ�
		//====================
		private:
			std::string name_;								//!< �{�[����
			Matrix offset_matrix_;							//!< �I�t�Z�b�g�s��(�����p���̋t�s��)
			std::vector<Matrix> animation_matrix_array_;	//!< �A�j���[�V�����s��


		//====================
		// �v���p�e�B
		//====================
		public:
			//----------------------------------------
			//! @brief �{�[�����擾�֐�
			//! @details
			//! @param void �Ȃ�
			//! @retval std::string* �{�[����
			//----------------------------------------
			std::string* getpName();

			//----------------------------------------
			//! @brief �I�t�Z�b�g�s��擾�֐�
			//! @details
			//! @param void �Ȃ�
			//! @retval Matirx* �I�t�Z�b�g�z��
			//----------------------------------------
			Matrix* getpOffsetMatrix();

			//----------------------------------------
			//! @brief �A�j���[�V�����s��z��T�C�Y�擾�֐�
			//! @details
			//! @param void �Ȃ�
			//! @retval int �A�j���[�V�����s��z��T�C�Y
			//----------------------------------------
			int getAnimationMatrixArraySize();

			//----------------------------------------
			//! @brief �A�j���[�V�����s��z��T�C�Y�ݒ�֐�
			//! @details
			//! @param value �A�j���[�V�����s��z��T�C�Y
			//! @retval void �Ȃ�
			//----------------------------------------
			void setAnimationMatrixArraySize(int value);

			//----------------------------------------
			//! @brief �A�j���[�V�����s��擾�֐�
			//! @details
			//! @param index �C���f�b�N�X
			//! @retval Matrix* �A�j���[�V�����s��
			//----------------------------------------
			Matrix* getpAnimationMatrix(int index);


		//====================
		// �V���A���C�Y
		//====================
		private:
			//----------------------------------------
			//! @brief �V���A���C�Y(I/O)�֐�
			//! @details
			//! @param archive �A�[�J�C�u�p�N���X
			//! @param version �o�[�W����
			//! @retval void �Ȃ�
			//----------------------------------------
			friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& archive, const unsigned version)
			{
				if (version) {}

				archive & name_;
				archive & offset_matrix_;
				archive & animation_matrix_array_;
			}
		};


		//****************************************											   
		//! @brief   �{�[���d��Class
		//!
		//! @details �{�[���̏d��Class
		//****************************************
		class BoneWeight
		{
		//====================
		// �萔
		//====================
		public:
			static const unsigned MAX_BONE_NUM = 4;		//!< �ő�{�[����


		//====================
		// �ϐ�
		//====================
		private:
			int bone_index_[MAX_BONE_NUM];		//!< �{�[���C���f�b�N�X
			float bone_weight_[MAX_BONE_NUM];	//!< �{�[���̏d��


		//====================
		// �v���p�e�B
		//====================
		public:
			//----------------------------------------
			//! @brief �{�[���C���f�b�N�X�擾�֐�
			//! @details
			//! @param index �C���f�b�N�X
			//! @retval int �{�[���C���f�b�N�X
			//----------------------------------------
			int getBoneIndex(int index);

			//----------------------------------------
			//! @brief �{�[���̏d�ݎ擾�֐�
			//! @details
			//! @param index �C���f�b�N�X
			//! @retval float �{�[���̏d��
			//----------------------------------------
			float getBoneWeight(int index);

			//----------------------------------------
			//! @brief �{�[���C���f�b�N�X&�d�ݐݒ�֐�
			//! @details
			//! @param bone_index �{�[���C���f�b�N�X
			//! @param bone_weight �{�[���̏d��
			//! @retval int* �{�[���C���f�b�N�X
			//----------------------------------------
			void setBoneIndexAndWeight(int bone_index, float bone_weight);


		//====================
		// �֐�
		//====================
		public:
			//----------------------------------------
			//! @brief �������֐�
			//! @details
			//! @param void �Ȃ�
			//! @retval void �Ȃ�
			//----------------------------------------
			void Init();


		//====================
		// �V���A���C�Y
		//====================
		private:
			//----------------------------------------
			//! @brief �V���A���C�Y(I/O)�֐�
			//! @details
			//! @param archive �A�[�J�C�u�p�N���X
			//! @param version �o�[�W����
			//! @retval void �Ȃ�
			//----------------------------------------
			friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& archive, const unsigned version)
			{
				if (version) {}

				archive & bone_index_;
				archive & bone_weight_;
			}
		};


	//====================
	// �ϐ�
	//====================
	private:
		std::vector<Vector3> position_array_;		//!< ���W�z��
		std::vector<Vector3> normal_array_;			//!< �@���z��
		std::vector<UVSet> uv_set_array_;			//!< UV�Z�b�g�z��
		std::vector<int> index_array_;				//!< �C���f�b�N�X�z��
		std::vector<Bone> bone_array_;				//!< �{�[���z��
		std::vector<BoneWeight> bone_weight_array_;	//!< �{�[���̏d�ݔz��
		int material_index_;						//!< �}�e���A���C���f�b�N�X


	//====================
	// �v���p�e�B
	//====================
	public:
		//----------------------------------------
		//! @brief ���_�z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getVertexArraySize();

		//----------------------------------------
		//! @brief ���W�z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getPositionArraySize();

		//----------------------------------------
		//! @brief ���W�z��T�C�Y�ݒ�֐�
		//! @details
		//! @param value �T�C�Y 
		//! @retval void �Ȃ�
		//----------------------------------------
		void setPositionArraySize(int value);

		//----------------------------------------
		//! @brief ���W�擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval Vector3* ���_
		//----------------------------------------
		Vector3* getpPosition(int index);

		//----------------------------------------
		//! @brief �@���z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getNormalArraySize();

		//----------------------------------------
		//! @brief �@���z��T�C�Y�ݒ�֐�
		//! @details
		//! @param value �T�C�Y 
		//! @retval void �Ȃ�
		//----------------------------------------
		void setNormalArraySize(int value);

		//----------------------------------------
		//! @brief �@���擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval Vector3* �@��
		//----------------------------------------
		Vector3* getpNormal(int index);

		//----------------------------------------
		//! @brief UV�Z�b�g�z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getUVSetArraySize();

		//----------------------------------------
		//! @brief UV�Z�b�g�z��T�C�Y�ݒ�֐�
		//! @details
		//! @param value �T�C�Y 
		//! @retval void �Ȃ�
		//----------------------------------------
		void setUVSetArraySize(int value);

		//----------------------------------------
		//! @brief UV�Z�b�g�擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval UVSet* UV�Z�b�g
		//----------------------------------------
		UVSet* getpUVSet(int index);

		//----------------------------------------
		//! @brief �C���f�b�N�X�z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getIndexArraySize();

		//----------------------------------------
		//! @brief �C���f�b�N�X�z��T�C�Y�ݒ�֐�
		//! @details
		//! @param value �T�C�Y 
		//! @retval void �Ȃ�
		//----------------------------------------
		void setIndexArraySize(int value);

		//----------------------------------------
		//! @brief �C���f�b�N�X�擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval int �C���f�b�N�X
		//----------------------------------------
		int* getpIndex(int index);

		//----------------------------------------
		//! @brief �{�[���z��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getBoneArraySize();

		//----------------------------------------
		//! @brief �{�[���z��T�C�Y�ݒ�֐�
		//! @details
		//! @param value �T�C�Y 
		//! @retval void �Ȃ�
		//----------------------------------------
		void setBoneArraySize(int value);

		//----------------------------------------
		//! @brief �{�[���擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval Bone* �{�[��
		//----------------------------------------
		Bone* getpBone(int index);

		//----------------------------------------
		//! @brief �{�[���z�񖖔��C���f�b�N�X�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval int �{�[���z�񖖔��C���f�b�N�X
		//----------------------------------------
		int getBoneArrayEndIndex();

		//----------------------------------------
		//! @brief �{�[���̏d�ݔz��T�C�Y�擾�֐�
		//! @details
		//! @param void �Ȃ� 
		//! @retval int �T�C�Y
		//----------------------------------------
		int getBoneWeightArraySize();

		//----------------------------------------
		//! @brief �{�[���̏d�ݔz��T�C�Y�ݒ�֐�
		//! @details
		//! @param value �T�C�Y 
		//! @retval void �Ȃ�
		//----------------------------------------
		void setBoneWeightArraySize(int value);

		//----------------------------------------
		//! @brief �{�[���̏d�ݎ擾�֐�
		//! @details
		//! @param index �C���f�b�N�X
		//! @retval BoneWeight* �{�[���̏d��
		//----------------------------------------
		BoneWeight* getpBoneWeight(int index);

		//----------------------------------------
		//! @brief �}�e���A���C���f�b�N�X�擾�֐�
		//! @details
		//! @param void �Ȃ�
		//! @retval int* �}�e���A���C���f�b�N�X
		//----------------------------------------
		int* getpMaterialIndex();


	//====================
	// �֐�
	//====================
	public:
		//----------------------------------------
		//! @brief �{�[���z��ǉ��֐�
		//! @details
		//! @param bone �{�[��
		//! @retval void �Ȃ�
		//----------------------------------------
		void AddBoneArray(Bone* bone);


	//====================
	// �V���A���C�Y
	//====================
	private:
		//----------------------------------------
		//! @brief �V���A���C�Y(I/O)�֐�
		//! @details
		//! @param archive �A�[�J�C�u�p�N���X
		//! @param version �o�[�W����
		//! @retval void �Ȃ�
		//----------------------------------------
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& archive, const unsigned version)
		{
			if (version) {}

			archive & position_array_;
			archive & normal_array_;
			archive & uv_set_array_;
			archive & index_array_;
			archive & bone_array_;
			archive & bone_weight_array_;
			archive & material_index_;
		}
	};


//====================
// static�֐�
//====================
public:
	//----------------------------------------
	//! @brief �C���|�[�g�f�[�^�֐�
	//! @details
	//! @param *md_bin_data_container �C���|�[�g��
	//! @param file_path              �t�@�C���p�X
	//! @retval bool �ǂݍ��ݐ����̗L��
	//----------------------------------------
	static bool InportData(MdBinData* md_bin_data_container,
						   std::string file_path);

	//----------------------------------------
	//! @brief �G�N�X�|�[�g�f�[�^�֐�
	//! @details
	//! @param *md_bin_data_container �G�N�X�|�[�g��
	//! @param file_path              �t�@�C���p�X
	//! @retval bool �������ݐ����̗L��
	//----------------------------------------
	static bool ExportData(MdBinData* md_bin_data_container,
						   std::string file_path);


//====================
// �ϐ�
//====================
private:
	std::vector<Mesh> mesh_array_;			//!< ���b�V���z��
	std::vector<Material> material_array_;	//!< �}�e���A���z��
	int animation_frame_num_;				//!< �A�j���[�V�����t���[����


//====================
// �v���p�e�B
//====================
public:
	//----------------------------------------
	//! @brief ���b�V���z��T�C�Y�擾�֐�
	//! @details
	//! @param void �Ȃ� 
	//! @retval int �T�C�Y
	//----------------------------------------
	int getMeshArraySize();

	//----------------------------------------
	//! @brief ���b�V���z��T�C�Y�ݒ�֐�
	//! @details
	//! @param value �T�C�Y
	//! @retval void �Ȃ�
	//----------------------------------------
	void setMeshArraySize(int value);

	//----------------------------------------
	//! @brief ���b�V���擾�֐�
	//! @details
	//! @param index �C���f�b�N�X
	//! @retval Mesh* ���b�V��
	//----------------------------------------
	Mesh* getpMesh(int index);

	//----------------------------------------
	//! @brief �}�e���A���z��T�C�Y�擾�֐�
	//! @details
	//! @param void �Ȃ� 
	//! @retval int �T�C�Y
	//----------------------------------------
	int getMaterialArraySize();

	//----------------------------------------
	//! @brief �}�e���A���z��T�C�Y�ݒ�֐�
	//! @details
	//! @param value �T�C�Y
	//! @retval void �Ȃ�
	//----------------------------------------
	void setMaterialArraySize(int value);

	//----------------------------------------
	//! @brief �}�e���A���擾�֐�
	//! @details
	//! @param index �C���f�b�N�X
	//! @retval Material* �}�e���A��
	//----------------------------------------
	Material* getpMaterial(int index);

	//----------------------------------------
	//! @brief �A�j���[�V�����t���[�����擾�֐�
	//! @details
	//! @param void �Ȃ�
	//! @retval int �A�j���[�V�����t���[����
	//----------------------------------------
	int getAnimationFramNum();

	//----------------------------------------
	//! @brief �A�j���[�V�����t���[�����ݒ�֐�
	//! @details
	//! @param value �A�j���[�V�����t���[����
	//! @retval void �Ȃ�
	//----------------------------------------
	void setAnimationFramNum(int value);


//====================
// �֐�
//====================
public:
	//----------------------------------------
	//! @brief �f�X�g���N�^�֐�
	//! @details
	//! @param void �Ȃ�
	//----------------------------------------
	~MdBinData();


//====================
// �V���A���C�Y
//====================
private:
	//----------------------------------------
	//! @brief �V���A���C�Y(I/O)�֐�
	//! @details
	//! @param archive �A�[�J�C�u�p�N���X
	//! @param version �o�[�W����
	//! @retval void �Ȃ�
	//----------------------------------------
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& archive, const unsigned version)
	{
		if (version) {}

		archive & mesh_array_;
		archive & material_array_;
		archive & animation_frame_num_;
	}
};



#endif