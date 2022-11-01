#pragma once

#include <vector>

namespace JoonasFX
{
	template <typename Type>
	class DelayLine : public FunctionProcessor
	{
	public:

		DelayLine() : FunctionProcessor() {
			
		}
		DelayLine(size_t n) : FunctionProcessor()
		{
			resize(n);
		}
		void clear() noexcept
		{
			std::fill(rawData.begin(), rawData.end(), Type(0));
		}

		size_t size() const noexcept
		{
			return rawData.size();
		}

		void resize(size_t newValue)
		{
			rawData.resize(newValue);
			leastRecentIndex = 0;
		}

		Type back() const noexcept
		{
			return rawData[leastRecentIndex];
		}

		Type get(size_t delayInSamples) const noexcept
		{
			//jassert(delayInSamples >= 0 && delayInSamples < size());

			return rawData[(leastRecentIndex + 1 + delayInSamples) % size()];
		}

		/** Set the specified sample in the delay line */
		void set(size_t delayInSamples, Type newValue) noexcept
		{
			//jassert(delayInSamples >= 0 && delayInSamples < size());

			rawData[(leastRecentIndex + 1 + delayInSamples) % size()] = newValue;
		}

		/** Adds a new value to the delay line, overwriting the least recently added sample */
		void push(Type valueToAdd) noexcept
		{
			rawData[leastRecentIndex] = valueToAdd;
			leastRecentIndex = leastRecentIndex == 0 ? size() - 1 : leastRecentIndex - 1;
		}

		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
		{
			DspFloatType r = back();
			push(X*I + Y*r);
			return A*r;
		}
		
	private:
		std::vector<Type> rawData;
		size_t leastRecentIndex = 0;
	};
}