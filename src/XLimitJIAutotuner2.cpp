#include "plugin.hpp"

#include <array>
#include <vector>
#include <unordered_map>

struct XLimitJIAutotuner2 : Module {

	using PrimeExponentMap = std::unordered_map<int,int>;
	
	const int64_t VOLTAGELISTLIMIT = 1e6;

	enum ParamId {
		H0_PARAM,
		H1_PARAM,
		H2_PARAM,
		H3_PARAM,
		H4_PARAM,
		H5_PARAM,
		H6_PARAM,
		H7_PARAM,
		
		S0_PARAM,
		S1_PARAM,
		S2_PARAM,
		S3_PARAM,
		S4_PARAM,
		S5_PARAM,
		S6_PARAM,
		S7_PARAM,
		
		LB0_PARAM,
		LB1_PARAM,
		LB2_PARAM,
		LB3_PARAM,
		LB4_PARAM,
		LB5_PARAM,
		LB6_PARAM,
		LB7_PARAM,
		
		UB0_PARAM,
		UB1_PARAM,
		UB2_PARAM,
		UB3_PARAM,
		UB4_PARAM,
		UB5_PARAM,
		UB6_PARAM,
		UB7_PARAM,

		BUT0_PARAM,
		BUT1_PARAM,
		BUT2_PARAM,
		BUT3_PARAM,
		BUT4_PARAM,
		BUT5_PARAM,
		BUT6_PARAM,
		BUT7_PARAM,

		REMAP_PARAM,
		REMAPKEYS_PARAM,

		GUESS_PARAM,
		GUESSBOUND_PARAM,
		EUCLID_PARAM,

		SPACE_PARAM,
		IMAGE_PARAM,
		
		NONE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		VOUT_OUTPUT,
		VOUTRES_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BOUNDS0_LIGHT = 0,
		BOUNDS1_LIGHT,
		BOUNDS2_LIGHT,
		BOUNDS3_LIGHT,
		BOUNDS4_LIGHT,
		BOUNDS5_LIGHT,
		BOUNDS6_LIGHT,
		BOUNDS7_LIGHT,
		MONZO_LIGHT,

		REMAP_LIGHT,
		
		GUESS_LIGHT,
		EUCLID_LIGHT,

		SPACE_LIGHT,
		IMAGE_LIGHT,

		LIGHTS_LEN
	};

	struct Monzo {
		int x0 = 0;
		int x1 = 0;
		int x2 = 0;
		int x3 = 0;
		int x4 = 0;
		int x5 = 0;
		int x6 = 0;
		int x7 = 0;
		float tenney = 0.0;
		double pitch = 0.0;
	};

	int mVoltageListZeroIdx = 0;
	
	std::vector<Monzo> mVoltageList;
	std::vector<float> mAngles;
	std::vector<float> mAnglesSpiral;
	std::vector<float> mAnglesUsed;

	std::array<int, 8> mVoltageSizeList;
	int64_t mWantedVoltageListSize;

	ParamId mHistoricRemapGuess = NONE_PARAM;
	
	std::array<float, 8 * 5> mCurrParams;
	std::array<float, 8 * 5> mHistoricParams;

	std::array<PrimeExponentMap, 8> mPrimeExponentMaps;
	
	double log22 = std::log2(2.0);
	double log23 = std::log2(3.0);
	double log25 = std::log2(5.0);
	double log27 = std::log2(7.0);
	double log211 = std::log2(11.0);
	double log213 = std::log2(13.0);
	double log217 = std::log2(17.0);
	double log219 = std::log2(19.0);

	XLimitJIAutotuner2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		configParam(H0_PARAM, 1.f, 1024.f, 2.f, "Harmonic");
		paramQuantities[H0_PARAM]->snapEnabled = true;		
		configParam(H1_PARAM, 1.f, 1024.f, 3.f, "Harmonic");
		paramQuantities[H1_PARAM]->snapEnabled = true;	
		configParam(H2_PARAM, 1.f, 1024.f, 5.f, "Harmonic");
		paramQuantities[H2_PARAM]->snapEnabled = true;		
		configParam(H3_PARAM, 1.f, 1024.f, 7.f, "Harmonic");
		paramQuantities[H3_PARAM]->snapEnabled = true;		
		configParam(H4_PARAM, 1.f, 1024.f, 11.f, "Harmonic");
		paramQuantities[H4_PARAM]->snapEnabled = true;		
		configParam(H5_PARAM, 1.f, 1024.f, 13.f, "Harmonic");
		paramQuantities[H5_PARAM]->snapEnabled = true;		
		configParam(H6_PARAM, 1.f, 1024.f, 17.f, "Harmonic");
		paramQuantities[H6_PARAM]->snapEnabled = true;		
		configParam(H7_PARAM, 1.f, 1024.f, 19.f, "Harmonic");
		paramQuantities[H7_PARAM]->snapEnabled = true;
		
		configParam(S0_PARAM, 1.f, 1024.f, 1.f, "Subarmonic");
		paramQuantities[S0_PARAM]->snapEnabled = true;		
		configParam(S1_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S1_PARAM]->snapEnabled = true;		
		configParam(S2_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S2_PARAM]->snapEnabled = true;		
		configParam(S3_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S3_PARAM]->snapEnabled = true;		
		configParam(S4_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S4_PARAM]->snapEnabled = true;		
		configParam(S5_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S5_PARAM]->snapEnabled = true;		
		configParam(S6_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S6_PARAM]->snapEnabled = true;		
		configParam(S7_PARAM, 1.f, 1024.f, 1.f, "Subharmonic");
		paramQuantities[S7_PARAM]->snapEnabled = true;
		
