#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Analyzer - needs libffft
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Window {
		int _size;
		float* _window;
		float _sum;

		Window(int size)
		: _size(size)
		, _window(new float[_size] {})
		, _sum(0.0)
		{}
		virtual ~Window() {
			delete[] _window;
		}

		inline int size() { return _size; }
		inline float at(int i) { assert(i >= 0 && i < _size); return _window[i]; }
		inline float sum() { return _sum; }
		void apply(float* in, float* out);
	};

	struct HanningWindow : Window {
		HanningWindow(int size, float alpha = 0.5);
	};

	struct HammingWindow : HanningWindow {
		HammingWindow(int size) : HanningWindow(size, 0.54) {}
	};

	struct KaiserWindow : Window {
		KaiserWindow(int size, float alpha = 7.865f);

		float i0(float x);
	};

	struct PlanckTaperWindow : Window {
		PlanckTaperWindow(int size, int taperSamples);
	};


	struct FFT1024 {
		void* _fft = NULL;
		FFT1024();
		~FFT1024();

		void do_fft(float* out, float* in);
	};

	struct FFT4096 {
		void* _fft = NULL;
		FFT4096();
		~FFT4096();

		void do_fft(float* out, float* in);
	};

	struct FFT8192 {
		void* _fft = NULL;
		FFT8192();
		~FFT8192();

		void do_fft(float* out, float* in);
	};

	struct FFT16384 {
		void* _fft = NULL;
		FFT16384();
		~FFT16384();

		void do_fft(float* out, float* in);
	};

	struct FFT32768 {
		void* _fft = NULL;
		FFT32768();
		~FFT32768();

		void do_fft(float* out, float* in);
	};


	struct SpectrumAnalyzer : OverlappingBuffer<float> {
		enum Size {
			SIZE_128 = 128,
			SIZE_256 = 256,
			SIZE_512 = 512,
			SIZE_1024 = 1024,
			SIZE_2048 = 2048,
			SIZE_4096 = 4096,
			SIZE_8192 = 8192,
			SIZE_16384 = 16384,
			SIZE_32768 = 32768
		};
		static constexpr Size maxSize = SIZE_32768;

		enum Overlap {
			OVERLAP_1 = 1,
			OVERLAP_2 = 2,
			OVERLAP_4 = 4,
			OVERLAP_8 = 8
		};

		enum WindowType {
			WINDOW_NONE,
			WINDOW_HANNING,
			WINDOW_HAMMING,
			WINDOW_KAISER
		};

		const float _sampleRate;
		ffft::FFTReal<float>* _fft = NULL;
		FFT1024* _fft1024 = NULL;
		FFT4096* _fft4096 = NULL;
		FFT8192* _fft8192 = NULL;
		FFT16384* _fft16384 = NULL;
		FFT32768* _fft32768 = NULL;
		Window* _window = NULL;
		float* _windowOut = NULL;
		float* _fftOut = NULL;

		SpectrumAnalyzer(
			Size size,
			Overlap overlap,
			WindowType windowType,
			float sampleRate,
			bool autoProcess = true
		);
		virtual ~SpectrumAnalyzer();

		void processBuffer(float* samples) override;
		void getMagnitudes(float* bins, int nBins);
	};


	void Table::generate() {
		if (!_table) {
			_table = new float[_length] {};
			_generate();
		}
	}


	void SineTable::_generate() {
		const float twoPI = 2.0f * M_PI;
		for (int i = 0, j = _length / 4; i <= j; ++i) {
			_table[i] = sinf(twoPI * (i / (float)_length));
		}
		for (int i = 1, j = _length / 4; i < j; ++i) {
			_table[i + j] = _table[j - i];
		}
		for (int i = 0, j = _length / 2; i < j; ++i) {
			_table[i + j] = -_table[i];
		}
	}


	void BlepTable::_generate() {
		// some amount of a sinc function.
		const float scaledPi = M_PI * 10.0f;
		_table[_length / 2] = 0.0f;
		for (int i = 1, j = _length / 2; i < j; ++i) {
			float radians = scaledPi * (i / (float)j);
			_table[j + i] = sinf(radians) / radians;
		}

		// "integrate": FIXME: avoid magic normalization value.
		const float norm = _length / 40.0f;
		float sum = 0.0f;
		for (int i = _length / 2; i < _length; ++i) {
			sum += _table[i];
			_table[i] = sum / norm;
		}

		// offset.
		for (int i = _length / 2; i < _length; ++i) {
			_table[i] -= 1.0f; // assumes successful normalization to 1-ish.
		}

		// copy to first half of table.
		for (int i = 0, j = _length / 2; i < j; ++i) {
			_table[i] = -_table[_length - 1 - i];
		}

		// smooth it out even more.
		HammingWindow hw(_length);
		hw.apply(_table, _table);
	}

    void Window::apply(float* in, float* out) {
	for (int i = 0; i < _size; ++i) {
		out[i] = in[i] * _window[i];
	}
}

    
HanningWindow::HanningWindow(int size, float alpha) : Window(size) {
	const float invAlpha = 1.0 - alpha;
	const float twoPIEtc = 2.0 * M_PI / (float)size;
	for (int i = 0; i < _size; ++i) {
		_sum += _window[i] = invAlpha*cos(twoPIEtc*(float)i + M_PI) + alpha;
	}
}


