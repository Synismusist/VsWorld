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
		SQR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PATH1_LIGHT,
		LIGHTS_LEN
	};
	
	std::vector<double> voltageList;
	std::vector<float> angles;
	std::vector<float> anglesUsed;

	const double log22 = std::log2(2.0);
	const double log23 = std::log2(3.0);
	const double log25 = std::log2(5.0);
	const double log27 = std::log2(7.0);
	const double log211 = std::log2(11.0);
	const double log213 = std::log2(13.0);
	const double log217 = std::log2(17.0);
	const double log219 = std::log2(19.0);

	std::array<float, 8> historicPowers{
		10.f, // 2
		2.f, // 3
		1.f, // 5
		0.f, // 7
		0.f, // 11
		0.f, // 13
		0.f, // 17
		0.f // 19
	};

	template <typename T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    	return (v < lo) ? lo : (v > hi) ? hi : v;
	}

	bool powersChanged(float pow2, float pow3, float pow5, float pow7, float pow11, float pow13, float pow17, float pow19) {
		bool hasChanged = false;
		hasChanged |= historicPowers[0] != pow2;
		hasChanged |= historicPowers[1] != pow3;
		hasChanged |= historicPowers[2] != pow5;
		hasChanged |= historicPowers[3] != pow7;
		hasChanged |= historicPowers[4] != pow11;
		hasChanged |= historicPowers[5] != pow13;
		hasChanged |= historicPowers[6] != pow17;
		hasChanged |= historicPowers[7] != pow19;
		return hasChanged;
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

	void buildList(float pow2, float pow3, float pow5, float pow7, float pow11, float pow13, float pow17, float pow19) {
		int size2 = 2 * pow2 + 1;
		int size3 = 2 * pow3 + 1;
		int size5 = 2 * pow5 + 1;
		int size7 = 2 * pow7 + 1;
		int size11 = 2 * pow11 + 1;
		int size13 = 2 * pow13 + 1;
		int size17 = 2 * pow17 + 1;
		int size19 = 2 * pow19 + 1;

		int voltageSize = size2 * size3 * size5 * size7 * size11 * size13 * size17 * size19;

		if(voltageSize > 1e7){
			return;
		}

		voltageList.resize(voltageSize);

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
										voltageList[idx] = modelFunD(0.0, -pow2 + i2, -pow3 + i3, -pow5 + i5, -pow7 + i7, -pow11 + i11, -pow13 + i13, -pow17 + i17, -pow19 + i19);
									}

		std::sort(voltageList.begin(), voltageList.end());

		filterAngles(voltageList, angles, 0.f, 1.f);

		historicPowers = {
			pow2,
			pow3,
			pow5,
			pow7,
			pow11,
			pow13,
			pow17,
			pow19,
		};

	}

	double findClosestInSorted(double target) {
		auto& vec = voltageList;

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

	XLimitJIAutotuner() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(POW2_PARAM, 0.f, 20.f, historicPowers[0], "Pow2");
		paramQuantities[POW2_PARAM]->snapEnabled = true;
		configParam(POW3_PARAM, 0.f, 20.f, historicPowers[1], "Pow3");
		paramQuantities[POW3_PARAM]->snapEnabled = true;
		configParam(POW5_PARAM, 0.f, 20.f, historicPowers[2], "Pow5");
		paramQuantities[POW5_PARAM]->snapEnabled = true;
		configParam(POW7_PARAM, 0.f, 20.f, historicPowers[3], "Pow7");
		paramQuantities[POW7_PARAM]->snapEnabled = true;
		configParam(POW11_PARAM, 0.f, 20.f, historicPowers[4], "Pow11");
		paramQuantities[POW11_PARAM]->snapEnabled = true;
		configParam(POW13_PARAM, 0.f, 20.f, historicPowers[5], "Pow13");
		paramQuantities[POW13_PARAM]->snapEnabled = true;
		configParam(POW17_PARAM, 0.f, 20.f, historicPowers[6], "Pow17");
		paramQuantities[POW17_PARAM]->snapEnabled = true;
		configParam(POW19_PARAM, 0.f, 20.f, historicPowers[7], "Pow19");
		paramQuantities[POW19_PARAM]->snapEnabled = true;

		configInput(VOCT_INPUT, "V/Oct");
		configOutput(SQR_OUTPUT, "V/Oct");

		configLight(PATH1_LIGHT, "Emergency stop");

		lights[PATH1_LIGHT].setBrightness(0.f);

		voltageList.reserve(1e7);
		anglesUsed.reserve(16);
		
		buildList(
			params[POW2_PARAM].getValue(),
			params[POW3_PARAM].getValue(),
			params[POW5_PARAM].getValue(),
			params[POW7_PARAM].getValue(),
			params[POW11_PARAM].getValue(),
			params[POW13_PARAM].getValue(),
			params[POW17_PARAM].getValue(),
			params[POW19_PARAM].getValue()
		);
	}

	void process(const ProcessArgs& args) override {
		float pow2 = params[POW2_PARAM].getValue();
		float pow3 = params[POW3_PARAM].getValue();
		float pow5 = params[POW5_PARAM].getValue();
		float pow7 = params[POW7_PARAM].getValue();
		float pow11 = params[POW11_PARAM].getValue();
		float pow13 = params[POW13_PARAM].getValue();
		float pow17 = params[POW17_PARAM].getValue();
		float pow19 = params[POW19_PARAM].getValue();
		
		int size2 = 2 * pow2 + 1;
		int size3 = 2 * pow3 + 1;
		int size5 = 2 * pow5 + 1;
		int size7 = 2 * pow7 + 1;
		int size11 = 2 * pow11 + 1;
		int size13 = 2 * pow13 + 1;
		int size17 = 2 * pow17 + 1;
		int size19 = 2 * pow19 + 1;

		int voltageSize = size2 * size3 * size5 * size7 * size11 * size13 * size17 * size19;

		lights[PATH1_LIGHT].setBrightness(voltageSize > 1e7 ? 1.f : 0.f);	

		if(powersChanged(pow2, pow3, pow5, pow7, pow11, pow13, pow17, pow19)){
			buildList(pow2, pow3, pow5, pow7, pow11, pow13, pow17, pow19);
		}
			
		int channels = std::max(1, inputs[VOCT_INPUT].getChannels());

		outputs[SQR_OUTPUT].setChannels(channels);
		anglesUsed.resize(channels);
		
		double baseVoltage = inputs[VOCT_INPUT].getPolyVoltage(0);
		outputs[SQR_OUTPUT].setVoltage(baseVoltage, 0);
		anglesUsed[0] = 0.f;

		for (int c = 1; c < channels; c++) {
			double currVoltage = inputs[VOCT_INPUT].getPolyVoltage(c);
			double harmonicVoltage = findClosestInSorted(currVoltage - baseVoltage);
			anglesUsed[c] = getFractionalPart(harmonicVoltage);
			outputs[SQR_OUTPUT].setVoltage(baseVoltage + harmonicVoltage, c);
		}
	
	}
};

