#include "plugin.hpp"

#include <array>
#include <vector>

struct XLimitJIAutotuner : Module {
	enum ParamId {
		POW2_PARAM,
		POW3_PARAM,
		POW5_PARAM,
		POW7_PARAM,
		POW11_PARAM,
		POW13_PARAM,
		POW17_PARAM,
		POW19_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VOCT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		VOUT_OUTPUT,
		VOUTRES_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PATH1_LIGHT,
		LIGHTS_LEN
	};
	
	std::vector<double> mVoltageList;
	std::vector<float> mAngles;
	std::vector<float> mAnglesUsed;

	std::array<float, 8> mHistoricParams;
	std::array<float, 8> mCurrParams;

	std::array<int, 8> mVoltageSizeList;
	int64_t mWantedVoltageListSize;

	const double log22 = std::log2(2.0);
	const double log23 = std::log2(3.0);
	const double log25 = std::log2(5.0);
	const double log27 = std::log2(7.0);
	const double log211 = std::log2(11.0);
	const double log213 = std::log2(13.0);
	const double log217 = std::log2(17.0);
	const double log219 = std::log2(19.0);

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

		mVoltageList.resize(mWantedVoltageListSize);

		auto& size2 = mVoltageSizeList[0];
		auto& size3 = mVoltageSizeList[1];
		auto& size5 = mVoltageSizeList[2];
		auto& size7 = mVoltageSizeList[3];
		auto& size11 = mVoltageSizeList[4];
		auto& size13 = mVoltageSizeList[5];
		auto& size17 = mVoltageSizeList[6];
		auto& size19 = mVoltageSizeList[7];
		
		int pow2 = -mCurrParams[0];
		int pow3 = -mCurrParams[1];
		int pow5 = -mCurrParams[2];
		int pow7 =  -mCurrParams[3];
		int pow11 = -mCurrParams[4];
		int pow13 = -mCurrParams[5];
		int pow17 = -mCurrParams[6];
		int pow19 = -mCurrParams[7];

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
	
	void updateCurrentParams(){
		mCurrParams[0] = params[POW2_PARAM].getValue();
		mCurrParams[1] = params[POW3_PARAM].getValue();
		mCurrParams[2] = params[POW5_PARAM].getValue();
		mCurrParams[3] = params[POW7_PARAM].getValue();
		mCurrParams[4] = params[POW11_PARAM].getValue();
		mCurrParams[5] = params[POW13_PARAM].getValue();
		mCurrParams[6] = params[POW17_PARAM].getValue();
		mCurrParams[7] = params[POW19_PARAM].getValue();
	}

	void updateHistoricParams(){
		mHistoricParams = mCurrParams;
	}

	void updateVoltageSizeList() {
		for(int i = 0; i < 8; i++){
			mVoltageSizeList[i] = 2 * mCurrParams[i] + 1;
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
		// check size of voltage list
		updateVoltageSizeList();
		bool sizeInvalid = mWantedVoltageListSize > 1e7;
		lights[PATH1_LIGHT].setBrightness(sizeInvalid ? 1.f : 0.f);

		return !sizeInvalid;
	}

	XLimitJIAutotuner() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(POW2_PARAM, 0.f, 20.f, 8.f, "Pow2");
		paramQuantities[POW2_PARAM]->snapEnabled = true;
		configParam(POW3_PARAM, 0.f, 20.f, 2.f, "Pow3");
		paramQuantities[POW3_PARAM]->snapEnabled = true;
		configParam(POW5_PARAM, 0.f, 20.f, 1.f, "Pow5");
		paramQuantities[POW5_PARAM]->snapEnabled = true;
		configParam(POW7_PARAM, 0.f, 20.f, 0.f, "Pow7");
		paramQuantities[POW7_PARAM]->snapEnabled = true;
		configParam(POW11_PARAM, 0.f, 20.f, 0.f, "Pow11");
		paramQuantities[POW11_PARAM]->snapEnabled = true;
		configParam(POW13_PARAM, 0.f, 20.f, 0.f, "Pow13");
		paramQuantities[POW13_PARAM]->snapEnabled = true;
		configParam(POW17_PARAM, 0.f, 20.f, 0.f, "Pow17");
		paramQuantities[POW17_PARAM]->snapEnabled = true;
		configParam(POW19_PARAM, 0.f, 20.f, 0.f, "Pow19");
		paramQuantities[POW19_PARAM]->snapEnabled = true;

		configInput(VOCT_INPUT, "V/Oct");
		configOutput(VOUT_OUTPUT, "V/Oct");
		configOutput(VOUTRES_OUTPUT, "V/Oct Residual");

		configLight(PATH1_LIGHT, "Monzo overfill");

		mVoltageList.reserve(1e7);
		mAnglesUsed.resize(16);
	}

