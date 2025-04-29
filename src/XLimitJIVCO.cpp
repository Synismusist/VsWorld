#include "plugin.hpp"

#include <array>
#include <vector>

struct XLimitJIVCO : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		SYNC_INPUT,
		VOCTRES_INPUT,
		VOCT_INPUT,
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
		LIGHTS_LEN
	};	

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

	double generateSine(double phase) {
		return 5.f * std::sin(2.0 * M_PI * phase);
	}

	double generateTriangle(double phase) {
		return 5.0f - 20.0f * std::abs(phase - 0.5f);
	}
	
	double generateSawtoothTable(double phase, double freq, double sampleRate) {
		int harmonicIdx = clamp((int)(0.4 * sampleRate / freq) - 1, 0, harmonicsSize - 1);
		//int tableIdx = phase * tableSize;
		return 5.0 * readWavetable(sawtoothTable, phase * tableSize, harmonicIdx * tableSize);
	}

	double generateSquareTable(double phase, double freq, double sampleRate) {
		int harmonicIdx = clamp((int)(0.4 * sampleRate / freq) - 1, 0, harmonicsSize - 1);
		//return 5.0 * readWavetable(squareTable, tableIdx + harmonicIdx * tableSize);
		return 5.0 * readWavetable(squareTable, phase * tableSize, harmonicIdx * tableSize);
	}

	double hermiteInterpolation(double p0, double p1, double p2, double p3, double frac) {
		double t = frac; // Normalized fractional position
		double t2 = t * t;
		double t3 = t2 * t;
	
		return 0.5 * (
			(2 * p1) +
			(-p0 + p2) * t +
			(2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 +
			(-p0 + 3 * p1 - 3 * p2 + p3) * t3
		);
	}

	double readWavetable(const std::array<double, tableSize * harmonicsSize>& wavetable, double posIdx, int harmonicIdx) {
		int i = static_cast<int>(posIdx); // Integer part of the index
		double frac = posIdx - i;         // Fractional part of the index
	
		// Get neighboring points
		double p0 = wavetable[((i - 1 + tableSize) % tableSize) + harmonicIdx];
		double p1 = wavetable[(i % tableSize) + harmonicIdx];
		double p2 = wavetable[(i + 1) % tableSize + harmonicIdx];
		double p3 = wavetable[(i + 2) % tableSize + harmonicIdx];
	
		return hermiteInterpolation(p0, p1, p2, p3, frac);
	}

	XLimitJIVCO() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(SYNC_INPUT, "Sync");
		configInput(VOCTRES_INPUT, "V/Oct Residual");
		configInput(VOCT_INPUT, "V/Oct");
		configOutput(SIN_OUTPUT, "Sine wave");
		configOutput(TRI_OUTPUT, "Triangle wave");
		configOutput(SAW_OUTPUT, "Sawtooth wave");
		configOutput(SQR_OUTPUT, "Square wave");
		
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

		// sync
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
		
		int channels = std::max(1, inputs[VOCT_INPUT].getChannels());

		outputs[SIN_OUTPUT].setChannels(channels);
		outputs[TRI_OUTPUT].setChannels(channels);
		outputs[SAW_OUTPUT].setChannels(channels);
		outputs[SQR_OUTPUT].setChannels(channels);

		for (int c = 0; c < channels; c++) {
			double vIn = inputs[VOCT_INPUT].getPolyVoltage(c);
			double vRes = inputs[VOCTRES_INPUT].getPolyVoltage(c);
			double voltage = vIn + vRes;
			double harmonicFreq = std::exp2(voltage) * (double)dsp::FREQ_C4;
			
			phases[c] += harmonicFreq * args.sampleTime;
			phases[c] = std::fmod(phases[c], 1.0);

			auto& phase = phases[c];	
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
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/XLimitJIVCO.svg"), 
			asset::plugin(pluginInstance, "res/XLimitJIVCO-dark.svg")
		));

		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(14.132, 34.133)), module, XLimitJIVCO::SYNC_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(14.133, 63.133)), module, XLimitJIVCO::VOCTRES_INPUT));
		addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(14.133, 92.132)), module, XLimitJIVCO::VOCT_INPUT));

		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(36.796, 34.133)), module, XLimitJIVCO::SIN_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(36.795, 53.432)), module, XLimitJIVCO::TRI_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(36.795, 72.732)), module, XLimitJIVCO::SAW_OUTPUT));
		addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(36.795, 92.132)), module, XLimitJIVCO::SQR_OUTPUT));
	}
};


Model* modelXLimitJIVCO = createModel<XLimitJIVCO, XLimitJIVCOWidget>("XLimitJIVCO");