// stdafx.cpp : 僅包含標準 Include 檔的原始程式檔
// Mask.pch 會成為先行編譯標頭檔
// stdafx.obj 會包含先行編譯型別資訊

#include "stdafx.h"
#define _CRT_SECURE_NO_DEPRECATE
// TODO: 在 STDAFX.H 中參考您需要的任何其他標頭，
// 而不要在這個檔案中參考
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