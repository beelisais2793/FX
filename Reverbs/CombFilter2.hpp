#pragma once

#include <vector>

using std::vector;

namespace FX::Delays
{
	class CombFilter : FilterProcessor {
	public:
		CombFilter() : FilterProcessor(), bufferIndex(0), lastCombOut(0) {};
		void setSize(const int& size);
		void clear();
		float process(const float input, const float damp, const float feedbacklvl);

		double Tick(double I, double damp=0.5, double feedback=0.5, double Y=0) {
			int bi = bufferIndex;
			bufferIndex += Y * buffer.size();
			bufferIndex %= buffer.size();
			double out = process(I,damp,feedback);
			bufferIndex = bi;
			return out;
		}
	private:
		vector<float> buffer;
		int bufferIndex;
		float lastCombOut;
	};



	void CombFilter::setSize(const int & size)
	{
		if (size != buffer.size()) {
			bufferIndex = 0;
			buffer.resize(size);
		}
		clear();
	}

	void CombFilter::clear()
	{
		lastCombOut = 0;
		for (auto iter = buffer.begin(); iter != buffer.end(); ++iter) {
			(*iter) = 0;
		}
	}

	float CombFilter::process(const float input, const float damp, const float feedbacklvl)
	{
		// read element at current position in filter buffer
		const float output = buffer[bufferIndex];
		float filterOut;

		// implement LPF in feedback to simulate room damping
		lastCombOut = (output * (1.0 - damp)) + (lastCombOut * damp);

		lastCombOut += 0.1f; lastCombOut -= 0.1f;
		
		// implement comb filter
		filterOut = input - (lastCombOut * feedbacklvl);

		filterOut += 0.1f; filterOut -= 0.1f;

		// write filter output to buffer and inc index
		buffer[bufferIndex] = filterOut;
		bufferIndex = (bufferIndex + 1) % buffer.size();

		return filterOut;
	}
}