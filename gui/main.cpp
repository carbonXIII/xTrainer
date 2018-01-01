/*
 * main.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: shado
 */
#define __WXMSW__
#include <wx/setup.h>
#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/filepicker.h>
#include <string>

#include "../tools/trainer.h"
#include "../structs/Dolphin.h"

#include <iostream>
#include <sstream>
#include <utility>

#include "PlayerInfoPanel.h"

enum{
	TimerCallback = wxID_HIGHEST,
	ProcessDialogRadio,
	ProcessDialogContinue,
	ProcessDialogRefresh
};

class App;
class ProcessDialog: public wxDialog{
public:
	ProcessDialog(App* parent, wxString title, const wxPoint& pos, const wxSize& size);

	void OnRefresh(wxCommandEvent& e);

	void OnClose(wxCloseEvent& e){
		EndModal(wxID_CANCEL);
	}

	void OnContinue(wxCommandEvent& e){
		if(validate()){
			EndModal(wxID_OK);
		}else{
			wxMessageDialog* warningBox = new wxMessageDialog(this, "Invalid selection.", "Input Error", wxOK | wxICON_ERROR);
			warningBox->ShowModal();
		}
	}

	void radioButtonCallback(wxCommandEvent& e){
		if(radioButtonA->GetValue()){
			EnableA(true);
			EnableB(false);
		}else{
			EnableA(false);
			EnableB(true);
		}
	}

	wxRadioButton *radioButtonA, *radioButtonB;

	//Section A
	wxStaticText* choiceText;
	wxChoice* choice;
	void EnableA(bool b){
		choiceText->Enable(b);
		choice->Enable(b);
	}

	//Section B
	wxStaticText *pidText, *fileText;
	wxTextCtrl *pidTextControl;
	wxFilePickerCtrl *fileTextControl;
	void EnableB(bool b){
		pidText->Enable(b);
		fileText->Enable(b);
		pidTextControl->Enable(b);
		fileTextControl->Enable(b);
	}

	bool validate(){
		if(radioButtonA->GetValue()){
			if(choice->GetSelection() == wxNOT_FOUND)return false;
		}else if(!fileTextControl->Validate())return false;

		return true;
	}

	std::pair<unsigned long,std::string> value(){
		if(radioButtonA->GetValue()){
			return pList[choice->GetSelection()];
		}

		return std::make_pair(pid, (std::string)fileTextControl->GetTextCtrlValue());
	}

	unsigned long pid = 0;
	std::vector<std::pair<unsigned long, std::string>> pList;
	App* parent;

	wxDECLARE_EVENT_TABLE();
};


BEGIN_EVENT_TABLE(ProcessDialog, wxDialog)
EVT_RADIOBUTTON(ProcessDialogRadio, ProcessDialog::radioButtonCallback)
EVT_CLOSE(ProcessDialog::OnClose)
EVT_BUTTON(ProcessDialogContinue, ProcessDialog::OnContinue)
EVT_BUTTON(ProcessDialogRefresh, ProcessDialog::OnRefresh)
END_EVENT_TABLE()

class MainFrame: public wxFrame{
public:
	PlayerInfoPanel* players[4];
	wxTimer* timer;

	MainFrame(App* parent, const wxString& title, const wxPoint& pos, const wxSize& size);

	~MainFrame(){
		timer->Stop();
	}

	void update(wxTimerEvent& event);

	wxDECLARE_EVENT_TABLE();

private:
	App* parent;
};


BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_TIMER(TimerCallback, MainFrame::update)
END_EVENT_TABLE()


class App: public wxApp{
public:
	virtual bool OnInit(){
		default_logger.useStdErr(true);

		//open dialog
		ProcessDialog dialog(this, "Select Target Process", wxDefaultPosition, wxSize(500,300));
		if(dialog.ShowModal() != wxID_OK)return false;

		auto val = dialog.value();

		m_process = new Process(val.first, val.second);
		m_dolphin = new Dolphin(m_process);

		frame = new MainFrame(this, "Dolphin Trainer", wxDefaultPosition, wxSize(1000,500));
		frame->Show();
		return true;
	}