KaiserWindow::KaiserWindow(int size, float alpha) : Window(size) {
	float ii0a = 1.0f / i0(alpha);
	float ism1 = 1.0f / (float)(size - 1);
	for (int i = 0; i < _size; ++i) {
		float x = i * 2.0f;
		x *= ism1;
		x -= 1.0f;
		x *= x;
		x = 1.0f - x;
		x = sqrtf(x);
		x *= alpha;
		_sum += _window[i] = i0(x) * ii0a;
	}
}

// Rabiner, Gold: "The Theory and Application of Digital Signal Processing", 1975, page 103.
float KaiserWindow::i0(float x) {
	assert(x >= 0.0f);
	assert(x < 20.0f);
	float y = 0.5f * x;
	float t = .1e-8f;
	float e = 1.0f;
	float de = 1.0f;
	for (int i = 1; i <= 25; ++i) {
		de = de * y / (float)i;
		float sde = de * de;
		e += sde;
		if (e * t - sde > 0.0f) {
			break;
		}
	}
	return e;
}


PlanckTaperWindow::PlanckTaperWindow(int size, int taperSamples) : Window(size) {
	_window[0] = 0.0f;
	_sum += _window[size - 1] = 1.0f;

	for (int i = 1; i < taperSamples; ++i) {
		float x = ((float)taperSamples / (float)i) - ((float)taperSamples / (float)(taperSamples - i));
		x = 1.0f + exp(x);
		x = 1.0f / x;
		_sum += _window[i] = x;
	}
	int nOnes = size - 2 * taperSamples;
	std::fill_n(_window + taperSamples, nOnes, 1.0f);
	_sum += nOnes;
	for (int i = 0; i < taperSamples; ++i) {
		_sum += _window[size - 1 - i] = _window[i];
	}
}


typedef ffft::FFTRealFixLen<10> FIXED_FFT1024;

FFT1024::FFT1024() {
	_fft = new FIXED_FFT1024();
}

FFT1024::~FFT1024() {
	delete (FIXED_FFT1024*)_fft;
}

