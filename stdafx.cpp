// stdafx.cpp : �ȥ]�t�з� Include �ɪ���l�{����
// Mask.pch �|��������sĶ���Y��
// stdafx.obj �|�]�t����sĶ���O��T

#include "stdafx.h"
#define _CRT_SECURE_NO_DEPRECATE
// TODO: �b STDAFX.H ���Ѧұz�ݭn�������L���Y�A
// �Ӥ��n�b�o���ɮפ��Ѧ�
#pragma once
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif