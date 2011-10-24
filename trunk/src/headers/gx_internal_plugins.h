/*
 * Copyright (C) 2009, 2010 Hermann Meyer, James Warden, Andreas Degert
 * Copyright (C) 2011 Pete Shorthose
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */

/* ------- This is the guitarix Engine namespace ------- */

#pragma once

#include <boost/thread/mutex.hpp>

namespace gx_jack { class GxJack; }

namespace gx_engine {

/****************************************************************
 ** MonoMute, StereoMute, MaxLevel, MidiAudioBuffer
 */

class MonoMute: public PluginDef {
private:
    static void process(int count, float *input, float *output, PluginDef*);
public:
    MonoMute();
};

class StereoMute: public PluginDef {
private:
    static void process(int count, float *input0, float *input1,
			float *output0, float *output1, PluginDef*);
public:
    StereoMute();
};

class MaxLevel: public PluginDef {
private:
    static float maxlevel[2];
    static void process(int count, float *input0, float *input1,
			float *output0, float *output1, PluginDef*);
    static int activate(bool start, PluginDef *plugin);
public:
    static float get(unsigned int channel) {
	assert(channel < 2); return maxlevel[channel];
    }
    static void reset() {
	maxlevel[0] = maxlevel[1] = 0;
    }
    MaxLevel();
};

class MidiAudioBuffer: PluginDef {
private:
    static gx_jack::GxJack* jack;
    static void fill_buffer(int count, float *input0, float *output0, PluginDef*);
public:
    Plugin plugin;
    MidiAudioBuffer();
    void set_jack(gx_jack::GxJack* jack_) { jack = jack_; }
};


/****************************************************************
 ** class TunerAdapter
 */

class TunerAdapter: public ModuleSelector, private PluginDef {
private:
    static void feed_tuner(int count, float *input, float *output, PluginDef*);
    static int regparam(const ParamReg& reg);
    static void init(unsigned int samplingFreq, PluginDef *plugin);
    PitchTracker pitch_tracker;
    int state;
    ModuleSequencer& engine;
    enum { tuner_use = 0x01, midi_use = 0x02 };
    void set_and_check(int use, bool on);
    const Plugin& dep_plugin;
public:
    Plugin plugin;
    TunerAdapter(const Plugin& pl, ModuleSequencer& engine);
    void used_for_display(bool on) { set_and_check(tuner_use, on); }
    void used_by_midi(bool on) { set_and_check(midi_use, on); }
    void set_module();
};


/****************************************************************
 ** class NoiseGate
 */

class NoiseGate {
private:
    static PluginDef inputdef;
    static float fnglevel;
    static float ngate;
    static bool off;
    static int noisegate_register(const ParamReg& reg);
    static void inputlevel_compute(int count, float *input0, float *output0, PluginDef*);
    static void outputgate_compute(int count, float *input, float *output, PluginDef*);
    static int outputgate_activate(bool start, PluginDef *pdef);
public:
    static Plugin inputlevel;
    static PluginDef outputgate;
    NoiseGate();
};


/****************************************************************
 ** class OscilloscopeAdapter
 */

class OscilloscopeAdapter: PluginDef {
private:
    static float* buffer;
    static unsigned int size;
    static void fill_buffer(int count, float *input0, float *output0, PluginDef*);
    static int activate(bool start, PluginDef *p);
    void change_buffersize(unsigned int);
public:
    Plugin plugin;
    sigc::signal<int, bool>          activation;
    sigc::signal<void, unsigned int> size_change;
    gx_ui::UiSignalUInt              post_pre_signal;
    gx_ui::UiSignalBool              visible;
    void clear_buffer();
    inline float *get_buffer() { return buffer; }
    OscilloscopeAdapter(gx_ui::GxUI *ui, ModuleSequencer& engine);
};


/****************************************************************
 ** class GxJConvSettings
 */

class GxJConvSettings {
 private:
    // main jconv setting
    string          fIRFile;
    string          fIRDir;

    float           fGain;       // jconv gain
    guint           fOffset;     // offset in IR where to start comvolution
    guint           fLength;     // length of the IR to use for convolution
    guint           fDelay;      // delay when to apply reverb
    Gainline        gainline;
    bool            fGainCor;

    void read_gainline(gx_system::JsonParser& jp);
    void read_favorites(gx_system::JsonParser& jp);
    inline void setIRFile(string name)            { fIRFile = name; }
    inline void setIRDir(string name)             { fIRDir = name; }