		configParam(LB0_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB0_PARAM]->snapEnabled = true;
		configParam(LB1_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB1_PARAM]->snapEnabled = true;
		configParam(LB2_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB2_PARAM]->snapEnabled = true;
		configParam(LB3_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB3_PARAM]->snapEnabled = true;
		configParam(LB4_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB4_PARAM]->snapEnabled = true;
		configParam(LB5_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB5_PARAM]->snapEnabled = true;
		configParam(LB6_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB6_PARAM]->snapEnabled = true;
		configParam(LB7_PARAM, -30.f, 30.f, 0.f, "Lower Bound");
		paramQuantities[LB7_PARAM]->snapEnabled = true;
		
		configParam(UB0_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB0_PARAM]->snapEnabled = true;
		configParam(UB1_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB1_PARAM]->snapEnabled = true;
		configParam(UB2_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB2_PARAM]->snapEnabled = true;
		configParam(UB3_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB3_PARAM]->snapEnabled = true;
		configParam(UB4_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB4_PARAM]->snapEnabled = true;
		configParam(UB5_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB5_PARAM]->snapEnabled = true;
		configParam(UB6_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB6_PARAM]->snapEnabled = true;
		configParam(UB7_PARAM, -30.f, 30.f, 0.f, "Upper Bound");
		paramQuantities[UB7_PARAM]->snapEnabled = true;

		configParam(BUT0_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT0_PARAM]->snapEnabled = true;
		configParam(BUT1_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT1_PARAM]->snapEnabled = true;
		configParam(BUT2_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT2_PARAM]->snapEnabled = true;
		configParam(BUT3_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT3_PARAM]->snapEnabled = true;
		configParam(BUT4_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT4_PARAM]->snapEnabled = true;
		configParam(BUT5_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT5_PARAM]->snapEnabled = true;
		configParam(BUT6_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT6_PARAM]->snapEnabled = true;
		configParam(BUT7_PARAM, -30.f, 30.f, 0.f, "Shift");
		paramQuantities[BUT7_PARAM]->snapEnabled = true;
		
		configSwitch(REMAP_PARAM, 0.f, 1.f, 0.f, "Remap keyboard inputs to tuning circle steps", {"No","Yes"});
		configParam(REMAPKEYS_PARAM, 1.f, 100.f, 12.f, "Keyboard keys per octave");
		paramQuantities[REMAPKEYS_PARAM]->snapEnabled = true;
		
		configSwitch(GUESS_PARAM, 0.f, 1.f, 1.f, "Guess correct pitch by choosing monzo with smallest tenney harmonic distance to root", {"No","Yes"});
		configParam(GUESSBOUND_PARAM, 1.f, 150.f, 50.f, "Pitch bounds for guessing in +-cents");
		paramQuantities[GUESSBOUND_PARAM]->snapEnabled = true;
		configSwitch(EUCLID_PARAM, 0.f, 4.f, 0.f, "Harmonic relationship to", {"Root only", "Input order", "Ascending harminic order", "Descending harmonic order"});
		
		configSwitch(SPACE_PARAM, 0.f, 1.f, 0.f, "Tuning steps", {"Pitch space","Frequency space"});
		configSwitch(IMAGE_PARAM, 0.f, 1.f, 0.f, "Tuning circle", {"Circle (1 Octave)", "Spiral (3 Octaves)"});

		configLight(MONZO_LIGHT, "Monzo overfill");		
		lights[MONZO_LIGHT].setBrightness(0.f);
		
		configLight(BOUNDS0_LIGHT, "Bounds invalid");		
		lights[BOUNDS0_LIGHT].setBrightness(0.f);
		configLight(BOUNDS1_LIGHT, "Bounds invalid");		
		lights[BOUNDS1_LIGHT].setBrightness(0.f);
		configLight(BOUNDS2_LIGHT, "Bounds invalid");		
		lights[BOUNDS2_LIGHT].setBrightness(0.f);
		configLight(BOUNDS3_LIGHT, "Bounds invalid");		
		lights[BOUNDS3_LIGHT].setBrightness(0.f);
		configLight(BOUNDS4_LIGHT, "Bounds invalid");		
		lights[BOUNDS4_LIGHT].setBrightness(0.f);
		configLight(BOUNDS5_LIGHT, "Bounds invalid");		
		lights[BOUNDS5_LIGHT].setBrightness(0.f);
		configLight(BOUNDS6_LIGHT, "Bounds invalid");		
		lights[BOUNDS6_LIGHT].setBrightness(0.f);
		configLight(BOUNDS7_LIGHT, "Bounds invalid");		
		lights[BOUNDS7_LIGHT].setBrightness(0.f);

		configInput(VIN_INPUT, "V/Oct");
		configOutput(VOUT_OUTPUT, "V/Oct");		
		configOutput(VOUTRES_OUTPUT, "V/Oct residual");

		configBypass(VIN_INPUT, VOUT_OUTPUT);
		
