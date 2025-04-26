#include "plugin.hpp"

#include <array>
#include <vector>

struct XLimitJIVCO : Module {
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
		SYNC_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SIN_OUTPUT,
		TRI_OUTPUT,
		SAW_OUTPUT,
		SQR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PATH1_LIGHT,
		LIGHTS_LEN
	};

	const double log22 = std::log2(2.0);
	const double log23 = std::log2(3.0);
	const double log25 = std::log2(5.0);
	const double log27 = std::log2(7.0);
	const double log211 = std::log2(11.0);
	const double log213 = std::log2(13.0);
	const double log217 = std::log2(17.0);
	const double log219 = std::log2(19.0);

	std::vector<double> voltageList;

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

	static constexpr int tableSize = 10000;
	static constexpr int harmonicsSize = 1000;

	std::array<double, tableSize * harmonicsSize> sawtoothTable;
	std::array<double, tableSize * harmonicsSize> squareTable;

	void buildSawtoothTables() {
		for(int ix = 0; ix < tableSize; ix++) {
			double phase = static_cast<double>(ix) / tableSize;
			double val = 0.0;
			for(int ih = 0; ih < harmonicsSize; ih++) {
				int harmonic = ih + 1;
				val += std::sin(2.0 * M_PI * phase * harmonic) / harmonic;
				sawtoothTable[ix + ih * tableSize] = val * 2.0 / M_PI * 5.0 / 6.0;	
			}
		}
	}

	void buildSquareTables() {
		for(int ix = 0; ix < tableSize; ix++) {
			double phase = static_cast<double>(ix) / tableSize;
			double val = 0.0;
			for(int ih = 0; ih < harmonicsSize; ih++) {
				int harmonic = ih + 1;
				if(harmonic & 1){
					val += std::sin(2.0 * M_PI * phase * harmonic) / harmonic;
				}
				squareTable[ix + ih * tableSize] = val * 2.0 / M_PI / 3.0 * 5.0;	
			}
		}
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

	double generateSine(double phase) {
		return 5.f * std::sin(2.0 * M_PI * phase);
	}

	double generateTriangle(double phase) {
		return 5.0f - 20.0f * std::abs(phase - 0.5f);
	}
	
	double generateSawtoothTable(double phase, double freq, double sampleRate) {
		int harmonicIdx = clamp((int)(0.4 * sampleRate / freq), 0, harmonicsSize - 1);
		int tableIdx = phase * tableSize;
		return 5.0 * sawtoothTable[tableIdx + harmonicIdx * tableSize];
	}

	double generateSquareTable(double phase, double freq, double sampleRate) {
		int harmonicIdx = clamp((int)(0.4 * sampleRate / freq), 0, harmonicsSize - 1);
		int tableIdx = phase * tableSize;
		return 5.0 * squareTable[tableIdx + harmonicIdx * tableSize];
	}

	XLimitJIVCO() {
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
		configInput(SYNC_INPUT, "Sync");
		
		configLight(PATH1_LIGHT, "Emergency stop");

		configOutput(SIN_OUTPUT, "Sine wave");
		configOutput(TRI_OUTPUT, "Triangle wave");
		configOutput(SAW_OUTPUT, "Sawtooth wave");
		configOutput(SQR_OUTPUT, "Square wave");
		
		voltageList.reserve(1e7);
		lights[PATH1_LIGHT].setBrightness(0.f);

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

		buildSawtoothTables();
		buildSquareTables();
	}

	std::array<bool, 8> syncState{
		false,
		false,
		false,
		false,
		false,
		false,
		false,
		false
	};

	std::array<double, 8> phases{
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
	};

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
		int syncChannels = inputs[SYNC_INPUT].getChannels();

		if(syncChannels == 1) {
			float syncVal = inputs[SYNC_INPUT].getPolyVoltage(0);
			if(syncVal && !syncState[0]){
				phases = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
			}
			syncState[0] = syncVal >= 0.5;

		} else if(syncChannels > 1) {
			for(int ii = 0; ii < syncChannels; ii++){
				float syncVal = inputs[SYNC_INPUT].getPolyVoltage(ii);
				if(syncVal && !syncState[ii]){
					phases[ii] = 0.0;
				}
				syncState[ii] = syncVal >= 0.5;
			}
		}

		outputs[SIN_OUTPUT].setChannels(channels);
		outputs[TRI_OUTPUT].setChannels(channels);
		outputs[SAW_OUTPUT].setChannels(channels);
		outputs[SQR_OUTPUT].setChannels(channels);
		
		double baseVoltage = inputs[VOCT_INPUT].getPolyVoltage(0);
		double baseFreq = std::exp2(baseVoltage) * (double)dsp::FREQ_C4;

		phases[0] += baseFreq * args.sampleTime;
		phases[0] = std::fmod(phases[0], 1.0);
		//phases[0] = std::fmod(phases[0], 120.0);
		//double phase = std::fmod(phases[0], 1.0);
		double phase = phases[0];

		outputs[SIN_OUTPUT].setVoltage(generateSine(phase), 0);
		outputs[TRI_OUTPUT].setVoltage(generateTriangle(phase), 0);
		outputs[SAW_OUTPUT].setVoltage(generateSawtoothTable(phase, baseFreq, args.sampleRate), 0);
		outputs[SQR_OUTPUT].setVoltage(generateSquareTable(phase, baseFreq, args.sampleRate), 0);

		for (int c = 1; c < channels; c++) {
			double currVoltage = inputs[VOCT_INPUT].getPolyVoltage(c);
			double harmonicVoltage = baseVoltage + findClosestInSorted(currVoltage - baseVoltage);
			double harmonicFreq = std::exp2(harmonicVoltage) * (double)dsp::FREQ_C4;
			
			phases[c] += harmonicFreq * args.sampleTime;
			phases[c] = std::fmod(phases[c], 1.0);

			phase = phases[c];
			//phase = phases[0] * harmonicFreq / baseFreq;
			//phase = std::fmod(phase, 1.0);			

			outputs[SIN_OUTPUT].setVoltage(generateSine(phase), c);
			outputs[TRI_OUTPUT].setVoltage(generateTriangle(phase), c);
			outputs[SAW_OUTPUT].setVoltage(generateSawtoothTable(phase, harmonicFreq, args.sampleRate), c);
			outputs[SQR_OUTPUT].setVoltage(generateSquareTable(phase, harmonicFreq, args.sampleRate), c);
		}

	}
};