 public:
    GxJConvSettings();
    explicit GxJConvSettings(gx_system::JsonParser& jp);

    // getters and setters
    inline string getIRFile() const               { return fIRFile; }
    string getFullIRPath() const;
    inline float           getGain() const        { return fGain; }
    inline guint           getOffset() const      { return fOffset; }
    inline guint           getLength() const      { return fLength; }
    inline guint           getDelay() const       { return fDelay; }
    inline bool            getGainCor() const     { return fGainCor; }
    inline const Gainline& getGainline() const    { return gainline; }
    inline string getIRDir() const                { return fIRDir; }
    void setFullIRPath(string name);

    inline void setGain(float gain)               { fGain       = gain; }
    inline void setGainCor(bool gain)             { fGainCor    = gain; }
    inline void setOffset(guint offs)             { fOffset     = offs; }
    inline void setLength(guint leng)             { fLength     = leng; }
    inline void setDelay(guint del)               { fDelay      = del;  }
    inline void setGainline(const Gainline& gain) { gainline    = gain; }

 public:

    // checkbutton state
    static bool* checkbutton7;
    list<Glib::ustring> faflist;
    typedef list<Glib::ustring>::iterator faf_iterator;
    void writeJSON(gx_system::JsonWriter& w);
};


/****************************************************************
 ** class ConvolverAdapter
 */

class ConvolverAdapter: PluginDef {
private:
    GxConvolver conv;
    boost::mutex activate_mutex;
    ModuleSequencer& engine;
    bool activated;
    // wrapper for the rack order function pointers
    static void convolver(int count, float *input0, float *input1,
			  float *output0, float *output1, PluginDef*);
    static int activate(bool start, PluginDef *pdef);
    static int convolver_register(const ParamReg& reg);
    static void convolver_init(unsigned int samplingFreq, PluginDef *pdef);
    void change_buffersize(unsigned int size);
public:
    Plugin plugin;
    GxJConvSettings jcset;
public:
    ConvolverAdapter(ModuleSequencer& engine);
    void restart();
    bool conv_start();
};


/****************************************************************
 ** class BaseConvolver
 */


class BaseConvolver: protected PluginDef {
protected:
    GxSimpleConvolver conv;
    boost::mutex activate_mutex;
    ModuleSequencer& engine;
    bool activated;
    static void init(unsigned int samplingFreq, PluginDef *p);
    static int activate(bool start, PluginDef *pdef);
    void change_buffersize(unsigned int);
    inline bool is_runnable() { return conv.is_runnable(); }
    inline void set_not_runnable() { conv.set_not_runnable(); }
    int conv_start();
public:
    Plugin plugin;
public:
    BaseConvolver(ModuleSequencer& engine, gx_resample::BufferResampler& resamp);
    virtual ~BaseConvolver();
    virtual bool start(bool force = false) = 0;
    inline void conv_stop() { conv.stop(); }
};

/****************************************************************
 ** class CabinetConvolver
 */

class CabinetConvolver: public BaseConvolver {
private:
    int current_cab;
    float level;
    int cabinet;
    float bass;
    float treble;
    float sum;
    value_pair *cab_names;
    inline void compensate_cab(int count, float *input0, float *output0);
    static void run_cab_conf(int count, float *input, float *output, PluginDef*);
    static int register_cab(const ParamReg& reg);
    bool update();
public:
    CabinetConvolver(ModuleSequencer& engine, gx_resample::BufferResampler& resamp);
    ~CabinetConvolver();
    bool start(bool force = false);
    bool cabinet_changed() { return current_cab != cabinet; }
    void update_cabinet() { current_cab = cabinet; }
    bool sum_changed() { return abs(sum - (level + bass + treble)) > 0.01; }
    void update_sum() { sum = level + bass + treble; }
    bool conv_update();
};


/****************************************************************
 ** class ContrastConvolver
 */

class ContrastConvolver: public BaseConvolver {
private:
    float level;
    float sum;
    // wrapper for the rack order function pointers
    inline void compensate_con(int count, float *input0, float *output0);
    static void run_contrast(int count, float *input, float *output, PluginDef*);
    static int register_con(const ParamReg& reg);
    inline void update_sum() { sum = level; }
public:
    ContrastConvolver(ModuleSequencer& engine, gx_resample::BufferResampler& resamp);
    inline bool sum_changed() { return abs(sum - level) > 0.01; }
    bool start(bool force = false);
};

} // namespace gx_engine