		mAnglesUsed.reserve(16);
		mVoltageList.reserve(VOLTAGELISTLIMIT);

	}

	template <typename T>
	T pow2(const T& a) {
		return a*a;
	}

	bool isRemap(){
		return params[REMAP_PARAM].getValue() == 1.f;
	}
	
	bool isGuess(){
		return params[GUESS_PARAM].getValue() == 1.f;
	}
	
	void setRemap(bool remap){
		params[REMAP_PARAM].setValue(remap);
	}

	float getTuningSpace(){
		return params[SPACE_PARAM].getValue();
	}

	float getCircleMode(){
		return params[IMAGE_PARAM].getValue();
	}
	
	double clampD(double value, double min, double max) {
		return (value < min) ? min : (value > max) ? max : value;
	}

	void updateCurrentParams(){
		mCurrParams[0] = params[H0_PARAM].getValue();
		mCurrParams[1] = params[H1_PARAM].getValue();
		mCurrParams[2] = params[H2_PARAM].getValue();
		mCurrParams[3] = params[H3_PARAM].getValue();
		mCurrParams[4] = params[H4_PARAM].getValue();
		mCurrParams[5] = params[H5_PARAM].getValue();
		mCurrParams[6] = params[H6_PARAM].getValue();
		mCurrParams[7] = params[H7_PARAM].getValue();

		mCurrParams[8] = params[S0_PARAM].getValue();
		mCurrParams[9] = params[S1_PARAM].getValue();
		mCurrParams[10] = params[S2_PARAM].getValue();
		mCurrParams[11] = params[S3_PARAM].getValue();
		mCurrParams[12] = params[S4_PARAM].getValue();
		mCurrParams[13] = params[S5_PARAM].getValue();
		mCurrParams[14] = params[S6_PARAM].getValue();
		mCurrParams[15] = params[S7_PARAM].getValue();

		mCurrParams[16] = params[LB0_PARAM].getValue();
		mCurrParams[17] = params[LB1_PARAM].getValue();
		mCurrParams[18] = params[LB2_PARAM].getValue();
		mCurrParams[19] = params[LB3_PARAM].getValue();
		mCurrParams[20] = params[LB4_PARAM].getValue();
		mCurrParams[21] = params[LB5_PARAM].getValue();
		mCurrParams[22] = params[LB6_PARAM].getValue();
		mCurrParams[23] = params[LB7_PARAM].getValue();

		mCurrParams[24] = params[UB0_PARAM].getValue();
		mCurrParams[25] = params[UB1_PARAM].getValue();
		mCurrParams[26] = params[UB2_PARAM].getValue();
		mCurrParams[27] = params[UB3_PARAM].getValue();
		mCurrParams[28] = params[UB4_PARAM].getValue();
		mCurrParams[29] = params[UB5_PARAM].getValue();
		mCurrParams[30] = params[UB6_PARAM].getValue();
		mCurrParams[31] = params[UB7_PARAM].getValue();

		mCurrParams[32] = params[BUT0_PARAM].getValue();
		mCurrParams[33] = params[BUT1_PARAM].getValue();
		mCurrParams[34] = params[BUT2_PARAM].getValue();
		mCurrParams[35] = params[BUT3_PARAM].getValue();
		mCurrParams[36] = params[BUT4_PARAM].getValue();
		mCurrParams[37] = params[BUT5_PARAM].getValue();
		mCurrParams[38] = params[BUT6_PARAM].getValue();
		mCurrParams[39] = params[BUT7_PARAM].getValue();

		if(params[REMAP_PARAM].getValue() == 1.f && params[GUESS_PARAM].getValue() == 1.f){
			switch(mHistoricRemapGuess){
			case REMAP_PARAM:
				params[REMAP_PARAM].setValue(0.f);
				break;
				
			case GUESS_PARAM:
				params[GUESS_PARAM].setValue(0.f);
				break;
			
			default:
				params[REMAP_PARAM].setValue(0.f);
				params[GUESS_PARAM].setValue(0.f);
				break;
			}
		}

		if(params[REMAP_PARAM].getValue() == 1.f){
			mHistoricRemapGuess = REMAP_PARAM;
		}
		else if(params[GUESS_PARAM].getValue() == 1.f){
			mHistoricRemapGuess = GUESS_PARAM;
		} 
	}
	
	void updateVoltageSizeList() {
		float* lbPtr = &(mCurrParams[16]);
		float* ubPtr = &(mCurrParams[24]);

		for(int i = 0; i < 8; i++){
			mVoltageSizeList[i] = (int)(ubPtr[i]) - (int)(lbPtr[i]) + 1;
		}

		int64_t sz = 1;
		for(int i = 0; i < 8; i++){
			sz *= mVoltageSizeList[i];
			if(sz > VOLTAGELISTLIMIT){
				break;
			}
		}
		mWantedVoltageListSize = sz;
	}
	
	bool checkIfParamsChanged() {
		return mCurrParams != mHistoricParams;
	}

	bool checkIfParamsValid() {
		float* lbPtr = &(mCurrParams[16]);
		float* ubPtr = &(mCurrParams[24]);

		bool paramsInvalid = false;

		// check if lb <= ub
		for(int i = 0; i < 8; i++){
			bool boundsInvalid = lbPtr[i] > ubPtr[i];	
			lights[i].setBrightness(boundsInvalid ? 1.f : 0.f);
			paramsInvalid |= boundsInvalid;
		}

		// check size of voltage list
		if(!paramsInvalid){
			updateVoltageSizeList();
			bool sizeInvalid = mWantedVoltageListSize > VOLTAGELISTLIMIT;
			if(sizeInvalid) {
				for(int i = 0; i < 8; i++){
					lights[i].setBrightness(1.f);
				}
			}
			lights[MONZO_LIGHT].setBrightness(sizeInvalid ? 1.f : 0.f);
			paramsInvalid |= sizeInvalid;
		}

		return !paramsInvalid;
	}
	
	void updateHistoricParams(){
		mHistoricParams = mCurrParams;
	}
	
	void filterAngles(std::vector<Monzo>& original, std::vector<float>& filtered, float lowerbound, float upperbound){
		
		// Find the range using binary search
		auto lower = std::lower_bound(original.begin(), original.end(), lowerbound, 
		[](const Monzo& a, const double& b) { return a.pitch < b; });
	
		auto upper = std::lower_bound(original.begin(), original.end(), upperbound, 
		[](const Monzo& a, const double& b) { return a.pitch < b; });
		
		// Copy values in range into the filter vector
		filtered.clear();
		filtered.reserve(std::distance(lower, upper));
		for (auto it = lower; it != upper; ++it) {
			filtered.push_back(it->pitch); 
		}
	}

	int findClosestToZeroIndex(const std::vector<Monzo>& vec) {
		if (vec.empty()) return -1;  // Handle empty case
	
		// Binary search: Find the position where 0 would be inserted
		auto it = std::lower_bound(vec.begin(), vec.end(), 0.f, [](const Monzo& a, const float& b) { return a.pitch < b; });
		
		if (it == vec.begin()) return 0; // If 0 is before first element
		if (it == vec.end()) return vec.size() - 1; // If 0 would be placed after last element
		
		// Compare previous and current elements to see which is closer to zero
		int idx = std::distance(vec.begin(), it);
		if (std::abs(vec[idx].pitch) < std::abs(vec[idx - 1].pitch)) return idx;
		else return idx - 1;
	}

	void primeFactorization(PrimeExponentMap& factors, int n, int sign) {	
		if(n < 2){
			return;
		}
		// Factor out all 2s
		while (n % 2 == 0) {
			factors[2] += sign * 1;
			n /= 2;
		}
	
		// Check odd numbers up to sqrt(n)
		for (int i = 3; i * i <= n; i += 2) {
			while (n % i == 0) {
				factors[i] += sign * 1;
				n /= i;
			}
		}
	
		// If n is still greater than 1, it's a prime
		if (n > 1) {
			factors[n] += sign * 1;
		}
	}

	PrimeExponentMap monzoPrimes(const Monzo& m) {
		PrimeExponentMap map;
		const int* x = &(m.x0);

		for(int ii = 0; ii < 8; ii++) {
			for(auto& pair : mPrimeExponentMaps[ii]){
				map[pair.first] += x[ii] * pair.second; // prime_base ^ (x * exponent)
			}
		}

		return map;
	}

	float tenneyDist(PrimeExponentMap map) {
		double tenney = 0.0;		
		for(auto& pair : map){
			tenney += std::log2(pair.first) * std::abs(pair.second);
		}
		return tenney;
	}
	
	void buildVoltageList(){
		float* hPtr = &(mCurrParams[0]);
		float* sPtr = &(mCurrParams[8]);
		float* lbPtr = &(mCurrParams[16]);
		float* shPtr = &(mCurrParams[32]);

		mVoltageList.resize(mWantedVoltageListSize);

		for(int ii = 0; ii < 8; ii++) {
			auto& map = mPrimeExponentMaps[ii];
			map.clear();
			primeFactorization(map, hPtr[ii], +1);
			primeFactorization(map, sPtr[ii], -1);
		}
		
		log22 = std::log2((double)hPtr[0] / (double)sPtr[0]);
		log23 = std::log2((double)hPtr[1] / (double)sPtr[1]);
		log25 = std::log2((double)hPtr[2] / (double)sPtr[2]);
		log27 = std::log2((double)hPtr[3] / (double)sPtr[3]);
		log211 = std::log2((double)hPtr[4] / (double)sPtr[4]);
		log213 = std::log2((double)hPtr[5] / (double)sPtr[5]);
		log217 = std::log2((double)hPtr[6] / (double)sPtr[6]);
		log219 = std::log2((double)hPtr[7] / (double)sPtr[7]);

		auto& size2 = mVoltageSizeList[0];
		auto& size3 = mVoltageSizeList[1];
		auto& size5 = mVoltageSizeList[2];
		auto& size7 = mVoltageSizeList[3];
		auto& size11 = mVoltageSizeList[4];
		auto& size13 = mVoltageSizeList[5];
		auto& size17 = mVoltageSizeList[6];
		auto& size19 = mVoltageSizeList[7];
		
		int pow2 = lbPtr[0] + shPtr[0];
		int pow3 = lbPtr[1] + shPtr[1];
		int pow5 = lbPtr[2] + shPtr[2];
		int pow7 = lbPtr[3] + shPtr[3];
		int pow11 = lbPtr[4] + shPtr[4];
		int pow13 = lbPtr[5] + shPtr[5];
		int pow17 = lbPtr[6] + shPtr[6];
		int pow19 = lbPtr[7] + shPtr[7];

		PrimeExponentMap distMap;

		for(int i2 = 0; i2 < size2; i2++)
			for(int i3 = 0; i3 < size3; i3++)
				for(int i5 = 0; i5 < size5; i5++) 
					for(int i7 = 0; i7 < size7; i7++) 
						for(int i11 = 0; i11 < size11; i11++) 
							for(int i13 = 0; i13 < size13; i13++) 
								for(int i17 = 0; i17 < size17; i17++) 
									for(int i19 = 0; i19 < size19; i19++) {
										int idx = i2 
										+ i3 * size2 
										+ i5 * size2 * size3 
										+ i7 * size2 * size3 * size5 
										+ i11 * size2 * size3 * size5 * size7
										+ i13 * size2 * size3 * size5 * size7 * size11
										+ i17 * size2 * size3 * size5 * size7 * size11 * size13
										+ i19 * size2 * size3 * size5 * size7 * size11 * size13 * size17;
										
										auto& pitch = mVoltageList[idx].pitch;
										auto& x0 = mVoltageList[idx].x0;
										auto& x1 = mVoltageList[idx].x1;
										auto& x2 = mVoltageList[idx].x2;
										auto& x3 = mVoltageList[idx].x3;
										auto& x4 = mVoltageList[idx].x4;
										auto& x5 = mVoltageList[idx].x5;
										auto& x6 = mVoltageList[idx].x6;
										auto& x7 = mVoltageList[idx].x7;
										auto& tenney = mVoltageList[idx].tenney;
										
										x0 = pow2 + i2;
										x1 = pow3 + i3;
										x2 = pow5 + i5;
										x3 = pow7 + i7;
										x4 = pow11 + i11;
										x5 = pow13 + i13;
										x6 = pow17 + i17;
										x7 = pow19 + i19;

										pitch = ((double)0.0
										+ x0 * log22 
										+ x1 * log23
										+ x2 * log25
										+ x3 * log27
										+ x4 * log211
										+ x5 * log213
										+ x6 * log217
										+ x7 * log219);

										distMap.clear();
										distMap = monzoPrimes(mVoltageList[idx]);
										tenney = tenneyDist(distMap);
									}

		std::sort(mVoltageList.begin(), mVoltageList.end(), 
		[](const Monzo& a, const Monzo& b) { return a.pitch < b.pitch; });

		filterAngles(mVoltageList, mAngles, 0.f, 1.f);
		filterAngles(mVoltageList, mAnglesSpiral, -1.001f, 2.f);	
		
		mVoltageListZeroIdx = findClosestToZeroIndex(mVoltageList);		
	}

	float gaussFun(float x, float mu, float var) {
		//float amp = std::sqrt(2.0 * 3.14159 * var);
		float diff = x - mu;
		return std::expf(-(diff * diff) / (2.0 * var));
	}

	double findClosestInSorted(double target) {
		auto& vec = mVoltageList;		
		if (vec.empty()) return target;  // Handle empty case

		auto lower = std::lower_bound(vec.begin(), vec.end(), target, [](const Monzo& a, const double& b) { return a.pitch < b; });
	
		if (lower == vec.end()) return vec.back().pitch;  // If target is beyond the last element
		if (lower == vec.begin()) return vec.front().pitch;  // If target is before the first element
	
		// Compare the closest values
		double prev = (lower - 1)->pitch;
		double next = lower->pitch;
	
		return (std::abs(prev - target) < std::abs(next - target)) ? prev : next;
	}

	double findClosestGuess(double target){
		auto& vec = mVoltageList;
		
		if (vec.empty()) return target;  // Handle empty case
		
		auto bounds = params[GUESSBOUND_PARAM].getValue() / 1200.f;

		auto lower = std::lower_bound(vec.begin(), vec.end(), target - bounds, 
		[](const Monzo& a, const double& b) { return a.pitch < b; });
	
		auto upper = std::lower_bound(vec.begin(), vec.end(), target + bounds, 
		[](const Monzo& a, const double& b) { return a.pitch < b; });

		auto minValueIt = std::min_element(lower, upper, [](const Monzo& a, const Monzo& b) {
        	return a.tenney < b.tenney;
    	});

   		return minValueIt->pitch; 

	}
	
	double getFractionalPart(double value) {		
		return value - std::floor(value); // Absolutwert für negative Zahlen
	}

	void updateButtonLights(){		
		lights[REMAP_LIGHT].setBrightness(params[REMAP_PARAM].getValue() == 1.f);
		lights[GUESS_LIGHT].setBrightness(params[GUESS_PARAM].getValue() == 1.f);
		//lights[EUCLID_LIGHT].setBrightness(params[EUCLID_PARAM].getValue() == 0.f);
		lights[SPACE_LIGHT].setBrightness(params[SPACE_PARAM].getValue() == 0.f);
		lights[IMAGE_LIGHT].setBrightness(params[IMAGE_PARAM].getValue() == 0.f);
	}

	void processGuess(const ProcessArgs& args) {
		int channels = std::max(1, inputs[VIN_INPUT].getChannels());

		outputs[VOUT_OUTPUT].setChannels(channels);
		outputs[VOUTRES_OUTPUT].setChannels(channels);
		mAnglesUsed.resize(channels);

		double baseVoltage = inputs[VIN_INPUT].getPolyVoltage(0);
		double harmonicVoltage = 0.0;
		for (int c = 0; c < channels; c++) {
			double currVoltage = inputs[VIN_INPUT].getPolyVoltage(c);
			harmonicVoltage = findClosestGuess(currVoltage - baseVoltage);
			
			mAnglesUsed[c] = harmonicVoltage;
			double vout = clampD(baseVoltage + harmonicVoltage, -10.0, 10.0);
			float voutF = static_cast<float>(vout);			
			float voutR = vout - static_cast<double>(voutF);
			outputs[VOUT_OUTPUT].setVoltage(voutF, c);
			outputs[VOUTRES_OUTPUT].setVoltage(voutR, c);
		}

	}

	void processDefault(const ProcessArgs& args) {
		int channels = std::max(1, inputs[VIN_INPUT].getChannels());

		outputs[VOUT_OUTPUT].setChannels(channels);
		outputs[VOUTRES_OUTPUT].setChannels(channels);
		mAnglesUsed.resize(channels);

		double baseVoltage = inputs[VIN_INPUT].getPolyVoltage(0);
		double harmonicVoltage = 0.0;
		for (int c = 0; c < channels; c++) {
			double currVoltage = inputs[VIN_INPUT].getPolyVoltage(c);
			harmonicVoltage = findClosestInSorted(currVoltage - baseVoltage);			
			mAnglesUsed[c] = harmonicVoltage;
			double vout = clampD(baseVoltage + harmonicVoltage, -10.0, 10.0);
			float voutF = static_cast<float>(vout);			
			float voutR = vout - static_cast<double>(voutF);
			outputs[VOUT_OUTPUT].setVoltage(voutF, c);
			outputs[VOUTRES_OUTPUT].setVoltage(voutR, c);
		}

	}

	
	void processRemap(const ProcessArgs& args) {
		
		int channels = std::max(1, inputs[VIN_INPUT].getChannels());

		outputs[VOUT_OUTPUT].setChannels(channels);
		outputs[VOUTRES_OUTPUT].setChannels(channels);
		mAnglesUsed.resize(channels);

		float keysPerOct = params[REMAPKEYS_PARAM].getValue();
		double currVoltage = inputs[VIN_INPUT].getPolyVoltage(0);
		int baseIdx = (int)std::roundf(keysPerOct * currVoltage);
		int idx = clamp(baseIdx + mVoltageListZeroIdx, 0, mVoltageList.size() - 1);
		double baseVoltage = mVoltageList[idx].pitch;
		for (int c = 0; c < channels; c++) {
			currVoltage = inputs[VIN_INPUT].getPolyVoltage(c);
			idx = clamp((int)std::roundf(keysPerOct * currVoltage) + mVoltageListZeroIdx - baseIdx, 0, mVoltageList.size() - 1);
			double harmonicVoltage = mVoltageList[idx].pitch;
			mAnglesUsed[c] = harmonicVoltage;
			double vout = clampD(baseVoltage + harmonicVoltage, -10.0, 10.0);
			float voutF = static_cast<float>(vout);			
			float voutR = vout - static_cast<double>(voutF);
			outputs[VOUT_OUTPUT].setVoltage(voutF, c);
			outputs[VOUTRES_OUTPUT].setVoltage(voutR, c);
		}

	}
	
	void process(const ProcessArgs& args) override {

		updateCurrentParams();
		updateButtonLights();
		bool paramsValid = checkIfParamsValid();
		bool paramsHasChanged = checkIfParamsChanged();
		if(paramsHasChanged && paramsValid){
			buildVoltageList();		
			updateHistoricParams();	
		}
		
		if(isRemap()) {
			processRemap(args);		
		} 
		else if(isGuess()) {
			processGuess(args);
		}
		else {
			processDefault(args);
		}
	
	}
};

