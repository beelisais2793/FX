#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Math
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct FastTanhf : public FunctionProcessor  {
		struct TanhfTable : Table {
			TanhfTable(int n) : Table(n) {}
			void _generate() override;
		};
		struct StaticTanhfTable : StaticTable<TanhfTable, 11> {};
		const Table& _table;

		FastTanhf() : 
		FunctionProcessor(),
		_table(StaticTanhfTable::table())	{
		}

		float value(float radians);

		double Tick(double I, double A=1, double X=0, double Y=0) {
			return value(M_PI*I);
		}
	};

	void FastTanhf::TanhfTable::_generate() {
		_table[0] = -1.0f;
		_table[_length - 1] = 1.0f;
		for (int i = 1, n = _length - 1; i < n; ++i) {
			_table[i] = tanhf((((i / (float)_length) * 2.0f) - 1.0f) * M_PI);
		}
	}

	float FastTanhf::value(float radians) {
		if (radians <= -M_PI) {
			return -1.0f;
		}
		if (radians >= M_PI) {
			return 1.0f;
		}
		return _table.value(((radians + M_PI) / (2.0f * M_PI)) * _table.length());
	}
}