	void getProcessList(std::vector<std::pair<unsigned long, std::string>>& out){
		out = enumerateProcesses("dolphin");
	}

	Dolphin& getDolphin(){
		return *m_dolphin;
	}

private:
	Process* m_process;
	Dolphin* m_dolphin;
	MainFrame* frame;
};

void MainFrame::update(wxTimerEvent& event){
	parent->getDolphin().ram.update();

	for(int i = 0; i < 4; i++){
		players[i]->refresh();
		LOG << players[i]->textCtrlData.x << '\n';
	}
}

void ProcessDialog::OnRefresh(wxCommandEvent& e){
	parent->getProcessList(pList);

	wxArrayString options;
	for(auto x : pList){
		wxString item = x.second;
		if(item.size() > 60){
			item = wxString("...") << item.substr(item.size() - 57, 57);
		}
		options.push_back(item);
	}

	choice->Set(options);
}

ProcessDialog::ProcessDialog(App* parent, wxString title, const wxPoint& pos, const wxSize& size): wxDialog(nullptr, wxID_ANY, title, pos, size), parent(parent){
	//Get process list
	parent->getProcessList(pList);

	wxArrayString options;
	for(auto x : pList){
		wxString item = x.second;
		if(item.size() > 60){
			item = wxString("...") << item.substr(item.size() - 57, 57);
		}
		options.push_back(item);
	}

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	radioButtonA = new wxRadioButton(this, ProcessDialogRadio, "Select process from list of discovered processes");
	mainSizer->Add(radioButtonA, 0, wxALL, 5);

	wxBoxSizer* listViewSizer = new wxBoxSizer(wxHORIZONTAL);
	{
		choiceText = new wxStaticText(this, -1, "Select target process: ");
		listViewSizer->Add(choiceText, 1, wxALL, 5);

		choice = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize, options);
		listViewSizer->Add(choice, 3, wxALL, 5);
	}
	mainSizer->Add(listViewSizer, 1, wxALL, 5);

	wxButton* refresh = new wxButton(this, ProcessDialogRefresh, "Refresh");
	mainSizer->Add(refresh, 0, wxALL | wxALIGN_RIGHT, 20);

	radioButtonB = new wxRadioButton(this, ProcessDialogRadio, "Manually enter process information");
	mainSizer->Add(radioButtonB, 0, wxALL, 5);

	wxFlexGridSizer* manualViewSizer = new wxFlexGridSizer(2,2,wxSize(5,5));
	{
		pidText = new wxStaticText(this, -1, "PID:");
		manualViewSizer->Add(pidText, 1, wxALL, 5);

		pidTextControl = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<unsigned long>(&pid));
		manualViewSizer->Add(pidTextControl, 1, wxALL | wxEXPAND, 5);

		fileText = new wxStaticText(this, -1, "Executable File:");
		manualViewSizer->Add(fileText, 1, wxALL, 5);

		fileTextControl = new wxFilePickerCtrl(this, -1, "", "Choose Dolphin executable file for the entered PID...");
		manualViewSizer->Add(fileTextControl, 1, wxALL | wxEXPAND, 5);
	}
	manualViewSizer->AddGrowableCol(1, 1);
	mainSizer->Add(manualViewSizer, 1, wxALL | wxEXPAND, 5);

	wxButton* b = new wxButton(this, ProcessDialogContinue, _("Continue"), wxPoint(6,2), wxDefaultSize);
	mainSizer->Add(b,  1, wxALL | wxEXPAND, 5);

	SetSizer(mainSizer);
}

MainFrame::MainFrame(App* parent, const wxString& title, const wxPoint& pos, const wxSize& size): wxFrame(nullptr, wxID_ANY, title, pos, size), parent(parent){
	SetBackgroundColour(wxColour("F0F0F0"));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	for(int i = 0; i < 4; i++){
		players[i] = new PlayerInfoPanel(&parent->getDolphin().ram.getPlayer(i), i, this, wxDefaultPosition, wxDefaultSize);
		mainSizer->Add(players[i], 1, wxALL, 20);
	}

	SetSizer(mainSizer);

	timer = new wxTimer(this, TimerCallback);
	timer->Start(1000/60);
}

IMPLEMENT_APP(App)