struct XLimitJIVCOWidget : ModuleWidget {
	XLimitJIVCOWidget(XLimitJIVCO* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/XLimitJIVCO.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.5, 37.238)), module, XLimitJIVCO::POW2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.5, 37.238)), module, XLimitJIVCO::POW3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42.5, 37.238)), module, XLimitJIVCO::POW5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(56.5, 37.238)), module, XLimitJIVCO::POW7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(70.5, 37.238)), module, XLimitJIVCO::POW11_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(84.5, 37.238)), module, XLimitJIVCO::POW13_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(98.5, 37.238)), module, XLimitJIVCO::POW17_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(112.5, 37.238)), module, XLimitJIVCO::POW19_PARAM));
		
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(109.207, 24.133)), module, XLimitJIVCO::PATH1_LIGHT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.65, 97.132)), module, XLimitJIVCO::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.546, 97.132)), module, XLimitJIVCO::SYNC_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(76.132, 97.132)), module, XLimitJIVCO::SIN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(88.632, 97.132)), module, XLimitJIVCO::TRI_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(101.632, 97.132)), module, XLimitJIVCO::SAW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(114.632, 97.132)), module, XLimitJIVCO::SQR_OUTPUT));
	}
};


Model* modelXLimitJIVCO = createModel<XLimitJIVCO, XLimitJIVCOWidget>("XLimitJIVCO");