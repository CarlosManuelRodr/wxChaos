
WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS =  `wx-config --cflags` `pkg-config --cflags gtk+-2.0` -DUNICODE -D_UNICODE
RESINC = 
LIBDIR = 
LIB =  -lsfml-system -lsfml-window -lsfml-graphics
LDFLAGS =  `wx-config --libs` `pkg-config --libs gtk+-2.0 x11 xrandr`

INC_RELEASE =  $(INC)
CFLAGS_RELEASE =  $(CFLAGS) -DBUILD_SETUP -O2 
RESINC_RELEASE =  $(RESINC)
RCFLAGS_RELEASE =  $(RCFLAGS)
LIBDIR_RELEASE =  $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE =  -s $(LDFLAGS)
OBJDIR_RELEASE = obj
DEP_RELEASE = 
OUT_RELEASE = bin/wxchaos

INC_NO_SETUP =  $(INC)
CFLAGS_NO_SETUP =  $(CFLAGS) -O2
RESINC_NO_SETUP =  $(RESINC)
RCFLAGS_NO_SETUP =  $(RCFLAGS)
LIBDIR_NO_SETUP =  $(LIBDIR)
LIB_NO_SETUP = $(LIB)
LDFLAGS_NO_SETUP =  -s $(LDFLAGS)
OBJDIR_NO_SETUP = obj/no_setup
DEP_NO_SETUP = 
OUT_NO_SETUP = bin/wxchaos


OBJ_RELEASE = $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncCommon.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncMatrix.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncNonCmplx.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncStr.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpICallback.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIOprt.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncCmplx.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIPackage.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIToken.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIValReader.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIValue.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIfThenElse.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_typeinfo.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptobject.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_string.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_string_util.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_thread.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_tokenizer.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_variablescope.o $(OBJDIR_RELEASE)/wxChaos/global.o $(OBJDIR_RELEASE)/wxChaos/gradient.o $(OBJDIR_RELEASE)/wxChaos/gradientdlg.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpError.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpParserBase.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpRPN.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpScriptTokens.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpTest.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpTokenReader.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpValReader.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpValue.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpValueCache.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpVariable.o $(OBJDIR_RELEASE)/wxChaos/wxMathPlot/mathplot.o $(OBJDIR_RELEASE)/wxChaos/wxSFMLCanvas.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageCmplx.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtBinAssign.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtBinCommon.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtCmplx.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtIndex.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtMatrix.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtNonCmplx.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageCommon.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageMatrix.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageNonCmplx.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageStr.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageUnit.o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpParser.o $(OBJDIR_RELEASE)/wxChaos/FractalTypes.o $(OBJDIR_RELEASE)/wxChaos/HTMLViewer.o $(OBJDIR_RELEASE)/wxChaos/IterDialog.o $(OBJDIR_RELEASE)/wxChaos/JuliaMode.o $(OBJDIR_RELEASE)/wxChaos/MainWindow.o $(OBJDIR_RELEASE)/wxChaos/SizeDialogSave.o $(OBJDIR_RELEASE)/wxChaos/FractalGUI.o $(OBJDIR_RELEASE)/wxChaos/StringFuncs.o $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptarray/scriptarray.o $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.o $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_atomic.o $(OBJDIR_RELEASE)/wxChaos/AngelScriptFunc.o $(OBJDIR_RELEASE)/wxChaos/BinaryOps.o $(OBJDIR_RELEASE)/wxChaos/BmpWriter.o $(OBJDIR_RELEASE)/wxChaos/ColorFrame.o $(OBJDIR_RELEASE)/wxChaos/CommandFrame.o $(OBJDIR_RELEASE)/wxChaos/ConfigParser.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_builder.o $(OBJDIR_RELEASE)/wxChaos/ConstDialog.o $(OBJDIR_RELEASE)/wxChaos/DPExplorer.o $(OBJDIR_RELEASE)/wxChaos/DimensionFrame.o $(OBJDIR_RELEASE)/wxChaos/FormulaDialog.o $(OBJDIR_RELEASE)/wxChaos/FractalCanvas.o $(OBJDIR_RELEASE)/wxChaos/FractalClasses.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_gc.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_generic.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_globalproperty.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_memory.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_module.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_objecttype.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_outputbuffer.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_parser.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_restore.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptcode.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptengine.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptfunction.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptnode.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_msvc.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_bytecode.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_arm.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_mips.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_ppc.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_ppc_64.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_sh4.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_gcc.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_mingw.o $(OBJDIR_RELEASE)/wxChaos/About.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x86.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_xenon.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_compiler.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_configgroup.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_context.o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_datatype.o

