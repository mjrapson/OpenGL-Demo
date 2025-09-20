// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson


extern "C" {
#ifdef _WIN32
_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#else
__attribute__((visibility("default"))) unsigned long NvOptimusEnablement = 0x00000001;
__attribute__((visibility("default"))) int AmdPowerXpressRequestHighPerformance = 1;
#endif
}

int main(int /* argc */, char** /* argv */)
{
    return 0;
}