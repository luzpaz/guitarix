namespace chorus_mono {
// generated from file '../src/faust/chorus_mono.dsp'

volatile bool inited = false;
class SIG0 {
  private:
	int 	fSamplingFreq;
	int 	iRec1[2];
  public:
	int getNumInputs() 	{ return 0; }
	int getNumOutputs() 	{ return 1; }
	void init(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		for (int i=0; i<2; i++) iRec1[i] = 0;
	}
	void fill (int count, float output[]) {
		for (int i=0; i<count; i++) {
			iRec1[0] = (1 + iRec1[1]);
			output[i] = sinf((9.587379924285257e-05f * (iRec1[0] - 1)));
			// post processing
			iRec1[1] = iRec1[0];
		}
	}
};
int 	IOTA;
float *fVec0;
FAUSTFLOAT 	fslider0;
float 	fConst0;
float 	fRec0[2];
static float 	ftbl0[65536];
float 	fConst1;
FAUSTFLOAT 	fslider1;
int	fSamplingFreq;

void init(int samplingFreq)
{
	if (!fVec0) fVec0 = new float[65536];
	SIG0 sig0;
	sig0.init(samplingFreq);
	sig0.fill(65536,ftbl0);
	fSamplingFreq = samplingFreq;
	IOTA = 0;
	for (int i=0; i<65536; i++) fVec0[i] = 0;
	fConst0 = (1.0f / fSamplingFreq);
	for (int i=0; i<2; i++) fRec0[i] = 0;
	fConst1 = (0.01f * fSamplingFreq);
	inited = true;
}

void mem_free()
{
	inited = false;
	jack_sync();
	if (fVec0) { delete fVec0; fVec0 = 0; }
}

inline bool is_inited()
{
    return inited;
}


void activate(bool start, int samplingFreq)
{
	if (start) {
		if (!is_inited()) {
			init(samplingFreq);
		}
	} else {
		if (is_inited()) {
			mem_free();
		}
	}
}

void compute(int count, float *input0, float *output0)
{
	float 	fSlow0 = (fConst0 * fslider0);
	float 	fSlow1 = fslider1;
	for (int i=0; i<count; i++) {
		float fTemp0 = (float)input0[i];
		fVec0[IOTA&65535] = fTemp0;
		float fTemp1 = (fSlow0 + fRec0[1]);
		fRec0[0] = (fTemp1 - floorf(fTemp1));
		float fTemp2 = (65536 * (fRec0[0] - floorf(fRec0[0])));
		float fTemp3 = floorf(fTemp2);
		int iTemp4 = int(fTemp3);
		float fTemp5 = (fConst1 * (1 + (0.02f * ((ftbl0[((1 + iTemp4) & 65535)] * (fTemp2 - fTemp3)) + (ftbl0[(iTemp4 & 65535)] * ((1 + fTemp3) - fTemp2))))));
		int iTemp6 = int(fTemp5);
		int iTemp7 = (1 + iTemp6);
		output0[i] = (FAUSTFLOAT)(fVec0[IOTA&65535] + (fSlow1 * (((fTemp5 - iTemp6) * fVec0[(IOTA-int((int(iTemp7) & 65535)))&65535]) + ((iTemp7 - fTemp5) * fVec0[(IOTA-int((iTemp6 & 65535)))&65535]))));
		// post processing
		fRec0[1] = fRec0[0];
		IOTA = IOTA+1;
	}
}

static struct RegisterParams { RegisterParams(); } RegisterParams;
RegisterParams::RegisterParams()
{
	registerVar("chorus_mono.level","","S","",&fslider1, 0.5f, 0.0f, 1.0f, 0.01f);
	registerVar("chorus_mono.freq","","S","",&fslider0, 2.0f, 0.0f, 1e+01f, 0.01f);
	registerInit("chorus_mono", init);
}

} // end namespace chorus_mono
