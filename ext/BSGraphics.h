#pragma once

#include "AddressLibrary.h"

#include <skse64/skse64/NiTextures.h>

// commonlib
namespace BSGraphics
{
	class State
	{
	public:
		inline static State* GetSingleton()
		{
			return Singleton_Addr;
		}

		// members
		std::uint64_t unk00;                            // 000
		std::uint64_t unk08;                            // 008
		std::uint64_t unk10;                            // 010
		std::uint64_t unk18;                            // 018
		std::uint32_t unk20;                            // 020
		std::uint32_t screenWidth;                      // 024
		std::uint32_t screenHeight;                     // 028
		std::uint32_t frameBufferViewport[2];           // 02C
		std::uint32_t unk34;                            // 034
		std::uint64_t unk38;                            // 038
		std::uint64_t unk40;                            // 040
		std::uint64_t unk48;                            // 048
		bool insideFrame;                               // 050
		bool letterbox;                                 // 051
		std::uint16_t unk52;                            // 052
		std::uint32_t unk54;                            // 054
		NiPointer<NiTexture> unk058;                    // 058 - black?
		NiPointer<NiTexture> defaultTextureWhite;       // 060
		NiPointer<NiTexture> defaultTextureGrey;        // 068
		NiPointer<NiTexture> defaultHeightMap;          // 070
		NiPointer<NiTexture> defaultReflectionCubeMap;  // 078
		NiPointer<NiTexture> defaultFaceDetailMap;      // 080
		NiPointer<NiTexture> defaultTexEffectMap;       // 088
		NiPointer<NiTexture> defaultTextureNormalMap;   // 090

	private:
		inline static const auto Singleton_Addr = IAL::Addr<State*>(524998, 411479);
	};
	static_assert(offsetof(State, screenWidth) == 0x24);
	static_assert(offsetof(State, frameBufferViewport) == 0x2C);
	static_assert(offsetof(State, letterbox) == 0x51);
}