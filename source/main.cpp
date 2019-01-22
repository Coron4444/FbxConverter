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
#include <iostream>

#include "FbxConverter/FbxConverter.h"



//--------------------------------------------------
// -�A�v���P�[�V�����̃G���g���[�֐�
//--------------------------------------------------
int main(void)
{
	// ���������[�N���o
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	
	// ������
	FbxConverter fbx_converter;
	if (!fbx_converter.Init())
	{
		fbx_converter.Uninit();
		std::cout << "���������Ɏ��s" << std::endl;
		rewind(stdin);
		getchar();
		return -1;
	}
	
	// �ϊ�
	std::string file_path;
	std::cout << "FBX�t�@�C���p�X�̓���(Drag&Drop����)\n��";
	std::cin >> file_path;
	if (!fbx_converter.ConvertToMdBin(&file_path))
	{
		std::cout << "��FBX�̕ϊ��Ɏ��s" << std::endl;
	}
	
	// �I��
	fbx_converter.Uninit();

	rewind(stdin);
	getchar();

	return 0;
}