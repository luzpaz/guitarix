declare id "Ampeg"; // in tonestack selector
declare name "Ampeg Style";

tstack = component("tonestack.dsp");
process = tstack[tse=tstack.ts.ampeg;];