	void process(const ProcessArgs& args) override {

		updateCurrentParams();
		bool paramsValid = checkIfParamsValid();
		bool paramsHasChanged = checkIfParamsChanged();
		if(paramsHasChanged && paramsValid){
			buildVoltageList();		
			updateHistoricParams();	
		}
			
		int channels = std::max(1, inputs[VOCT_INPUT].getChannels());

		outputs[VOUT_OUTPUT].setChannels(channels);
		outputs[VOUTRES_OUTPUT].setChannels(channels);
		mAnglesUsed.resize(channels);
		
		double baseVoltage = inputs[VOCT_INPUT].getPolyVoltage(0);
		outputs[VOUT_OUTPUT].setVoltage(baseVoltage, 0);
		outputs[VOUTRES_OUTPUT].setVoltage(0.f, 0);
		mAnglesUsed[0] = 0.f;

		for (int c = 1; c < channels; c++) {
			double currVoltage = inputs[VOCT_INPUT].getPolyVoltage(c);
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

struct TuningCircle : Widget {

	XLimitJIAutotuner* mModule = nullptr;

	std::vector<float> defaultAnglesUsed{ 0.f, 3.f / 2.f, 5.f / 4.f, 15.f / 8.f };
	std::vector<float> defaultAngles{
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

	void setModule(XLimitJIAutotuner* module){
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
    	float centerX = mm2px(26.0f);
    	float centerY = mm2px(26.0f);
    	float radiusOuter = 26.0f;	
    	float radiusInner = 13.f * 0.33f;		
    	float radiusMiddle = (radiusOuter + radiusInner) * 0.5f;

		float radius = mm2px(radiusOuter);
		float radius2 = mm2px(radiusMiddle - 1.f);

		auto& angles = defaultAngles;
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

		auto& anglesUsed = defaultAnglesUsed;
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
		nvgCircle(args.vg, centerX, centerY, mm2px(13.f * 0.33f));
		nvgFill(args.vg);

		// Draw a blue circle
		nvgFillColor(args.vg, bgColor);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(13.f * 0.33f - 1.f));
		nvgFill(args.vg);


	}
};

struct XLimitJIAutotunerWidget : ModuleWidget {
	XLimitJIAutotunerWidget(XLimitJIAutotuner* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/XLimitJIAutotuner.svg"),
			asset::plugin(pluginInstance, "res/XLimitJIAutotuner-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.5, 37.238)), module, XLimitJIAutotuner::POW2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.5, 37.238)), module, XLimitJIAutotuner::POW3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42.5, 37.238)), module, XLimitJIAutotuner::POW5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(56.5, 37.238)), module, XLimitJIAutotuner::POW7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(70.5, 37.238)), module, XLimitJIAutotuner::POW11_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(84.5, 37.238)), module, XLimitJIAutotuner::POW13_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(98.5, 37.238)), module, XLimitJIAutotuner::POW17_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(112.5, 37.238)), module, XLimitJIAutotuner::POW19_PARAM));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(12.65, 97.132)), module, XLimitJIAutotuner::VOCT_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(110.132, 97.132)), module, XLimitJIAutotuner::VOUT_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(110.132, 69.5415)), module, XLimitJIAutotuner::VOUTRES_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(109.207, 24.133)), module, XLimitJIAutotuner::PATH1_LIGHT));

		// mm2px(Vec(49.785, 49.785))
		addChild(createWidget<Widget>(mm2px(Vec(38.607, 61.796))));

		//FramebufferWidget* myWidgetFb = new FramebufferWidget;
		//addChild(myWidgetFb);

		TuningCircle* myWidget = createWidget<TuningCircle>(mm2px(Vec(37.5, 60.689)));
		myWidget->setSize(mm2px(Vec(52, 52)));
		myWidget->setModule(module);
		addChild(myWidget);
	}
};

Model* modelXLimitJIAutotuner = createModel<XLimitJIAutotuner, XLimitJIAutotunerWidget>("XLimitJIAutotuner");