OBJ_NO_SETUP = $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncCommon.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncMatrix.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncNonCmplx.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncStr.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpICallback.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIOprt.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncCmplx.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIPackage.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIToken.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIValReader.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIValue.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIfThenElse.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_typeinfo.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptobject.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_string.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_string_util.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_thread.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_tokenizer.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_variablescope.o $(OBJDIR_NO_SETUP)/wxChaos/global.o $(OBJDIR_NO_SETUP)/wxChaos/gradient.o $(OBJDIR_NO_SETUP)/wxChaos/gradientdlg.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpError.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpParserBase.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpRPN.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpScriptTokens.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpTest.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpTokenReader.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValReader.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValue.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValueCache.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpVariable.o $(OBJDIR_NO_SETUP)/wxChaos/wxMathPlot/mathplot.o $(OBJDIR_NO_SETUP)/wxChaos/wxSFMLCanvas.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageCmplx.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtBinAssign.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtBinCommon.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtCmplx.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtIndex.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtMatrix.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtNonCmplx.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageCommon.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageMatrix.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageNonCmplx.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageStr.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageUnit.o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpParser.o $(OBJDIR_NO_SETUP)/wxChaos/FractalTypes.o $(OBJDIR_NO_SETUP)/wxChaos/HTMLViewer.o $(OBJDIR_NO_SETUP)/wxChaos/IterDialog.o $(OBJDIR_NO_SETUP)/wxChaos/JuliaMode.o $(OBJDIR_NO_SETUP)/wxChaos/MainWindow.o $(OBJDIR_NO_SETUP)/wxChaos/SizeDialogSave.o $(OBJDIR_NO_SETUP)/wxChaos/FractalGUI.o $(OBJDIR_NO_SETUP)/wxChaos/StringFuncs.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptarray/scriptarray.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_atomic.o $(OBJDIR_NO_SETUP)/wxChaos/AngelScriptFunc.o $(OBJDIR_NO_SETUP)/wxChaos/BinaryOps.o $(OBJDIR_NO_SETUP)/wxChaos/BmpWriter.o $(OBJDIR_NO_SETUP)/wxChaos/ColorFrame.o $(OBJDIR_NO_SETUP)/wxChaos/CommandFrame.o $(OBJDIR_NO_SETUP)/wxChaos/ConfigParser.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_builder.o $(OBJDIR_NO_SETUP)/wxChaos/ConstDialog.o $(OBJDIR_NO_SETUP)/wxChaos/DPExplorer.o $(OBJDIR_NO_SETUP)/wxChaos/DimensionFrame.o $(OBJDIR_NO_SETUP)/wxChaos/FormulaDialog.o $(OBJDIR_NO_SETUP)/wxChaos/FractalCanvas.o $(OBJDIR_NO_SETUP)/wxChaos/FractalClasses.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_gc.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_generic.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_globalproperty.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_memory.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_module.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_objecttype.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_outputbuffer.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_parser.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_restore.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptcode.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptengine.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptfunction.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptnode.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_msvc.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_bytecode.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_arm.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_mips.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_ppc.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_ppc_64.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_sh4.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_gcc.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_mingw.o $(OBJDIR_NO_SETUP)/wxChaos/About.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x86.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_xenon.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_compiler.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_configgroup.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_context.o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_datatype.o

all: release

clean: clean_release


before_release: 
	@echo ""
	@echo "#########################################################"
	@echo "#                                                       #"
	@echo "#   Building wxChaos                                    #"
	@echo "#                                                       #"
	@echo "#########################################################"
	@echo ""
	test -d bin/ || mkdir -p bin/
	test -d $(OBJDIR_RELEASE)/wxChaos/muParserX || mkdir -p $(OBJDIR_RELEASE)/wxChaos/muParserX
	test -d $(OBJDIR_RELEASE)/wxChaos/angelscript/src || mkdir -p $(OBJDIR_RELEASE)/wxChaos/angelscript/src
	test -d $(OBJDIR_RELEASE)/wxChaos || mkdir -p $(OBJDIR_RELEASE)/wxChaos
	test -d $(OBJDIR_RELEASE)/wxChaos/wxMathPlot || mkdir -p $(OBJDIR_RELEASE)/wxChaos/wxMathPlot
	test -d $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptarray || mkdir -p $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptarray
	test -d $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring || mkdir -p $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring

after_release: 
	@echo ""
	@echo "#########################################################"
	@echo "#                                                       #"
	@echo "#   Done. To install type make install as root.         #"
	@echo "#                                                       #"
	@echo "#########################################################"
	@echo ""

release: before_release out_release after_release

out_release: $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(OBJ_RELEASE) $(LDFLAGS_RELEASE) $(LIBDIR_RELEASE) $(LIB_RELEASE) -o $(OUT_RELEASE)