struct TuningCircle2 : LedDisplay {

	XLimitJIAutotuner2* mModule = nullptr;

	std::vector<float> mDefaultAnglesUsed{ 0.f, 3.f / 2.f, 5.f / 4.f, 15.f / 8.f };
	std::vector<float> mDefaultAngles{
		0.f, 
		64.f / 45.f, 
		16.f / 15.f, 
		8.f / 5.f, 
		6.f / 5.f,
		9.f / 5.f,
		16.f / 9.f,
		4.f / 3.f,
		3.f / 2.f,
		9.f / 8.f,
		10.f / 9.f,
		5.f / 3.f,
		5.f / 4.f, 
		15.f / 8.f,
		45.f / 32.f
	};

	void setModule(XLimitJIAutotuner2* module){
		mModule = module;
	}
	
	double getFractionalPart(double value) {
		return value - std::floor(value); // Absolutwert für negative Zahlen
	}
	
	NVGcolor hsvToRgb(float h, float s, float v) {
		float r = 0.f, g = 0.f, b = 0.f;
	
		int i = static_cast<int>(h * 6);
		float f = h * 6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);
	
		switch (i % 6) {
			case 0: r = v, g = t, b = p; break;
			case 1: r = q, g = v, b = p; break;
			case 2: r = p, g = v, b = t; break;
			case 3: r = p, g = q, b = v; break;
			case 4: r = t, g = p, b = v; break;
			case 5: r = v, g = p, b = q; break;
		}
	
