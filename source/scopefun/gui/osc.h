///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/spinbutt.h>
#include <wx/slider.h>
#include <wx/notebook.h>
#include <wx/radiobut.h>
#include <wx/statline.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/dataview.h>
#include <wx/scrolwin.h>
#include <wx/statbmp.h>

#include "scopefun/gui/guiheader.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class Osciloskop
///////////////////////////////////////////////////////////////////////////////
class Osciloskop : public wxFrame
{
	private:

	protected:
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenu* m_menu2;
		wxMenu* m_menu3;
		wxMenu* m_menu5;
		wxMenu* m_menu8;
		wxMenu* m_menu7;
		wxPanel* m_panel26;
		wxCheckBox* m_checkBox26;
		wxCheckBox* m_checkBox261;
		wxButton* m_buttonConnect;
		wxButton* m_buttonDisconnect;
		wxButton* m_buttonReset;
		wxButton* m_buttonUndo;
		wxButton* m_buttonRedo;
		wxStaticText* m_staticText19;
		wxChoice* m_comboBoxTimeControl;
		wxNotebook* m_notebook7;
		wxPanel* m_panel251;
		wxStaticText* m_staticText141;
		wxChoice* m_comboBoxTimeCapture;
		wxCheckBox* m_checkBoxFull;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textCtrlTimePosition;
		wxSpinButton* m_spinBtnXPos;
		wxSlider* m_sliderTimePosition;
		wxStaticText* m_staticText122;
		wxTextCtrl* m_textCtrlTimeFrame;
		wxSpinButton* m_spinBtnFrameHistory;
		wxSlider* m_sliderTimeFrame;
		wxButton* m_buttonPlay;
		wxButton* m_buttonPause;
		wxButton* m_buttonCapture;
		wxButton* m_buttonSimulate;
		wxButton* m_buttonClear;
		wxStaticText* m_staticText181;
		wxTextCtrl* m_textCtrlTimeFrameSize;
		wxStaticText* m_staticText1811;
		wxTextCtrl* m_textCtrlTimeFFTSize;
		wxNotebook* m_notebook4;
		wxPanel* m_panel15;
		wxStaticText* m_staticText34;
		wxChoice* m_comboBoxCh0Capture;
		wxStaticText* m_staticTextCh0Scale;
		wxTextCtrl* m_textCtrlCh0Scale;
		wxChoice* m_choiceCh0ACDC;
		wxCheckBox* m_checkBoxCh0Invert;
		wxCheckBox* m_checkBoxCh0Ground;
		wxCheckBox* m_checkBoxSignal1;
		wxCheckBox* m_checkBoxFFT1;
		wxCheckBox* m_checkBoxAvg01;
		wxStaticText* m_staticText1221;
		wxTextCtrl* m_textCtrlCh0Position;
		wxSpinButton* m_spinBtnCh0YPos;
		wxSlider* m_sliderCh0Position;
		wxPanel* m_panel16;
		wxStaticText* m_staticText341;
		wxChoice* m_comboBoxCh1Capture;
		wxStaticText* m_staticTextCh1Scale;
		wxTextCtrl* m_textCtrlCh1Scale;
		wxChoice* m_choiceCh1ACDC;
		wxCheckBox* m_checkBoxCh1Invert;
		wxCheckBox* m_checkBoxCh1Ground;
		wxCheckBox* m_checkBoxSignal2;
		wxCheckBox* m_checkBoxFFT2;
		wxCheckBox* m_checkBoxAvg02;
		wxStaticText* m_staticText12211;
		wxTextCtrl* m_textCtrlCh1Position;
		wxSpinButton* m_spinBtnCh1YPos;
		wxSlider* m_sliderCh1Position;
		wxPanel* m_panel161;
		wxStaticText* m_staticTextFunction;
		wxChoice* m_comboBoxFunction;
		wxCheckBox* m_checkBoxXYVoltageGraph;
		wxCheckBox* m_checkBoxSignalF;
		wxCheckBox* m_checkBoxFFTF;
		wxNotebook* m_notebook3;
		wxPanel* m_panel3;
		wxRadioButton* m_radioBtnTriggered;
		wxButton* m_buttonReArm;
		wxStaticText* m_staticText47;
		wxChoice* m_comboBoxTrigger;
		wxStaticText* m_staticText45;
		wxChoice* m_comboBoxTriggerSource;
		wxStaticText* m_staticText46;
		wxChoice* m_comboBoxTriggerSlope;
		wxStaticLine* m_staticline11;
		wxStaticText* m_staticText1222;
		wxStaticText* m_staticText12221;
		wxStaticText* m_staticText12222;
		wxStaticText* m_staticText55;
		wxTextCtrl* m_textCtrlTriggerLevel;
		wxSpinButton* m_spinBtnTrigLevel;
		wxSlider* m_sliderTriggerLevel;
		wxTextCtrl* m_textCtrlTriggerHisteresis;
		wxSpinButton* m_spinBtnTrigHis;
		wxSlider* m_sliderTriggerHisteresis;
		wxTextCtrl* m_textCtrlTriggerPre;
		wxSpinButton* m_spinBtnTrigPre;
		wxSlider* m_sliderTriggerPre;
		wxTextCtrl* m_textCtrlTriggerHoldoff;
		wxSpinButton* m_spinBtnTrigHoldoff;
		wxSlider* m_sliderTriggerHoldoff;

