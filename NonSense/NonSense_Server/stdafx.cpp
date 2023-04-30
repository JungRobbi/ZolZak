#include "stdafx.h"

UINT CBVSRVDescriptorSize = 0;
UINT RTVDescriptorSize = 0;

UINT gnRtvDescriptorIncrementSize = 0;
UINT gnDsvDescriptorIncrementSize = 0;

FLOAT ClearColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
bool DebugMode = false;
bool OptionMode = false;
UINT OBJNum = 2;