		return nvgRGBAf(r, g, b, 1.0f); // Alpha is set to 1.0 (fully opaque)
	}

	void draw(const DrawArgs& args) override {
		bool circleMode = true;
		if(mModule){
			circleMode = mModule->getCircleMode() == 0.f;
		}
		if(circleMode){		
			drawCircle(args);
		}else{
			drawSpiral(args);
		}
	}

	void drawCircle(const DrawArgs& args) {

		bool isDark = settings::preferDarkPanels;
		auto bgColor = nvgRGBf(1.0,1.0,1.0);
		if(isDark){
			bgColor = nvgRGBf(0.0666, 0.0, 0.1);
		}

		// Draw a yellow rectangle with the size of this widget
		nvgFillColor(args.vg, bgColor);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
		nvgFill(args.vg);

    	// Circle center and radius
    	float centerX = box.size.x * 0.5f;
    	float centerY = box.size.y * 0.5f;

    	float radiusOuter = (56.5 - 4.f) * 0.5f;	
    	float radiusInner = radiusOuter * 0.333f * 0.5f;		
    	float radiusMiddle = (radiusOuter + radiusInner) * 0.5f;

		float radius = mm2px(radiusOuter);
		float radius2 = mm2px(radiusMiddle - 1.f);

		auto& angles = mDefaultAngles;
		if(mModule){
			angles = mModule->mAngles;
		}

    	// Draw each line
    	for (float normalizedAngle : angles) {
        	// Convert normalized angle [0, 1) to radians [0, 2*PI)
        	float angleRadians = normalizedAngle - 0.25;
			if(mModule){
				if(mModule->getTuningSpace() == 1.f) {
					angleRadians = (std::exp2(normalizedAngle) - 1.f - 0.25f);
				} 

			}
			angleRadians *= 2.0f * M_PI;

        	// Calculate line start and end points
        	float x1 = centerX;  // Start point (center of the circle)
        	float y1 = centerY;
        	float x2 = centerX + radius * cos(angleRadians);  // End point on the circle
        	float y2 = centerY + radius * sin(angleRadians);

        	// Draw the line
        	nvgBeginPath(args.vg);
        	nvgMoveTo(args.vg, x1, y1);
        	nvgLineTo(args.vg, x2, y2);
        	nvgStrokeColor(args.vg, hsvToRgb(normalizedAngle, 1.f, 1.f));  // Red color
        	nvgStrokeWidth(args.vg, 2.0f);  // Line thickness
        	nvgStroke(args.vg);
    	}
		
		// Draw a blue circle
		nvgFillColor(args.vg, nvgRGBf(0.2 * isDark, 0.2 * isDark, 0.2 * isDark));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusMiddle));
		nvgFill(args.vg);

		// Draw a blue circle
		nvgFillColor(args.vg, bgColor);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusMiddle - 1.f));
		nvgFill(args.vg);	

		auto& anglesUsed = mDefaultAnglesUsed;
		bool freqSpace = false;
		if(mModule) {
			anglesUsed = mModule->mAnglesUsed;
			freqSpace = mModule->getTuningSpace() == 1.f;
		}
		
    	// Draw each line
    	for (float unnormalizedAngle : anglesUsed) {
			float normalizedAngle = getFractionalPart(unnormalizedAngle);
        	// Convert normalized angle [0, 1) to radians [0, 2*PI)
        	float angleRadians = normalizedAngle - 0.25;
			if(freqSpace){
				angleRadians = (std::exp2(normalizedAngle) - 1.f - 0.25f);		

			}
			angleRadians *= 2.0f * M_PI;

        	// Calculate line start and end points
        	float x1 = centerX;  // Start point (center of the circle)
        	float y1 = centerY;
        	float x2 = centerX + radius2 * cos(angleRadians);  // End point on the circle
        	float y2 = centerY + radius2 * sin(angleRadians);

        	// Draw the line
        	nvgBeginPath(args.vg);
        	nvgMoveTo(args.vg, x1, y1);
        	nvgLineTo(args.vg, x2, y2);
        	nvgStrokeColor(args.vg, hsvToRgb(normalizedAngle, 0.9999f, 0.9999f));  // Red color
        	nvgStrokeWidth(args.vg, 2.0f);  // Line thickness
        	nvgStroke(args.vg);
    	}	
		
		
		// Draw a blue circle
		nvgFillColor(args.vg, bgColor);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusInner - 1.f));
		nvgFill(args.vg);

	}

	void drawSpiral(const DrawArgs& args) {		
		bool isDark = settings::preferDarkPanels;
		auto bgColor = nvgRGBf(1.0,1.0,1.0);
		if(isDark){
			bgColor = nvgRGBf(0.0666, 0.0, 0.1);
		}

		const int numSegments = 100;
		const float startRadius = mm2px(2.5f);
		const float endRadius = mm2px(22.f);
		float x = box.size.x * 0.5f;
		float y = box.size.y * 0.5f;
		const float angleStep = (2.0f * M_PI * 3) / numSegments; // Covers 3 full turns		
		bool freqSpace = false;

		if(mModule){
			freqSpace = mModule->getTuningSpace();
			auto& angles = mModule->mAnglesSpiral;
			for (float angle : angles) {
				if(angle < -1.f || angle > 2.f){
					continue;
				}
				float angleFrac = getFractionalPart(angle);
				if(freqSpace){
					angle = (std::floor(angle) + (std::exp2(angleFrac) - 1.f));
				}				
        		float radian = angle * 2.0f * M_PI - M_PI / 2;

				// Find closest radius on the spiral path
				float closestRadius = startRadius + (endRadius - startRadius) * ((angle + 1.0f) / 3.0f);
        		float px = x + closestRadius * cos(radian);
        		float py = y + closestRadius * sin(radian);

				// Determine line length based on spacing (adjust coefficient if needed)
				float closestRadius2 = startRadius + (endRadius - startRadius) * ((angle + 1.0f + 1.0f) / 3.0f);
				float lx = x + closestRadius2 * cos(radian);
				float ly = y + closestRadius2 * sin(radian);

				nvgBeginPath(args.vg);
        		nvgMoveTo(args.vg, px, py);
        		nvgLineTo(args.vg, lx, ly);
				nvgStrokeColor(args.vg, hsvToRgb(angleFrac, 0.9999f, 0.9999f));  // Red color
				nvgStrokeWidth(args.vg, 2.0f);  // Line thickness
				nvgStroke(args.vg);
    		}

			auto& anglesUsed = mModule->mAnglesUsed;
			nvgBeginPath(args.vg);
    		for (float angle : anglesUsed) {
				if(angle < -1.f || angle > 2.f){
					continue;
				}
				float angleFrac = getFractionalPart(angle);
				if(freqSpace){
					angle = (std::floor(angle) + (std::exp2(angleFrac) - 1.f));
				}
        		float radian = angle * 2.0f * M_PI - M_PI / 2;

				// Find closest radius on the spiral path
				float closestRadius = startRadius + (endRadius - startRadius) * ((angle + 1.0f) / 3.0f);
        		float px = x + closestRadius * cos(radian);
        		float py = y + closestRadius * sin(radian);

				// Determine line length based on spacing (adjust coefficient if needed)
				float closestRadius2 = startRadius + (endRadius - startRadius) * ((angle + 1.0f + 1.0f) / 3.0f);
				float lx = x + closestRadius2 * cos(radian);
				float ly = y + closestRadius2 * sin(radian);

        		nvgMoveTo(args.vg, px, py);
        		nvgLineTo(args.vg, lx, ly);
    		}
			nvgStrokeColor(args.vg, nvgRGBf(isDark, isDark, isDark));  // Red color
			nvgStrokeWidth(args.vg, 2.0f);  // Line thickness
			nvgStroke(args.vg);
			
		}

		nvgBeginPath(args.vg);
    	float prevX = x + startRadius * cos(-0.5f * M_PI);
    	float prevY = y + startRadius * sin(-0.5f * M_PI);

    	for (int i = 1; i <= numSegments; ++i) {
        	float angle = i * angleStep - 0.5 * M_PI;
        	float radius = startRadius + (endRadius - startRadius) * (i / static_cast<float>(numSegments));
        	float px = x + radius * cos(angle);
        	float py = y + radius * sin(angle);

        	float cx = (prevX + px) / 2;
        	float cy = (prevY + py) / 2;

        	if (i == 1) {
        	    nvgMoveTo(args.vg, prevX, prevY);
        	}

        	nvgQuadTo(args.vg, cx, cy, px, py);
        	prevX = px;
        	prevY = py;
    	}
		nvgStrokeColor(args.vg, nvgRGBf(0.2 * isDark, 0.2 * isDark, 0.2 * isDark));  // Red color
		nvgStrokeWidth(args.vg, 2.0f);  // Line thickness
		nvgStroke(args.vg);
	}
};


