#include "plugin.hpp"

#include <array>
#include <vector>

struct XLimitJIAutotuner2 : Module {

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
		BUT0_LIGHT,
		BUT1_LIGHT,
		BUT2_LIGHT,
		BUT3_LIGHT,
		BUT4_LIGHT,
		BUT5_LIGHT,
		BUT6_LIGHT,
		BUT7_LIGHT,
		LIGHTS_LEN
	};
	
	std::vector<double> mVoltageList;
	std::vector<float> mAngles;
	std::vector<float> mAnglesUsed;

	std::array<int, 8> mVoltageSizeList;
	int64_t mWantedVoltageListSize;
	
	std::array<float, 8 * 5> mCurrParams;
	std::array<float, 8 * 5> mHistoricParams;
	
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
		
		configParam(LB0_PARAM, -100.f, 100.f, -10.f, "Lower Bound");
		paramQuantities[LB0_PARAM]->snapEnabled = true;
		configParam(LB1_PARAM, -100.f, 100.f, -2.f, "Lower Bound");
		paramQuantities[LB1_PARAM]->snapEnabled = true;
		configParam(LB2_PARAM, -100.f, 100.f, -1.f, "Lower Bound");
		paramQuantities[LB2_PARAM]->snapEnabled = true;
		configParam(LB3_PARAM, -100.f, 100.f, 0.f, "Lower Bound");
		paramQuantities[LB3_PARAM]->snapEnabled = true;
		configParam(LB4_PARAM, -100.f, 100.f, 0.f, "Lower Bound");
		paramQuantities[LB4_PARAM]->snapEnabled = true;
		configParam(LB5_PARAM, -100.f, 100.f, 0.f, "Lower Bound");
		paramQuantities[LB5_PARAM]->snapEnabled = true;
		configParam(LB6_PARAM, -100.f, 100.f, 0.f, "Lower Bound");
		paramQuantities[LB6_PARAM]->snapEnabled = true;
		configParam(LB7_PARAM, -100.f, 100.f, 0.f, "Lower Bound");
		paramQuantities[LB7_PARAM]->snapEnabled = true;
		
		configParam(UB0_PARAM, -100.f, 100.f, 10.f, "Upper Bound");
		paramQuantities[UB0_PARAM]->snapEnabled = true;
		configParam(UB1_PARAM, -100.f, 100.f, 2.f, "Upper Bound");
		paramQuantities[UB1_PARAM]->snapEnabled = true;
		configParam(UB2_PARAM, -100.f, 100.f, 1.f, "Upper Bound");
		paramQuantities[UB2_PARAM]->snapEnabled = true;
		configParam(UB3_PARAM, -100.f, 100.f, 0.f, "Upper Bound");
		paramQuantities[UB3_PARAM]->snapEnabled = true;
		configParam(UB4_PARAM, -100.f, 100.f, 0.f, "Upper Bound");
		paramQuantities[UB4_PARAM]->snapEnabled = true;
		configParam(UB5_PARAM, -100.f, 100.f, 0.f, "Upper Bound");
		paramQuantities[UB5_PARAM]->snapEnabled = true;
		configParam(UB6_PARAM, -100.f, 100.f, 0.f, "Upper Bound");
		paramQuantities[UB6_PARAM]->snapEnabled = true;
		configParam(UB7_PARAM, -100.f, 100.f, 0.f, "Upper Bound");
		paramQuantities[UB7_PARAM]->snapEnabled = true;

		configSwitch(BUT0_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT1_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT2_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT3_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT4_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT5_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT6_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});
		configSwitch(BUT7_PARAM, 0.f, 1.f, 1.f, "Mirror Bounds", {"0","1"});

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
		configOutput(VOUTRES_OUTPUT, "V/Oct Residual");
		
		mAnglesUsed.reserve(16);
		mVoltageList.reserve(1e7);

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
			if(sz > 1e7){
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
			bool sizeInvalid = mWantedVoltageListSize > 1e7;
			lights[MONZO_LIGHT].setBrightness(sizeInvalid ? 1.f : 0.f);
			paramsInvalid |= sizeInvalid;
		}

		return !paramsInvalid;
	}
	
	void updateHistoricParams(){
		mHistoricParams = mCurrParams;
	}
	
	void updateButtonLights(){
		float* butPtr = &(mCurrParams[32]);
		auto* lightPtr = &(lights[BUT0_LIGHT]);

		for(int i = 0; i < 8; i++){
			lightPtr[i].setBrightness(butPtr[i]);
		}
	}
	
	void updateMirrorButtons(){
		float* butPtr = &(mCurrParams[32]);
		auto* lbPtr = &(params[LB0_PARAM]);
		float* ubPtr = &(mCurrParams[24]);
		
		for(int i = 0; i < 8; i++){
			if(butPtr[i] == 1.f){
				lbPtr[i].setValue(-ubPtr[i]);
			}
		}

	}

	double modelFunD(double v, double x2, double x3, double x5 = 0.0, double x7 = 0.0, double x11 = 0.0, double x13 = 0.0, double x17 = 0.0, double x19 = 0.0) {
		v += log22 * x2;
		v += log23 * x3;
		v += log25 * x5;
		v += log27 * x7;
		v += log211 * x11;
		v += log213 * x13;
		v += log217 * x17;
		v += log219 * x19;
		return v;
	}

	void filterAngles(std::vector<double>& original, std::vector<float>& filtered, float lowerbound, float upperbound){
		
		// Find the range [0, 1) using binary search
		auto lower = std::lower_bound(original.begin(), original.end(), lowerbound); // First value >= 0
		auto upper = std::lower_bound(original.begin(), original.end(), upperbound); // First value >= 1	
		// Copy values in range [0, 1) into the filter vector
		filtered.clear();
		filtered.assign(lower, upper);
	}
	
	void buildVoltageList(){
		float* hPtr = &(mCurrParams[0]);
		float* sPtr = &(mCurrParams[8]);
		float* lbPtr = &(mCurrParams[16]);

		mVoltageList.resize(mWantedVoltageListSize);
		
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
		
		int pow2 = lbPtr[0];
		int pow3 = lbPtr[1];
		int pow5 = lbPtr[2];
		int pow7 = lbPtr[3];
		int pow11 = lbPtr[4];
		int pow13 = lbPtr[5];
		int pow17 = lbPtr[6];
		int pow19 = lbPtr[7];

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
										mVoltageList[idx] = modelFunD(0.0, pow2 + i2, pow3 + i3, pow5 + i5, pow7 + i7, pow11 + i11, pow13 + i13, pow17 + i17, pow19 + i19);
									}

		std::sort(mVoltageList.begin(), mVoltageList.end());

		filterAngles(mVoltageList, mAngles, 0.f, 1.f);	
		
	}

	double findClosestInSorted(double target) {
		auto& vec = mVoltageList;

		auto lower = std::lower_bound(vec.begin(), vec.end(), target);
	
		if (lower == vec.end()) return vec.back();  // If target is beyond the last element
		if (lower == vec.begin()) return vec.front();  // If target is before the first element
	
		// Compare the closest values
		double prev = *(lower - 1);
		double next = *lower;
	
		return (std::abs(prev - target) < std::abs(next - target)) ? prev : next;
	}
	
	double getFractionalPart(double value) {
		// Subtrahiere die Ganzzahl-Komponente, um die Nachkommastellen zu erhalten
		return value - std::floor(value); // Absolutwert für negative Zahlen
	}

	
	void process(const ProcessArgs& args) override {

		updateCurrentParams();
		updateMirrorButtons();
		updateButtonLights();
		bool paramsValid = checkIfParamsValid();
		bool paramsHasChanged = checkIfParamsChanged();
		if(paramsHasChanged && paramsValid){
			buildVoltageList();		
			updateHistoricParams();	
		}
		
		int channels = std::max(1, inputs[VIN_INPUT].getChannels());

		outputs[VOUT_OUTPUT].setChannels(channels);
		outputs[VOUTRES_OUTPUT].setChannels(channels);
		mAnglesUsed.resize(channels);
		
		double baseVoltage = inputs[VIN_INPUT].getPolyVoltage(0);
		outputs[VOUT_OUTPUT].setVoltage(baseVoltage, 0);
		outputs[VOUTRES_OUTPUT].setVoltage(0.f, 0);
		mAnglesUsed[0] = 0.f;

		for (int c = 1; c < channels; c++) {
			double currVoltage = inputs[VIN_INPUT].getPolyVoltage(c);
			double harmonicVoltage = findClosestInSorted(currVoltage - baseVoltage);
			mAnglesUsed[c] = getFractionalPart(harmonicVoltage);
			double vout = baseVoltage + harmonicVoltage;
			float voutF = static_cast<float>(vout);			
			float voutR = vout - static_cast<double>(voutF);
			outputs[VOUT_OUTPUT].setVoltage(voutF, c);
			outputs[VOUTRES_OUTPUT].setVoltage(voutR, c);
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

    	float radiusOuter = 40.f * 0.5f;	
    	float radiusInner = radiusOuter * 0.33f * 0.5f;		
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
        	float angleRadians = (normalizedAngle - 0.25) * 2.0f * M_PI;

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
		if(mModule) {
			anglesUsed = mModule->mAnglesUsed;
		}
		
    	// Draw each line
    	for (float normalizedAngle : anglesUsed) {
        	// Convert normalized angle [0, 1) to radians [0, 2*PI)
        	float angleRadians = (normalizedAngle - 0.25) * 2.0f * M_PI;

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
		nvgFillColor(args.vg, nvgRGBf(0.2 * isDark, 0.2 * isDark, 0.2 * isDark));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusInner));
		nvgFill(args.vg);

		// Draw a blue circle
		nvgFillColor(args.vg, bgColor);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusInner - 1.f));
		nvgFill(args.vg);

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
		
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 35.133)), module, XLimitJIAutotuner2::BUT0_PARAM, XLimitJIAutotuner2::BUT0_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 42.133)), module, XLimitJIAutotuner2::BUT1_PARAM, XLimitJIAutotuner2::BUT1_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 49.133)), module, XLimitJIAutotuner2::BUT2_PARAM, XLimitJIAutotuner2::BUT2_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 56.133)), module, XLimitJIAutotuner2::BUT3_PARAM, XLimitJIAutotuner2::BUT3_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 63.133)), module, XLimitJIAutotuner2::BUT4_PARAM, XLimitJIAutotuner2::BUT4_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 70.133)), module, XLimitJIAutotuner2::BUT5_PARAM, XLimitJIAutotuner2::BUT5_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 77.133)), module, XLimitJIAutotuner2::BUT6_PARAM, XLimitJIAutotuner2::BUT6_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<SmallSimpleLight<WhiteLight>>>(mm2px(Vec(46.273, 84.133)), module, XLimitJIAutotuner2::BUT7_PARAM, XLimitJIAutotuner2::BUT7_LIGHT));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(11.9305, 104.541)), module, XLimitJIAutotuner2::VIN_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(111.132, 104.541)), module, XLimitJIAutotuner2::VOUT_OUTPUT));		
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(95.1325, 104.541)), module, XLimitJIAutotuner2::VOUTRES_OUTPUT));

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
		
		TuningCircle2* myWidget = createWidget<TuningCircle2>(mm2px(Vec(79.0, 37.16)));
		myWidget->setSize(mm2px(Vec(45, 45)));
		myWidget->setModule(module);
		addChild(myWidget);
	}
};


Model* modelXLimitJIAutotuner2 = createModel<XLimitJIAutotuner2, XLimitJIAutotuner2Widget>("XLimitJIAutotuner2");