struct TuningCircle : LedDisplay {

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

		// Draw a yellow rectangle with the size of this widget
		nvgFillColor(args.vg, nvgRGBf(1.0, 1.0, 1.0));
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
			angles = mModule->angles;
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
		nvgFillColor(args.vg, nvgRGBf(0.0, 0.0, 0.0));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusMiddle));
		nvgFill(args.vg);

		// Draw a blue circle
		nvgFillColor(args.vg, nvgRGBf(1.0, 1.0, 1.0));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(radiusMiddle - 1.f));
		nvgFill(args.vg);	

		auto& anglesUsed = defaultAnglesUsed;
		if(mModule) {
			anglesUsed = mModule->anglesUsed;
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
		nvgFillColor(args.vg, nvgRGBf(0.0, 0.0, 0.0));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(13.f * 0.33f));
		nvgFill(args.vg);

		// Draw a blue circle
		nvgFillColor(args.vg, nvgRGBf(1.0, 1.0, 1.0));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, centerX, centerY, mm2px(13.f * 0.33f - 1.f));
		nvgFill(args.vg);


	}
};

struct XLimitJIAutotunerWidget : ModuleWidget {
	XLimitJIAutotunerWidget(XLimitJIAutotuner* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/XLimitJIAutotuner.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.5, 37.238)), module, XLimitJIAutotuner::POW2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.5, 37.238)), module, XLimitJIAutotuner::POW3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42.5, 37.238)), module, XLimitJIAutotuner::POW5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(56.5, 37.238)), module, XLimitJIAutotuner::POW7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(70.5, 37.238)), module, XLimitJIAutotuner::POW11_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(84.5, 37.238)), module, XLimitJIAutotuner::POW13_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(98.5, 37.238)), module, XLimitJIAutotuner::POW17_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(112.5, 37.238)), module, XLimitJIAutotuner::POW19_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.65, 97.132)), module, XLimitJIAutotuner::VOCT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.132, 97.132)), module, XLimitJIAutotuner::SQR_OUTPUT));

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