struct XLimitJIAutotuner2Widget : ModuleWidget {
	XLimitJIAutotuner2Widget(XLimitJIAutotuner2* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/XLimitJIAutotuner2.svg"),
			asset::plugin(pluginInstance, "res/XLimitJIAutotuner2-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 35.133)), module, XLimitJIAutotuner2::H0_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 35.133)), module, XLimitJIAutotuner2::S0_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 35.133)), module, XLimitJIAutotuner2::LB0_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 35.133)), module, XLimitJIAutotuner2::UB0_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 42.133)), module, XLimitJIAutotuner2::H1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 42.133)), module, XLimitJIAutotuner2::S1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 42.133)), module, XLimitJIAutotuner2::LB1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 42.133)), module, XLimitJIAutotuner2::UB1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 49.133)), module, XLimitJIAutotuner2::H2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 49.133)), module, XLimitJIAutotuner2::S2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 49.133)), module, XLimitJIAutotuner2::LB2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 49.133)), module, XLimitJIAutotuner2::UB2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 56.133)), module, XLimitJIAutotuner2::H3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 56.133)), module, XLimitJIAutotuner2::S3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 56.133)), module, XLimitJIAutotuner2::LB3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 56.133)), module, XLimitJIAutotuner2::UB3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 63.133)), module, XLimitJIAutotuner2::H4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 63.133)), module, XLimitJIAutotuner2::S4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 63.133)), module, XLimitJIAutotuner2::LB4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 63.133)), module, XLimitJIAutotuner2::UB4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 70.133)), module, XLimitJIAutotuner2::H5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 70.133)), module, XLimitJIAutotuner2::S5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 70.133)), module, XLimitJIAutotuner2::LB5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 70.133)), module, XLimitJIAutotuner2::UB5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 77.133)), module, XLimitJIAutotuner2::H6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 77.133)), module, XLimitJIAutotuner2::S6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 77.133)), module, XLimitJIAutotuner2::LB6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 77.133)), module, XLimitJIAutotuner2::UB6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.273, 84.133)), module, XLimitJIAutotuner2::H7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.273, 84.133)), module, XLimitJIAutotuner2::S7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.273, 84.133)), module, XLimitJIAutotuner2::LB7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(66.273, 84.133)), module, XLimitJIAutotuner2::UB7_PARAM));
		
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 35.133)), module, XLimitJIAutotuner2::BUT0_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 42.133)), module, XLimitJIAutotuner2::BUT1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 49.133)), module, XLimitJIAutotuner2::BUT2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 56.133)), module, XLimitJIAutotuner2::BUT3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 63.133)), module, XLimitJIAutotuner2::BUT4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 70.133)), module, XLimitJIAutotuner2::BUT5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 77.133)), module, XLimitJIAutotuner2::BUT6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(46.273, 84.133)), module, XLimitJIAutotuner2::BUT7_PARAM));
		
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(13.6305, 106.7995)), module, XLimitJIAutotuner2::VIN_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(125.9435, 104.541)), module, XLimitJIAutotuner2::VOUT_OUTPUT));		
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(107.9015, 104.541)), module, XLimitJIAutotuner2::VOUTRES_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 17.133)), module, XLimitJIAutotuner2::MONZO_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 35.133)), module, XLimitJIAutotuner2::BOUNDS0_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 42.133)), module, XLimitJIAutotuner2::BOUNDS1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 49.133)), module, XLimitJIAutotuner2::BOUNDS2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 56.133)), module, XLimitJIAutotuner2::BOUNDS3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 63.133)), module, XLimitJIAutotuner2::BOUNDS4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 70.132)), module, XLimitJIAutotuner2::BOUNDS5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 77.132)), module, XLimitJIAutotuner2::BOUNDS6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.232, 84.132)), module, XLimitJIAutotuner2::BOUNDS7_LIGHT));

		// mm2px(Vec(42.815, 42.815))
		addChild(createWidget<Widget>(mm2px(Vec(80.092, 33.092))));
		
		TuningCircle2* myWidget = createWidget<TuningCircle2>(mm2px(Vec(80.0, 31.5)));
		myWidget->setSize(mm2px(Vec(56.5, 56.5)));
		myWidget->setModule(module);
		addChild(myWidget);

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(29.1325, 115.1325)), module, XLimitJIAutotuner2::REMAP_PARAM, XLimitJIAutotuner2::REMAP_LIGHT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(39.1325, 115.1325)), module, XLimitJIAutotuner2::REMAPKEYS_PARAM));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(29.1325, 99.6325)), module, XLimitJIAutotuner2::GUESS_PARAM, XLimitJIAutotuner2::GUESS_LIGHT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(39.1325, 99.6325)), module, XLimitJIAutotuner2::GUESSBOUND_PARAM));
		//addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(49.1325, 99.6325)), module, XLimitJIAutotuner2::EUCLID_PARAM, XLimitJIAutotuner2::EUCLID_LIGHT));

		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(132.5, 35.133)), module, XLimitJIAutotuner2::SPACE_PARAM, XLimitJIAutotuner2::SPACE_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(108.25, 59.75)), module, XLimitJIAutotuner2::IMAGE_PARAM, XLimitJIAutotuner2::IMAGE_LIGHT));

	}

};


Model* modelXLimitJIAutotuner2 = createModel<XLimitJIAutotuner2, XLimitJIAutotuner2Widget>("XLimitJIAutotuner2");