		// Virtual event handlers, override them in your derived class
		virtual void onActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void onActivateApp( wxActivateEvent& event ) { event.Skip(); }
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnIdle( wxIdleEvent& event ) { event.Skip(); }
		virtual void OnSetFocus( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void m_menuItem1OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem2OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem3OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem20OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem21OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem22OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem23OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem6OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem8OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem9OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemReadEEPROMOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemWriteEEPROMOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem12OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemWriteCalibrateOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemReadCalibrateOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemWriteCertificateOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemReadCertificateOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemLockCertificateOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemResetEEPROMOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemFactoryWriteEEPROMOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemFactoryLoadFX3RamOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemEraseECalibrationOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItem15OnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemDebugOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_menuItemInfoOnMenuSelection( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonConnectOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDisconnectOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonResetOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonUndoOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonRedoOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_comboBoxTimeControlOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_comboBoxTimeCaptureOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFullOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTimePositionOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnXPosOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnXPosOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderTimePositionOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTimeFrameOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnFrameHistoryOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnFrameHistoryOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderTimeFrameOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonPlayOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonPauseOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCaptureOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonSimulateOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonClearOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTimeFrameSizeOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTimeFFTSizeOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_comboBoxCh0CaptureOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlCh0ScaleOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choiceCh0ACDCOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxCh0InvertOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxCh0GroundOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxSignal1OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFFT1OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxAvg01OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlCh0PositionOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnCh0YPosOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnCh0YPosOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderCh0PositionOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_comboBoxCh1CaptureOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlCh1ScaleOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choiceCh1ACDCOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxCh1InvertOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxCh1GroundOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxSignal2OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFFT2OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxAvg02OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlCh1PositionOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnCh1YPosOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnCh1YPosOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderCh1PositionOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_comboBoxFunctionTypeOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxXYVoltageGraphOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxSignalFOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFFTFOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonReArmOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_comboBoxTriggerOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_comboBoxTriggerSourceOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_comboBoxTriggerSlopeOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTriggerLevelOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigLevelOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigLevelOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderTriggerLevelOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTriggerHisteresisOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigHisOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigHisOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderTriggerHisteresisOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTriggerPreOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigPreOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigPreOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderTriggerPreOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrlTriggerHoldoffOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigHoldoffOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnTrigHoldoffOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderTriggerHoldoffOnScroll( wxScrollEvent& event ) { event.Skip(); }


	public:
		wxMenuItem* m_menuItemWriteCertificate;
		wxMenuItem* m_separator4111;
		wxMenuItem* m_menuItemLockCertificate;
		wxMenuItem* m_separator411;
		wxMenuItem* m_menuItemResetEEPROM;
		wxMenuItem* m_menuItemFactoryWriteEEPROM;
		wxMenuItem* m_menuItemFactoryLoadFX3Ram;
		wxMenuItem* m_menuItemEraseECalibration;
		wxMenuItem* m_menuItemDebug;

		Osciloskop( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Oscilloscope"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1026,843 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Osciloskop();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Thermal
///////////////////////////////////////////////////////////////////////////////
class Thermal : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText52;
		wxTextCtrl* m_textCtrlHeating;
		wxSpinButton* m_spinBtnHeating;
		wxSlider* m_sliderHeat;
		wxStaticText* m_staticText53;
		wxTextCtrl* m_textCtrlCooling;
		wxSpinButton* m_spinBtnCooling;
		wxSlider* m_sliderCool;
		wxCheckBox* m_checkBoxEnabled;
		wxStaticLine* m_staticline5;
		wxButton* m_buttonOk;
		wxButton* m_buttonDefault;
		wxButton* m_buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void ThermalOnActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void m_textCtrlHeatingOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnHeatingOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnHeatingOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderHeatOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrlCoolingOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnCoolingOnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnCoolingOnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderCoolOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_checkBoxEnabledOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonOkOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDefaultOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCancelOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Thermal( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Thermal"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );

		~Thermal();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Display
///////////////////////////////////////////////////////////////////////////////
class Display : public wxDialog
{
	private:

	protected:
		wxCheckBox* m_checkBoxUnits;
		wxCheckBox* m_checkBoxAxis;
		wxCheckBox* m_checkBoxGrid;
		wxCheckBox* m_checkBoxFFTUnit;
		wxCheckBox* m_checkBoxFFTAxis;
		wxCheckBox* m_checkBoxFFTGrid;
		wxCheckBox* m_checkBoxDigitalUnit;
		wxCheckBox* m_checkBoxDigitalAxis;
		wxCheckBox* m_checkBoxDigitalGrid;
		wxStaticText* m_staticText78;
		wxChoice* m_choiceSignal;
		wxStaticText* m_staticText38;
		wxTextCtrl* m_textCtrlSignal;
		wxSlider* m_sliderSignal;
		wxStaticText* m_staticText79;
		wxChoice* m_choiceFFT;
		wxStaticText* m_staticText39;
		wxTextCtrl* m_textCtrlFFT;
		wxSlider* m_sliderFFT;
		wxStaticText* m_staticText391;
		wxChoice* m_choiceFFTY;
		wxStaticText* m_staticText3911;
		wxChoice* m_choiceFFTX;
		wxStaticLine* m_staticline9;
		wxCheckBox* m_checkBox3dSolid;
		wxCheckBox* m_checkBox3dLight;
		wxCheckBox* m_checkBox3dDepthTest;
		wxStaticText* m_staticText962;
		wxTextCtrl* m_textCtrl3dAlphaCh0;
		wxStaticText* m_staticText992;
		wxSlider* m_slider3dAlphaCh0;
		wxStaticText* m_staticText1002;
		wxStaticText* m_staticText9621;
		wxTextCtrl* m_textCtrl3dAlphaCh1;
		wxStaticText* m_staticText9921;
		wxSlider* m_slider3dAlphaCh1;
		wxStaticText* m_staticText10021;
		wxStaticText* m_staticText96211;
		wxTextCtrl* m_textCtrl3dAlphaFun;
		wxStaticText* m_staticText99211;
		wxSlider* m_slider3dAlphaFun;
		wxStaticText* m_staticText100211;
		wxStaticLine* m_staticline10;
		wxStaticText* m_staticText96;
		wxTextCtrl* m_textCtrl3dTessalation;
		wxStaticText* m_staticText99;
		wxSlider* m_slider3dTessalation;
		wxStaticText* m_staticText100;
		wxStaticText* m_staticText961;
		wxTextCtrl* m_textCtrl2dTessalation;
		wxStaticText* m_staticText991;
		wxSlider* m_slider2dTessalation;
		wxStaticText* m_staticText1001;
		wxStaticLine* m_staticline4;
		wxButton* m_buttonOk;
		wxButton* m_buttonDefault;
		wxButton* m_buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void DisplayOnInitDialog( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void m_checkBoxUnitsOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxAxisOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxGridOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFFTUnitOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFFTAxisOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxFFTGridOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxDigitalUnitOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxDigitalAxisOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxDigitalGridOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choiceSignalOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlSignalOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sliderSignalOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_choiceFFTOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlFFTOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_sliderFFTOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_choiceFFTYOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choiceFFTXOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBox3dSolidOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBox3dLightOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBox3dDepthTestOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrl3dAlphaCh0OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_slider3dAlphaCh0OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrl3dAlphaCh1OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_slider3dAlphaCh1OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrl3dAlphaFunOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_slider3dAlphaFunOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrl3dTessalationOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_slider3dTessalationOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_textCtrl2dTessalationOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_slider2dTessalationOnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonOkOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDefaultOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCancelOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Display( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Display"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );

		~Display();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Measure
///////////////////////////////////////////////////////////////////////////////
class Measure : public wxDialog
{
	private:

	protected:
		wxCheckBox* m_checkBoxAutoClear;
		wxCheckBox* m_checkBoxAutoClearTrigger;
		wxButton* m_buttonHistoryClear;
		wxButton* m_buttonClearX;
		wxButton* m_buttonClearY;
		wxButton* m_buttonClearFFT;
		wxButton* m_buttonCopy;
		wxButton* m_buttonPickX0;
		wxSpinButton* m_spinBtnX0;
		wxButton* m_buttonPickX1;
		wxSpinButton* m_spinBtnX1;
		wxButton* m_buttonPickY0;
		wxSpinButton* m_spinBtnY0;
		wxButton* m_buttonPickY1;
		wxSpinButton* m_spinBtnY1;
		wxButton* m_buttonPickFFT0;
		wxSpinButton* m_spinBtnFFT0;
		wxButton* m_buttonPickFFT1;
		wxSpinButton* m_spinBtnFFT1;
		wxScrolledWindow* m_scrolledWindow1;
		wxDataViewListCtrl* m_dataViewListCtrl1;
		wxButton* m_buttonOk;
		wxButton* m_buttonDefault;
		wxButton* m_buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void DisplayOnInitDialog( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void m_checkBoxAutoClearOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxAutoClearTriggerOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonHistoryClearOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonClearXOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonClearYOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonClearFFTOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCopyOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonPickX0OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlX0OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnX0OnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnX0OnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderX0OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonPickX1OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlX1OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnX1OnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnX1OnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderX1OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonPickY0OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlY0OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choiceY0OnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnY0OnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnY0OnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderY0OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonPickY1OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlY1OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choiceY1OnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnY1OnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnY1OnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderY1OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonPickFFT0OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlFFT0OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnFFT0OnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnFFT0OnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderFFT0OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_buttonPickFFT1OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlFFT1OnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_spinBtnFFT1OnSpinDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_spinBtnFFT1OnSpinUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void m_sliderFFT1OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void m_ItemActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void m_ItemStartEditing( wxDataViewEvent& event ) { event.Skip(); }
		virtual void m_SelectionChanged1( wxDataViewEvent& event ) { event.Skip(); }
		virtual void m_OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void m_dataViewListCtrl1OnMouseEvents( wxMouseEvent& event ) { event.Skip(); }
		virtual void m_buttonOkOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDefaultOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCancelOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxTextCtrl* m_textCtrlX0;
		wxChoice* m_choiceX0;
		wxSlider* m_sliderX0;
		wxTextCtrl* m_textCtrlX1;
		wxChoice* m_choiceX1;
		wxSlider* m_sliderX1;
		wxTextCtrl* m_textCtrlY0;
		wxChoice* m_choiceY0;
		wxSlider* m_sliderY0;
		wxTextCtrl* m_textCtrlY1;
		wxChoice* m_choiceY1;
		wxSlider* m_sliderY1;
		wxTextCtrl* m_textCtrlFFT0;
		wxChoice* m_choiceFFT0;
		wxSlider* m_sliderFFT0;
		wxTextCtrl* m_textCtrlFFT1;
		wxChoice* m_choiceFFT1;
		wxSlider* m_sliderFFT1;

		Measure( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Measure"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );

		~Measure();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Info
///////////////////////////////////////////////////////////////////////////////
class Info : public wxDialog
{
	private:

	protected:
		wxNotebook* m_notebook5;
		wxPanel* m_panel11;
		wxStaticBitmap* m_gplLogo;
		wxTextCtrl* m_textCtrlLicense;
		wxPanel* m_panel12;
		wxTextCtrl* m_textCtrlInfo;
		wxPanel* m_panel13;
		wxStaticText* m_staticText99;
		wxNotebook* m_notebook7;
		wxPanel* m_panel22;
		wxTextCtrl* m_textCtrlGlew;
		wxPanel* m_panel23;
		wxTextCtrl* m_textCtrlKissFFT;
		wxPanel* m_panel231;
		wxTextCtrl* m_textCtrlwxWidgets;
		wxPanel* m_panel2311;
		wxTextCtrl* m_textCtrlSDL2;
		wxPanel* m_panel23111;
		wxTextCtrl* m_textCtrlCJSON;
		wxPanel* m_panel231111;
		wxTextCtrl* m_textCtrlLibUSB;
		wxPanel* m_panel232;
		wxTextCtrl* m_textCtrlPThreads;
		wxPanel* m_panel28;
		wxTextCtrl* m_textCtrlLibPython;
		wxPanel* m_panel17;
		wxTextCtrl* m_textCtrlVersion;
		wxTextCtrl* m_textCtrlDateTime;
		wxStaticBitmap* m_bitmapLogo;
		wxButton* m_buttonOk;

		// Virtual event handlers, override them in your derived class
		virtual void ThermalOnActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void InfoOnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void m_textCtrlGlewOnTextURL( wxTextUrlEvent& event ) { event.Skip(); }
		virtual void m_buttonOkOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Info( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 700,500 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );

		~Info();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Debug
///////////////////////////////////////////////////////////////////////////////
class Debug : public wxDialog
{
	private:

	protected:
		wxButton* m_buttonSave;

		// Virtual event handlers, override them in your derived class
		virtual void ThermalOnActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void m_buttonSaveOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxTextCtrl* m_textCtrlOutput;

		Debug( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Debug"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 488,396 ), long style = wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER );

		~Debug();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Storage
///////////////////////////////////////////////////////////////////////////////
class Storage : public wxDialog
{
	private:

	protected:
		wxStaticLine* m_staticline21;
		wxStaticText* m_staticText112;
		wxChoice* m_choiceStorage;
		wxStaticText* m_staticText113;
		wxTextCtrl* m_textCtrlStorage;
		wxStaticText* m_staticText114;
		wxStaticText* m_staticText119;
		wxChoice* m_choicePacketSize;
		wxStaticText* m_staticText120;
		wxStaticLine* m_staticline20;
		wxButton* m_buttonOk;
		wxButton* m_buttonDefault;
		wxButton* m_buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void m_choiceStorageOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_textCtrlStorageOnTextEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_choicePacketSizeOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonOkOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDefaultOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCancelOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Storage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Storage"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~Storage();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Connection
///////////////////////////////////////////////////////////////////////////////
class Connection : public wxDialog
{
	private:

	protected:
		wxStaticLine* m_staticline20;
		wxStaticText* m_staticText111;
		wxChoice* m_choiceConnection;
		wxStaticLine* m_staticline21;
		wxStaticText* m_staticText122;
		wxTextCtrl* m_textCtrlIP;
		wxStaticText* m_staticText123;
		wxTextCtrl* m_textCtrlPort;
		wxCheckBox* m_checkBoxConnected;
		wxButton* m_buttonConnect;
		wxButton* m_buttonDisconnect;
		wxStaticLine* m_staticline22;
		wxButton* m_buttonOk;
		wxButton* m_buttonDefault;
		wxButton* m_buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void m_choiceConnectionOnChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_checkBoxConnectedOnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonConnectOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDisconnectOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonOkOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonDefaultOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void m_buttonCancelOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Connection( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~Connection();

};