void FFT1024::do_fft(float* out, float* in) {
	((FIXED_FFT1024*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<12> FIXED_FFT4096;

FFT4096::FFT4096() {
	_fft = new FIXED_FFT4096();
}

FFT4096::~FFT4096() {
	delete (FIXED_FFT4096*)_fft;
}

void FFT4096::do_fft(float* out, float* in) {
	((FIXED_FFT4096*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<13> FIXED_FFT8192;

FFT8192::FFT8192() {
	_fft = new FIXED_FFT8192();
}

FFT8192::~FFT8192() {
	delete (FIXED_FFT8192*)_fft;
}

void FFT8192::do_fft(float* out, float* in) {
	((FIXED_FFT8192*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<14> FIXED_FFT16384;

FFT16384::FFT16384() {
	_fft = new FIXED_FFT16384();
}

FFT16384::~FFT16384() {
	delete (FIXED_FFT16384*)_fft;
}

void FFT16384::do_fft(float* out, float* in) {
	((FIXED_FFT16384*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<15> FIXED_FFT32768;

FFT32768::FFT32768() {
	_fft = new FIXED_FFT32768();
}

FFT32768::~FFT32768() {
	delete (FIXED_FFT32768*)_fft;
}

void FFT32768::do_fft(float* out, float* in) {
	((FIXED_FFT32768*)_fft)->do_fft(out, in);
}


SpectrumAnalyzer::SpectrumAnalyzer(
	Size size,
	Overlap overlap,
	WindowType windowType,
	float sampleRate,
	bool autoProcess
)
: OverlappingBuffer(size, overlap, autoProcess)
, _sampleRate(sampleRate)
{
	assert(size <= maxSize);
	assert(_sampleRate > size);

	switch (size) {
		case SIZE_1024: {
			_fft1024 = new FFT1024();
			break;
		}
		case SIZE_4096: {
			_fft4096 = new FFT4096();
			break;
		}
		case SIZE_8192: {
			_fft8192 = new FFT8192();
			break;
		}
		case SIZE_16384: {
			_fft16384 = new FFT16384();
			break;
		}
		case SIZE_32768: {
			_fft32768 = new FFT32768();
			break;
		}
		default: {
			_fft = new ffft::FFTReal<float>(size);
		}
	}

	switch (windowType) {
		case WINDOW_NONE: {
			break;
		}
		case WINDOW_HANNING: {
			_window = new HanningWindow(size);
			_windowOut = new float[size];
			break;
		}
		case WINDOW_HAMMING: {
			_window = new HammingWindow(size);
			_windowOut = new float[size];
			break;
		}
		case WINDOW_KAISER: {
			_window = new KaiserWindow(size);
			_windowOut = new float[size];
			break;
		}
	}

	_fftOut = new float[_size];
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
	if (_fft) {
		delete _fft;
	}
	if (_fft1024) {
		delete _fft1024;
	}
	if (_fft4096) {
		delete _fft4096;
	}
	if (_fft8192) {
		delete _fft8192;
	}
	if (_fft16384) {
		delete _fft16384;
	}
	if (_fft32768) {
		delete _fft32768;
	}

	if (_window) {
		delete _window;
		delete[] _windowOut;
	}

	delete[] _fftOut;
}

void SpectrumAnalyzer::processBuffer(float* samples) {
	float* input = samples;
	if (_window) {
		_window->apply(samples, _windowOut);
		input = _windowOut;
	}
	if (_fft1024) {
		_fft1024->do_fft(_fftOut, input);
	}
	else if (_fft4096) {
		_fft4096->do_fft(_fftOut, input);
	}
	else if (_fft8192) {
		_fft8192->do_fft(_fftOut, input);
	}
	else if (_fft16384) {
		_fft16384->do_fft(_fftOut, input);
	}
	else if (_fft32768) {
		_fft32768->do_fft(_fftOut, input);
	}
	else {
		_fft->do_fft(_fftOut, input);
	}
}

void SpectrumAnalyzer::getMagnitudes(float* bins, int nBins) {
	assert(nBins <= _size / 2);
	assert(_size % nBins == 0);

	const int bands = _size / 2;
	const int binWidth = bands / nBins;
	const float invBinWidth = 1.0 / (float)binWidth;
	const float normalization = 2.0 / powf(_window ? _window->sum() : _size, 2.0);
	for (int bin = 0; bin < nBins; ++bin) {
		float sum = 0.0;
		int binEnd = bin * binWidth + binWidth;
		for (int i = binEnd - binWidth; i < binEnd; ++i) {
			sum += (_fftOut[i]*_fftOut[i] + _fftOut[i + bands]*_fftOut[i + bands]) * normalization;
		}
		bins[bin] = sum * invBinWidth;
	}
}

}