$(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncCommon.o: wxChaos/muParserX/mpFuncCommon.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpFuncCommon.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncCommon.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncMatrix.o: wxChaos/muParserX/mpFuncMatrix.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpFuncMatrix.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncMatrix.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncNonCmplx.o: wxChaos/muParserX/mpFuncNonCmplx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpFuncNonCmplx.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncNonCmplx.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncStr.o: wxChaos/muParserX/mpFuncStr.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpFuncStr.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncStr.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpICallback.o: wxChaos/muParserX/mpICallback.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpICallback.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpICallback.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpIOprt.o: wxChaos/muParserX/mpIOprt.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpIOprt.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIOprt.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncCmplx.o: wxChaos/muParserX/mpFuncCmplx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpFuncCmplx.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpFuncCmplx.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpIPackage.o: wxChaos/muParserX/mpIPackage.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpIPackage.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIPackage.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpIToken.o: wxChaos/muParserX/mpIToken.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpIToken.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIToken.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpIValReader.o: wxChaos/muParserX/mpIValReader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpIValReader.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIValReader.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpIValue.o: wxChaos/muParserX/mpIValue.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpIValue.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIValue.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpIfThenElse.o: wxChaos/muParserX/mpIfThenElse.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpIfThenElse.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpIfThenElse.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_typeinfo.o: wxChaos/angelscript/src/as_typeinfo.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_typeinfo.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_typeinfo.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptobject.o: wxChaos/angelscript/src/as_scriptobject.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_scriptobject.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptobject.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_string.o: wxChaos/angelscript/src/as_string.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_string.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_string.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_string_util.o: wxChaos/angelscript/src/as_string_util.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_string_util.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_string_util.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_thread.o: wxChaos/angelscript/src/as_thread.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_thread.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_thread.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_tokenizer.o: wxChaos/angelscript/src/as_tokenizer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_tokenizer.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_tokenizer.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_variablescope.o: wxChaos/angelscript/src/as_variablescope.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_variablescope.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_variablescope.o

$(OBJDIR_RELEASE)/wxChaos/global.o: wxChaos/global.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/global.cpp -o $(OBJDIR_RELEASE)/wxChaos/global.o

$(OBJDIR_RELEASE)/wxChaos/gradient.o: wxChaos/gradient.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/gradient.cpp -o $(OBJDIR_RELEASE)/wxChaos/gradient.o

$(OBJDIR_RELEASE)/wxChaos/gradientdlg.o: wxChaos/gradientdlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/gradientdlg.cpp -o $(OBJDIR_RELEASE)/wxChaos/gradientdlg.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpError.o: wxChaos/muParserX/mpError.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpError.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpError.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpParserBase.o: wxChaos/muParserX/mpParserBase.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpParserBase.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpParserBase.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpRPN.o: wxChaos/muParserX/mpRPN.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpRPN.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpRPN.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpScriptTokens.o: wxChaos/muParserX/mpScriptTokens.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpScriptTokens.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpScriptTokens.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpTest.o: wxChaos/muParserX/mpTest.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpTest.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpTest.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpTokenReader.o: wxChaos/muParserX/mpTokenReader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpTokenReader.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpTokenReader.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpValReader.o: wxChaos/muParserX/mpValReader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpValReader.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpValReader.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpValue.o: wxChaos/muParserX/mpValue.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpValue.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpValue.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpValueCache.o: wxChaos/muParserX/mpValueCache.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpValueCache.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpValueCache.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpVariable.o: wxChaos/muParserX/mpVariable.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpVariable.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpVariable.o

$(OBJDIR_RELEASE)/wxChaos/wxMathPlot/mathplot.o: wxChaos/wxMathPlot/mathplot.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/wxMathPlot/mathplot.cpp -o $(OBJDIR_RELEASE)/wxChaos/wxMathPlot/mathplot.o

$(OBJDIR_RELEASE)/wxChaos/wxSFMLCanvas.o: wxChaos/wxSFMLCanvas.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/wxSFMLCanvas.cpp -o $(OBJDIR_RELEASE)/wxChaos/wxSFMLCanvas.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageCmplx.o: wxChaos/muParserX/mpPackageCmplx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpPackageCmplx.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageCmplx.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtBinAssign.o: wxChaos/muParserX/mpOprtBinAssign.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpOprtBinAssign.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtBinAssign.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtBinCommon.o: wxChaos/muParserX/mpOprtBinCommon.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpOprtBinCommon.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtBinCommon.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtCmplx.o: wxChaos/muParserX/mpOprtCmplx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpOprtCmplx.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtCmplx.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtIndex.o: wxChaos/muParserX/mpOprtIndex.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpOprtIndex.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtIndex.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtMatrix.o: wxChaos/muParserX/mpOprtMatrix.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpOprtMatrix.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtMatrix.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtNonCmplx.o: wxChaos/muParserX/mpOprtNonCmplx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpOprtNonCmplx.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpOprtNonCmplx.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageCommon.o: wxChaos/muParserX/mpPackageCommon.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpPackageCommon.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageCommon.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageMatrix.o: wxChaos/muParserX/mpPackageMatrix.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpPackageMatrix.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageMatrix.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageNonCmplx.o: wxChaos/muParserX/mpPackageNonCmplx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpPackageNonCmplx.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageNonCmplx.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageStr.o: wxChaos/muParserX/mpPackageStr.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpPackageStr.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageStr.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageUnit.o: wxChaos/muParserX/mpPackageUnit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpPackageUnit.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpPackageUnit.o

$(OBJDIR_RELEASE)/wxChaos/muParserX/mpParser.o: wxChaos/muParserX/mpParser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/muParserX/mpParser.cpp -o $(OBJDIR_RELEASE)/wxChaos/muParserX/mpParser.o

$(OBJDIR_RELEASE)/wxChaos/FractalTypes.o: wxChaos/FractalTypes.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/FractalTypes.cpp -o $(OBJDIR_RELEASE)/wxChaos/FractalTypes.o

$(OBJDIR_RELEASE)/wxChaos/HTMLViewer.o: wxChaos/HTMLViewer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/HTMLViewer.cpp -o $(OBJDIR_RELEASE)/wxChaos/HTMLViewer.o

$(OBJDIR_RELEASE)/wxChaos/IterDialog.o: wxChaos/IterDialog.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/IterDialog.cpp -o $(OBJDIR_RELEASE)/wxChaos/IterDialog.o

$(OBJDIR_RELEASE)/wxChaos/JuliaMode.o: wxChaos/JuliaMode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/JuliaMode.cpp -o $(OBJDIR_RELEASE)/wxChaos/JuliaMode.o

$(OBJDIR_RELEASE)/wxChaos/MainWindow.o: wxChaos/MainWindow.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/MainWindow.cpp -o $(OBJDIR_RELEASE)/wxChaos/MainWindow.o

$(OBJDIR_RELEASE)/wxChaos/SizeDialogSave.o: wxChaos/SizeDialogSave.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/SizeDialogSave.cpp -o $(OBJDIR_RELEASE)/wxChaos/SizeDialogSave.o

$(OBJDIR_RELEASE)/wxChaos/FractalGUI.o: wxChaos/FractalGUI.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/FractalGUI.cpp -o $(OBJDIR_RELEASE)/wxChaos/FractalGUI.o

$(OBJDIR_RELEASE)/wxChaos/StringFuncs.o: wxChaos/StringFuncs.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/StringFuncs.cpp -o $(OBJDIR_RELEASE)/wxChaos/StringFuncs.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptarray/scriptarray.o: wxChaos/angelscript/add_on/scriptarray/scriptarray.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/add_on/scriptarray/scriptarray.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptarray/scriptarray.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.o: wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.o: wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_atomic.o: wxChaos/angelscript/src/as_atomic.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_atomic.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_atomic.o

$(OBJDIR_RELEASE)/wxChaos/AngelScriptFunc.o: wxChaos/AngelScriptFunc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/AngelScriptFunc.cpp -o $(OBJDIR_RELEASE)/wxChaos/AngelScriptFunc.o

$(OBJDIR_RELEASE)/wxChaos/BinaryOps.o: wxChaos/BinaryOps.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/BinaryOps.cpp -o $(OBJDIR_RELEASE)/wxChaos/BinaryOps.o

$(OBJDIR_RELEASE)/wxChaos/BmpWriter.o: wxChaos/BmpWriter.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/BmpWriter.cpp -o $(OBJDIR_RELEASE)/wxChaos/BmpWriter.o

$(OBJDIR_RELEASE)/wxChaos/ColorFrame.o: wxChaos/ColorFrame.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/ColorFrame.cpp -o $(OBJDIR_RELEASE)/wxChaos/ColorFrame.o

$(OBJDIR_RELEASE)/wxChaos/CommandFrame.o: wxChaos/CommandFrame.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/CommandFrame.cpp -o $(OBJDIR_RELEASE)/wxChaos/CommandFrame.o

$(OBJDIR_RELEASE)/wxChaos/ConfigParser.o: wxChaos/ConfigParser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/ConfigParser.cpp -o $(OBJDIR_RELEASE)/wxChaos/ConfigParser.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_builder.o: wxChaos/angelscript/src/as_builder.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_builder.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_builder.o

$(OBJDIR_RELEASE)/wxChaos/ConstDialog.o: wxChaos/ConstDialog.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/ConstDialog.cpp -o $(OBJDIR_RELEASE)/wxChaos/ConstDialog.o

$(OBJDIR_RELEASE)/wxChaos/DPExplorer.o: wxChaos/DPExplorer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/DPExplorer.cpp -o $(OBJDIR_RELEASE)/wxChaos/DPExplorer.o

$(OBJDIR_RELEASE)/wxChaos/DimensionFrame.o: wxChaos/DimensionFrame.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/DimensionFrame.cpp -o $(OBJDIR_RELEASE)/wxChaos/DimensionFrame.o

$(OBJDIR_RELEASE)/wxChaos/FormulaDialog.o: wxChaos/FormulaDialog.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/FormulaDialog.cpp -o $(OBJDIR_RELEASE)/wxChaos/FormulaDialog.o

$(OBJDIR_RELEASE)/wxChaos/FractalCanvas.o: wxChaos/FractalCanvas.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/FractalCanvas.cpp -o $(OBJDIR_RELEASE)/wxChaos/FractalCanvas.o

$(OBJDIR_RELEASE)/wxChaos/FractalClasses.o: wxChaos/FractalClasses.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/FractalClasses.cpp -o $(OBJDIR_RELEASE)/wxChaos/FractalClasses.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_gc.o: wxChaos/angelscript/src/as_gc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_gc.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_gc.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_generic.o: wxChaos/angelscript/src/as_generic.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_generic.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_generic.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_globalproperty.o: wxChaos/angelscript/src/as_globalproperty.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_globalproperty.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_globalproperty.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_memory.o: wxChaos/angelscript/src/as_memory.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_memory.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_memory.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_module.o: wxChaos/angelscript/src/as_module.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_module.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_module.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_objecttype.o: wxChaos/angelscript/src/as_objecttype.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_objecttype.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_objecttype.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_outputbuffer.o: wxChaos/angelscript/src/as_outputbuffer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_outputbuffer.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_outputbuffer.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_parser.o: wxChaos/angelscript/src/as_parser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_parser.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_parser.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_restore.o: wxChaos/angelscript/src/as_restore.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_restore.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_restore.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptcode.o: wxChaos/angelscript/src/as_scriptcode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_scriptcode.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptcode.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptengine.o: wxChaos/angelscript/src/as_scriptengine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_scriptengine.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptengine.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptfunction.o: wxChaos/angelscript/src/as_scriptfunction.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_scriptfunction.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptfunction.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptnode.o: wxChaos/angelscript/src/as_scriptnode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_scriptnode.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_scriptnode.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_msvc.o: wxChaos/angelscript/src/as_callfunc_x64_msvc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_x64_msvc.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_msvc.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_bytecode.o: wxChaos/angelscript/src/as_bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_bytecode.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_bytecode.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc.o: wxChaos/angelscript/src/as_callfunc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_arm.o: wxChaos/angelscript/src/as_callfunc_arm.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_arm.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_arm.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_mips.o: wxChaos/angelscript/src/as_callfunc_mips.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_mips.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_mips.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_ppc.o: wxChaos/angelscript/src/as_callfunc_ppc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_ppc.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_ppc.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_ppc_64.o: wxChaos/angelscript/src/as_callfunc_ppc_64.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_ppc_64.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_ppc_64.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_sh4.o: wxChaos/angelscript/src/as_callfunc_sh4.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_sh4.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_sh4.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_gcc.o: wxChaos/angelscript/src/as_callfunc_x64_gcc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_x64_gcc.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_gcc.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_mingw.o: wxChaos/angelscript/src/as_callfunc_x64_mingw.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_x64_mingw.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x64_mingw.o

$(OBJDIR_RELEASE)/wxChaos/About.o: wxChaos/About.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/About.cpp -o $(OBJDIR_RELEASE)/wxChaos/About.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x86.o: wxChaos/angelscript/src/as_callfunc_x86.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_x86.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_x86.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_xenon.o: wxChaos/angelscript/src/as_callfunc_xenon.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_callfunc_xenon.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_callfunc_xenon.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_compiler.o: wxChaos/angelscript/src/as_compiler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_compiler.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_compiler.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_configgroup.o: wxChaos/angelscript/src/as_configgroup.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_configgroup.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_configgroup.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_context.o: wxChaos/angelscript/src/as_context.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_context.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_context.o

$(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_datatype.o: wxChaos/angelscript/src/as_datatype.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c wxChaos/angelscript/src/as_datatype.cpp -o $(OBJDIR_RELEASE)/wxChaos/angelscript/src/as_datatype.o

clean_release: 
	@echo "########################################################"
	@echo "#                                                      #"
	@echo "#  Cleaning up                                         #"
	@echo "#                                                      #"
	@echo "########################################################"
	@echo ""
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/wxChaos/muParserX
	rm -rf $(OBJDIR_RELEASE)/wxChaos/angelscript/src
	rm -rf $(OBJDIR_RELEASE)/wxChaos
	rm -rf $(OBJDIR_RELEASE)/wxChaos/wxMathPlot
	rm -rf $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptarray
	rm -rf $(OBJDIR_RELEASE)/wxChaos/angelscript/add_on/scriptstdstring


#### NO SETUP ####
before_no_setup: 
	@echo ""
	@echo "#########################################################"
	@echo "#                                                       #"
	@echo "#   Building wxChaos (No Setup)                         #"
	@echo "#                                                       #"
	@echo "#########################################################"
	@echo ""
	test -d bin/ || mkdir -p bin/
	test -d $(OBJDIR_NO_SETUP)/wxChaos/muParserX || mkdir -p $(OBJDIR_NO_SETUP)/wxChaos/muParserX
	test -d $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src || mkdir -p $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src
	test -d $(OBJDIR_NO_SETUP)/wxChaos || mkdir -p $(OBJDIR_NO_SETUP)/wxChaos
	test -d $(OBJDIR_NO_SETUP)/wxChaos/wxMathPlot || mkdir -p $(OBJDIR_NO_SETUP)/wxChaos/wxMathPlot
	test -d $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptarray || mkdir -p $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptarray
	test -d $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring || mkdir -p $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring
	test -d $(OUT_RELEASE)/Doc || cp -r app_resources/English/Doc bin/
	test -d $(OUT_RELEASE)/Resources || cp -r app_resources/English/Resources bin/
	test -d $(OUT_RELEASE)/ScriptSamples || cp -r app_resources/English/ScriptSamples bin/
	test -d $(OUT_RELEASE)/UserScripts || cp -r app_resources/English/UserScripts bin/
	test -e $(OUT_RELEASE)/config.ini || cp app_resources/English/config.ini bin/

after_no_setup: 
	@echo ""
	@echo "#########################################################"
	@echo "#                                                       #"
	@echo "#   Done.                                               #"
	@echo "#                                                       #"
	@echo "#########################################################"
	@echo ""

no_setup: before_no_setup out_no_setup after_no_setup


out_no_setup: $(OBJ_NO_SETUP) $(DEP_NO_SETUP)
	$(LD) $(OBJ_NO_SETUP) $(LDFLAGS_NO_SETUP) $(LIBDIR_NO_SETUP) $(LIB_NO_SETUP) -o $(OUT_NO_SETUP)

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncCommon.o: wxChaos/muParserX/mpFuncCommon.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpFuncCommon.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncCommon.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncMatrix.o: wxChaos/muParserX/mpFuncMatrix.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpFuncMatrix.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncMatrix.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncNonCmplx.o: wxChaos/muParserX/mpFuncNonCmplx.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpFuncNonCmplx.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncNonCmplx.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncStr.o: wxChaos/muParserX/mpFuncStr.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpFuncStr.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncStr.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpICallback.o: wxChaos/muParserX/mpICallback.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpICallback.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpICallback.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIOprt.o: wxChaos/muParserX/mpIOprt.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpIOprt.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIOprt.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncCmplx.o: wxChaos/muParserX/mpFuncCmplx.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpFuncCmplx.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpFuncCmplx.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIPackage.o: wxChaos/muParserX/mpIPackage.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpIPackage.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIPackage.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIToken.o: wxChaos/muParserX/mpIToken.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpIToken.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIToken.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIValReader.o: wxChaos/muParserX/mpIValReader.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpIValReader.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIValReader.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIValue.o: wxChaos/muParserX/mpIValue.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpIValue.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIValue.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIfThenElse.o: wxChaos/muParserX/mpIfThenElse.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpIfThenElse.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpIfThenElse.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_typeinfo.o: wxChaos/angelscript/src/as_typeinfo.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_typeinfo.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_typeinfo.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptobject.o: wxChaos/angelscript/src/as_scriptobject.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_scriptobject.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptobject.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_string.o: wxChaos/angelscript/src/as_string.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_string.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_string.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_string_util.o: wxChaos/angelscript/src/as_string_util.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_string_util.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_string_util.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_thread.o: wxChaos/angelscript/src/as_thread.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_thread.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_thread.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_tokenizer.o: wxChaos/angelscript/src/as_tokenizer.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_tokenizer.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_tokenizer.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_variablescope.o: wxChaos/angelscript/src/as_variablescope.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_variablescope.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_variablescope.o

$(OBJDIR_NO_SETUP)/wxChaos/global.o: wxChaos/global.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/global.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/global.o

$(OBJDIR_NO_SETUP)/wxChaos/gradient.o: wxChaos/gradient.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/gradient.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/gradient.o

$(OBJDIR_NO_SETUP)/wxChaos/gradientdlg.o: wxChaos/gradientdlg.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/gradientdlg.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/gradientdlg.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpError.o: wxChaos/muParserX/mpError.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpError.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpError.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpParserBase.o: wxChaos/muParserX/mpParserBase.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpParserBase.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpParserBase.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpRPN.o: wxChaos/muParserX/mpRPN.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpRPN.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpRPN.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpScriptTokens.o: wxChaos/muParserX/mpScriptTokens.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpScriptTokens.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpScriptTokens.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpTest.o: wxChaos/muParserX/mpTest.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpTest.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpTest.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpTokenReader.o: wxChaos/muParserX/mpTokenReader.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpTokenReader.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpTokenReader.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValReader.o: wxChaos/muParserX/mpValReader.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpValReader.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValReader.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValue.o: wxChaos/muParserX/mpValue.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpValue.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValue.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValueCache.o: wxChaos/muParserX/mpValueCache.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpValueCache.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpValueCache.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpVariable.o: wxChaos/muParserX/mpVariable.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpVariable.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpVariable.o

$(OBJDIR_NO_SETUP)/wxChaos/wxMathPlot/mathplot.o: wxChaos/wxMathPlot/mathplot.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/wxMathPlot/mathplot.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/wxMathPlot/mathplot.o

$(OBJDIR_NO_SETUP)/wxChaos/wxSFMLCanvas.o: wxChaos/wxSFMLCanvas.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/wxSFMLCanvas.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/wxSFMLCanvas.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageCmplx.o: wxChaos/muParserX/mpPackageCmplx.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpPackageCmplx.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageCmplx.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtBinAssign.o: wxChaos/muParserX/mpOprtBinAssign.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpOprtBinAssign.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtBinAssign.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtBinCommon.o: wxChaos/muParserX/mpOprtBinCommon.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpOprtBinCommon.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtBinCommon.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtCmplx.o: wxChaos/muParserX/mpOprtCmplx.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpOprtCmplx.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtCmplx.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtIndex.o: wxChaos/muParserX/mpOprtIndex.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpOprtIndex.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtIndex.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtMatrix.o: wxChaos/muParserX/mpOprtMatrix.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpOprtMatrix.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtMatrix.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtNonCmplx.o: wxChaos/muParserX/mpOprtNonCmplx.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpOprtNonCmplx.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpOprtNonCmplx.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageCommon.o: wxChaos/muParserX/mpPackageCommon.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpPackageCommon.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageCommon.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageMatrix.o: wxChaos/muParserX/mpPackageMatrix.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpPackageMatrix.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageMatrix.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageNonCmplx.o: wxChaos/muParserX/mpPackageNonCmplx.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpPackageNonCmplx.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageNonCmplx.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageStr.o: wxChaos/muParserX/mpPackageStr.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpPackageStr.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageStr.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageUnit.o: wxChaos/muParserX/mpPackageUnit.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpPackageUnit.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpPackageUnit.o

$(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpParser.o: wxChaos/muParserX/mpParser.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/muParserX/mpParser.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/muParserX/mpParser.o

$(OBJDIR_NO_SETUP)/wxChaos/FractalTypes.o: wxChaos/FractalTypes.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/FractalTypes.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/FractalTypes.o

$(OBJDIR_NO_SETUP)/wxChaos/HTMLViewer.o: wxChaos/HTMLViewer.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/HTMLViewer.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/HTMLViewer.o

$(OBJDIR_NO_SETUP)/wxChaos/IterDialog.o: wxChaos/IterDialog.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/IterDialog.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/IterDialog.o

$(OBJDIR_NO_SETUP)/wxChaos/JuliaMode.o: wxChaos/JuliaMode.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/JuliaMode.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/JuliaMode.o

$(OBJDIR_NO_SETUP)/wxChaos/MainWindow.o: wxChaos/MainWindow.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/MainWindow.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/MainWindow.o

$(OBJDIR_NO_SETUP)/wxChaos/SizeDialogSave.o: wxChaos/SizeDialogSave.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/SizeDialogSave.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/SizeDialogSave.o

$(OBJDIR_NO_SETUP)/wxChaos/FractalGUI.o: wxChaos/FractalGUI.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/FractalGUI.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/FractalGUI.o

$(OBJDIR_NO_SETUP)/wxChaos/StringFuncs.o: wxChaos/StringFuncs.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/StringFuncs.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/StringFuncs.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptarray/scriptarray.o: wxChaos/angelscript/add_on/scriptarray/scriptarray.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/add_on/scriptarray/scriptarray.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptarray/scriptarray.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.o: wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.o: wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_atomic.o: wxChaos/angelscript/src/as_atomic.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_atomic.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_atomic.o

$(OBJDIR_NO_SETUP)/wxChaos/AngelScriptFunc.o: wxChaos/AngelScriptFunc.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/AngelScriptFunc.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/AngelScriptFunc.o

$(OBJDIR_NO_SETUP)/wxChaos/BinaryOps.o: wxChaos/BinaryOps.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/BinaryOps.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/BinaryOps.o

$(OBJDIR_NO_SETUP)/wxChaos/BmpWriter.o: wxChaos/BmpWriter.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/BmpWriter.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/BmpWriter.o

$(OBJDIR_NO_SETUP)/wxChaos/ColorFrame.o: wxChaos/ColorFrame.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/ColorFrame.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/ColorFrame.o

$(OBJDIR_NO_SETUP)/wxChaos/CommandFrame.o: wxChaos/CommandFrame.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/CommandFrame.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/CommandFrame.o

$(OBJDIR_NO_SETUP)/wxChaos/ConfigParser.o: wxChaos/ConfigParser.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/ConfigParser.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/ConfigParser.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_builder.o: wxChaos/angelscript/src/as_builder.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_builder.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_builder.o

$(OBJDIR_NO_SETUP)/wxChaos/ConstDialog.o: wxChaos/ConstDialog.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/ConstDialog.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/ConstDialog.o

$(OBJDIR_NO_SETUP)/wxChaos/DPExplorer.o: wxChaos/DPExplorer.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/DPExplorer.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/DPExplorer.o

$(OBJDIR_NO_SETUP)/wxChaos/DimensionFrame.o: wxChaos/DimensionFrame.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/DimensionFrame.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/DimensionFrame.o

$(OBJDIR_NO_SETUP)/wxChaos/FormulaDialog.o: wxChaos/FormulaDialog.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/FormulaDialog.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/FormulaDialog.o

$(OBJDIR_NO_SETUP)/wxChaos/FractalCanvas.o: wxChaos/FractalCanvas.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/FractalCanvas.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/FractalCanvas.o

$(OBJDIR_NO_SETUP)/wxChaos/FractalClasses.o: wxChaos/FractalClasses.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/FractalClasses.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/FractalClasses.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_gc.o: wxChaos/angelscript/src/as_gc.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_gc.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_gc.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_generic.o: wxChaos/angelscript/src/as_generic.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_generic.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_generic.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_globalproperty.o: wxChaos/angelscript/src/as_globalproperty.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_globalproperty.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_globalproperty.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_memory.o: wxChaos/angelscript/src/as_memory.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_memory.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_memory.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_module.o: wxChaos/angelscript/src/as_module.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_module.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_module.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_objecttype.o: wxChaos/angelscript/src/as_objecttype.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_objecttype.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_objecttype.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_outputbuffer.o: wxChaos/angelscript/src/as_outputbuffer.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_outputbuffer.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_outputbuffer.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_parser.o: wxChaos/angelscript/src/as_parser.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_parser.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_parser.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_restore.o: wxChaos/angelscript/src/as_restore.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_restore.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_restore.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptcode.o: wxChaos/angelscript/src/as_scriptcode.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_scriptcode.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptcode.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptengine.o: wxChaos/angelscript/src/as_scriptengine.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_scriptengine.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptengine.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptfunction.o: wxChaos/angelscript/src/as_scriptfunction.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_scriptfunction.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptfunction.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptnode.o: wxChaos/angelscript/src/as_scriptnode.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_scriptnode.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_scriptnode.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_msvc.o: wxChaos/angelscript/src/as_callfunc_x64_msvc.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_x64_msvc.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_msvc.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_bytecode.o: wxChaos/angelscript/src/as_bytecode.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_bytecode.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_bytecode.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc.o: wxChaos/angelscript/src/as_callfunc.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_arm.o: wxChaos/angelscript/src/as_callfunc_arm.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_arm.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_arm.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_mips.o: wxChaos/angelscript/src/as_callfunc_mips.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_mips.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_mips.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_ppc.o: wxChaos/angelscript/src/as_callfunc_ppc.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_ppc.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_ppc.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_ppc_64.o: wxChaos/angelscript/src/as_callfunc_ppc_64.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_ppc_64.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_ppc_64.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_sh4.o: wxChaos/angelscript/src/as_callfunc_sh4.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_sh4.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_sh4.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_gcc.o: wxChaos/angelscript/src/as_callfunc_x64_gcc.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_x64_gcc.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_gcc.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_mingw.o: wxChaos/angelscript/src/as_callfunc_x64_mingw.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_x64_mingw.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x64_mingw.o

$(OBJDIR_NO_SETUP)/wxChaos/About.o: wxChaos/About.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/About.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/About.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x86.o: wxChaos/angelscript/src/as_callfunc_x86.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_x86.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_x86.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_xenon.o: wxChaos/angelscript/src/as_callfunc_xenon.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_callfunc_xenon.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_callfunc_xenon.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_compiler.o: wxChaos/angelscript/src/as_compiler.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_compiler.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_compiler.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_configgroup.o: wxChaos/angelscript/src/as_configgroup.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_configgroup.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_configgroup.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_context.o: wxChaos/angelscript/src/as_context.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_context.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_context.o

$(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_datatype.o: wxChaos/angelscript/src/as_datatype.cpp
	$(CXX) $(CFLAGS_NO_SETUP) $(INC_NO_SETUP) -c wxChaos/angelscript/src/as_datatype.cpp -o $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src/as_datatype.o

clean_no_setup: 
	@echo "########################################################"
	@echo "#                                                      #"
	@echo "#  Cleaning up                                         #"
	@echo "#                                                      #"
	@echo "########################################################"
	@echo ""
	rm -rf bin/Doc
	rm -rf bin/Resources
	rm -rf bin/ScriptSamples
	rm -rf bin/UserScripts
	rm -f bin/config.ini
	rm -f $(OBJ_NO_SETUP) $(OUT_NO_SETUP)
	rm -rf $(OBJDIR_NO_SETUP)/wxChaos/muParserX
	rm -rf $(OBJDIR_NO_SETUP)/wxChaos/angelscript/src
	rm -rf $(OBJDIR_NO_SETUP)/wxChaos
	rm -rf $(OBJDIR_NO_SETUP)/wxChaos/wxMathPlot
	rm -rf $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptarray
	rm -rf $(OBJDIR_NO_SETUP)/wxChaos/angelscript/add_on/scriptstdstring
	rm -rf $(OBJDIR_NO_SETUP)

install:
	@echo "Installing to: /opt/extras.ubuntu.com/wxChaos"
	@test -d $(DESTDIR)/opt/extras.ubuntu.com/ || mkdir -p $(DESTDIR)/opt/extras.ubuntu.com/
	@mkdir -p $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@mkdir -p $(DESTDIR)/usr/share/applications
	@cp -rp bin/wxchaos $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@cp -rp install/Linux/wxChaos.png $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@cp -rp install/Linux/extras-wxChaos.desktop $(DESTDIR)/usr/share/applications
	@cp -rp app_resources/English/Doc $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@cp -rp app_resources/English/Resources $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@cp -rp app_resources/English/ScriptSamples $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@cp -rp app_resources/English/UserScripts $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@cp -rp app_resources/English/config.ini $(DESTDIR)/opt/extras.ubuntu.com/wxChaos
	@echo "Done"

uninstall:
	@echo "Uninstalling..."
	@rm -rf /opt/extras.ubuntu.com/wxChaos
	@rm -f /usr/share/applications/extras-wxChaos.desktop
	@echo "Done"


.PHONY: before_release after_release clean_release before_no_setup after_no_setup install uninstall

