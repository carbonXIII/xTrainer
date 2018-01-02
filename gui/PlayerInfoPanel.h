/*
 * PlayerInfoPanel.h
 *
 *  Created on: Dec 24, 2017
 *      Author: shado
 */

#ifndef GUI_PLAYERINFOPANEL_H_
#define GUI_PLAYERINFOPANEL_H_

#include <wx/wx.h>

class PlayerInfoPanel: public wxPanel{
public:
	PlayerInfoPanel(Player* player, int idx, wxWindow* parent, const wxPoint& pos, const wxSize& size): wxPanel(parent, wxID_ANY, pos, size), m_player(player){
		wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

		wxStaticText* label = new wxStaticText(this, wxID_ANY, wxString("Player ") << (idx + 1) << ":");
		mainSizer->Add(label, 1, wxALL | wxEXPAND, 5);

		wxBoxSizer* posBoxSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText* xLabel = new wxStaticText(this, wxID_ANY, wxString("x:"));
			posBoxSizer->Add(xLabel, 1, wxALL, 5);

			wxTextCtrl* xCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0,
					wxFloatingPointValidator<float>(&textCtrlData.x));
			posBoxSizer->Add(xCtrl, 3, wxALL, 5);

			wxStaticText* yLabel = new wxStaticText(this, wxID_ANY, wxString("y:"));
			posBoxSizer->Add(yLabel, 1, wxALL, 5);

			wxTextCtrl* yCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0,
					wxFloatingPointValidator<float>(&textCtrlData.y));
			posBoxSizer->Add(yCtrl, 3, wxALL, 5);
		}
		mainSizer->Add(posBoxSizer, 1, wxALL | wxEXPAND, 5);

		wxBoxSizer* velBoxSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText* xLabel = new wxStaticText(this, wxID_ANY, wxString("vx:"));
			velBoxSizer->Add(xLabel, 1, wxALL, 5);

			wxTextCtrl* xCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0,
					wxFloatingPointValidator<float>(&textCtrlData.vx));
			velBoxSizer->Add(xCtrl, 3, wxALL, 5);

			wxStaticText* yLabel = new wxStaticText(this, wxID_ANY, wxString("vy:"));
			velBoxSizer->Add(yLabel, 1, wxALL, 5);

			wxTextCtrl* yCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0,
					wxFloatingPointValidator<float>(&textCtrlData.vy));
			velBoxSizer->Add(yCtrl, 3, wxALL, 5);
		}
		mainSizer->Add(velBoxSizer, 1, wxALL | wxEXPAND, 5);

		wxBoxSizer* extraBoxSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText* dmgLabel = new wxStaticText(this, wxID_ANY, wxString("damage:"));
			extraBoxSizer->Add(dmgLabel, 2, wxALL, 5);

			wxTextCtrl* dmgCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0,
					wxFloatingPointValidator<float>(&textCtrlData.dmg));
			extraBoxSizer->Add(dmgCtrl, 3, wxALL, 5);

			wxStaticText* stockLabel = new wxStaticText(this, wxID_ANY, wxString("stock:"));
			extraBoxSizer->Add(stockLabel, 2, wxALL, 5);

			wxTextCtrl* stockCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0,
					wxFloatingPointValidator<uint8_t>(&textCtrlData.stockCount));
			extraBoxSizer->Add(stockCtrl, 3, wxALL, 5);
		}
		mainSizer->Add(extraBoxSizer, 1, wxALL | wxEXPAND, 5);

		SetSizer(mainSizer);
	}

	void refresh(){
		textCtrlData.x = m_player->getX();
		textCtrlData.y = m_player->getY();
		textCtrlData.vx = m_player->getVx();
		textCtrlData.vy = m_player->getVy();
		textCtrlData.dmg = m_player->getDmg();
		textCtrlData.stockCount = m_player->getStock();

		wxStaticText* t;
		t->upda

		Enable(m_player->isInGame());
	}

	struct{
		float x, y, vx, vy, dmg;
		uint8_t stockCount;
	} textCtrlData;

	Player* m_player;
};



#endif /* GUI_PLAYERINFOPANEL_H_ */
