#include "Core/Audio/Decoder.hpp"
#include "Core/Audio/Decoder_ma.hpp"

namespace Core::Audio
{
	bool IDecoder::create(StringView path, IDecoder** pp_decoder)
	{
		ScopeObject<IDecoder> p_decoder;

		try
		{
			p_decoder = new Decoder_ma(path);
			*pp_decoder = p_decoder.get();
			return true;
		}
		catch (std::exception const& e)
		{
			spdlog::error("[core] WAV: {}", e.what());
		}

		*pp_decoder = nullptr;
		return false;
	}
}
