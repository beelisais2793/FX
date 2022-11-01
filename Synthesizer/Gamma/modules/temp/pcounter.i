
namespace gam 
{
	class PCounter{
	public:

		PCounter(unsigned period=256);
		PCounter& period(unsigned n);
		unsigned period() const;
		bool cycled() const;
		unsigned count() const;
		PCounter& reset();

		%extend
		{
			bool Tick() { return (*$self)(); }
		}
	};
}