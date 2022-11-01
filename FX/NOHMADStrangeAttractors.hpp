// NOHMAD
struct LorenzAttractor {
	float sigma, beta, rho, pitch; // Params
	float x, y, z; // Out

	static constexpr float DEFAULT_SIGNMA_VALUE = 10.0f;
	static constexpr float DEFAULT_BETA_VALUE = 8.0f / 3.0f;
	static constexpr float DEFAULT_RHO_VALUE = 28.0f;
	static constexpr float DEFAULT_PITCH_VALUE = 0.5f;

	LorenzAttractor() :
		sigma(DEFAULT_SIGNMA_VALUE), beta(DEFAULT_BETA_VALUE), rho(DEFAULT_RHO_VALUE), pitch(DEFAULT_PITCH_VALUE),
		x(1.0f), y(1.0f), z(1.0f) {}

	void process(float dt) {
		float dx = sigma * (y - x);
		float dy = x * (rho - z) - y;
		float dz = (x * y) - (beta * z);

		x += dx * dt * pitch * 375.0f;
		y += dy * dt * pitch * 375.0f;
		z += dz * dt * pitch * 375.0f;
	}
};

struct RosslerAttractor {
	float a, b, c, pitch; // Params
	float x, y, z; // Out

	static constexpr float DEFAULT_A_VALUE = 0.2f;
	static constexpr float DEFAULT_B_VALUE = 0.2f;
	static constexpr float DEFAULT_C_VALUE = 5.7f;
	static constexpr float DEFAULT_PITCH_VALUE = 0.5f;

	RosslerAttractor() :
		a(DEFAULT_A_VALUE), b(DEFAULT_B_VALUE), c(DEFAULT_C_VALUE), pitch(DEFAULT_PITCH_VALUE),
		x(1.0f), y(1.0f), z(1.0f) {}

	void process(float dt) {
		float dx = -y - z;
		float dy = x + (a * y);
		float dz = b + z * (x - c);

		x += dx * dt * pitch * 2910.0f;
		y += dy * dt * pitch * 2910.0f;
		z += dz * dt * pitch * 2910.0f;
	}
};