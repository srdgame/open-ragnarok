/* $Id$ */
#include "stdafx.h"
#include "openro.h"
#include "sdle/ft_font.h"
#include "rogl/actgl.h"

rogl::ActGL* act_test = NULL;

OpenRO::OpenRO() : ROEngine() {
	ReadIni("data.ini");
	m_state = ST_Login;
	//m_showui = true;

	m_serverlist = NULL;
	m_map = NULL;
}

OpenRO::~OpenRO() {
	if (m_serverlist != NULL)
		delete(m_serverlist);
}

void OpenRO::LoginScreen() {
	dskLogin->setEnabled(true);
	dskLogin->clear();
	m_state = ST_Login;
	m_gui.setDesktop(dskLogin);
	m_network.getLogin().Close();
}

void OpenRO::CharSelectScreen() {
	dskChar->setEnabled(true);
	dskCreate->clear();
	m_gui.setDesktop(dskChar);
}

void OpenRO::ProcessLogin(const std::string& user, const std::string& pass) {
	dskLogin->setEnabled(false);
	m_network.getLogin().Connect(OpenRO::ConnectionIP, OpenRO::ConnectionPort);
	m_network.GameLogin(user, pass);
}

void OpenRO::ServiceSelect(unsigned int serviceid) {
	//If nothing selected
	if (serviceid < (m_serverlist->getServerCount()-1))
		return;

	dskService->setEnabled(false); //Disable the service select window
	m_network.getLogin().Close();	//Close the socket to the login server

	struct in_addr addr;
	addr.s_addr = (long)m_serverlist->getInfo(serviceid).ip;	//Convert the IP to string (stored in long)

	fprintf(stdout,"%s:%d\n",inet_ntoa(addr),m_serverlist->getInfo(serviceid).port);	//Debug info

	m_network.getChar().Connect(inet_ntoa(addr), m_serverlist->getInfo(serviceid).port);	//Connect to the charserver
	m_network.CharLogin(m_serverlist->getAccountId(), m_serverlist->getSessionId1(), m_serverlist->getSessionId2(), m_serverlist->getSex());	//Login to the charserver
}

void OpenRO::CharSelect(unsigned int slot){
	//If the socket to charserver is closed
	if(!m_network.getChar().isConnected())
		return;

	dskChar->setEnabled(false);

	printf("Char slot %d selected.\n",slot);
	m_network.CharSelect(slot);
}

void OpenRO::HandleKeyboard() {
	if (keys[SDLK_ESCAPE])
		m_quit = true;
	if (keys[SDLK_F10]) {
		m_gui.Dialog("Testing", "You've pressed F10", *this);
		keys[SDLK_F10] = false;
	}
}

void OpenRO::Quit() {
	m_quit = true;
	m_state = ST_Unk;
}

void OpenRO::AfterDraw() {
	m_network.Process();

	ronet::Packet* pkt = m_network.popPacket();

#define HANDLEPKT(pktid, delp) case ronet::pkt ##pktid ##ID : delpkt = delp; hndl ##pktid ((ronet::pkt ##pktid *)pkt); break

	bool delpkt; // if it's true, we dispose of the packet
	while (pkt != NULL) {
		delpkt = true;
		switch(pkt->getID()) {
			/*
			case ronet::pktServerListID:
				delpkt = false;
				hndlServerList((ronet::pktServerList*)pkt);
				break;
			*/
			//Add new packets here
			HANDLEPKT(UpdateStatus, false);
			HANDLEPKT(ServerList, false);
			HANDLEPKT(CharList, true);
			HANDLEPKT(LoginError, true);
			HANDLEPKT(AuthFailed, true);
			HANDLEPKT(CharCreated, true);
			HANDLEPKT(CharPosition, false);
			HANDLEPKT(MapAcctSend, false);
			HANDLEPKT(MapLoginSuccess, false);
			HANDLEPKT(OwnSpeech, false);
			HANDLEPKT(SkillList, false);
			default:
				std::cerr << "Unhandled packet id " << pkt->getID() << "(len: " << pkt->size() << ")" << std::endl;
		}

		if (delpkt)
			delete(pkt);
		pkt = m_network.popPacket();
	}
}

void OpenRO::LoadMap(const char* map) {
	RO::RSW* rsw;

	// TODO: Delete active map (if any)

	std::string rsw_fn(map);
	rsw_fn += ".rsw";
	// Load the rsw object
	if (!getROObjects().ReadRSW(rsw_fn.c_str(), getFileManager())) {
		fprintf(stderr, "Error loading RSW file %s\n", rsw_fn.c_str());
		return;
	}
	rsw = (RO::RSW*)getROObjects().get(rsw_fn);
	if (!getROObjects().ReadGND(rsw->gnd_file, getFileManager())) {
		fprintf(stderr, "Error loading GND file %s\n", rsw->gnd_file);
		return;
	}
	if (!getROObjects().ReadGAT(rsw->gat_file, getFileManager())) {
		fprintf(stderr, "Error loading GAT file %s\n", rsw->gat_file);
		return;
	}

	RswObject* obj = new RswObject(rsw, getROObjects());
	obj->loadTextures(getTextureManager(), getFileManager());

	printf("Map info:\n");
	printf("\tRequested map: %s\n", map);
	printf("\tMap size: %d, %d\n", obj->getGND()->getWidth(),  obj->getGND()->getHeight());
	
	setMap(obj);
}

void OpenRO::BeforeRun() {
	ParseClientInfo();

	InitDisplay(1024, 768, false);

	// TODO: This should be a parameter
	sdle::FTFont* lsans = new sdle::FTFont();
	FileData data = m_filemanager.getFile("font\\lsans.ttf");
	if (!lsans->openFromMemory(data.getBuffer(), data.blobSize(), 12)) {
		fprintf(stderr, "Erorr loading font 'font\\lsans.ttf'\n");
	}
	m_gui.FontManager().add("lsans.ttf", lsans);
	m_gui.setDefaultFont(lsans);

	m_texturemanager.RegisterPNG(getFileManager(), "openro\\selected.png");

	// Hide the mouse cursor
	SDL_ShowCursor(0);

	dskLogin = new DesktopLogin(this);
	dskService = new DesktopService(this);
	dskCreate = new DesktopCreate(this);
	dskChar = new DesktopChar(this);

#if 1
	m_gui.setDesktop(dskLogin);
#else
	me.open(*this, RO::J_ALCHEMIST, RO::S_MALE);
	me.map_x = 50;
	me.map_y = 50;
	//LoadMap("new_zone01");
	LoadMap("new_1-1");
#endif

	FullAct ycursor;
	char xcursor[256];
	sprintf(xcursor,"sprite\\cursors");
	ycursor.Load(xcursor, getROObjects(), getFileManager(), getTextureManager());
	setCursor(ycursor);
}

//Add new packets here

void OpenRO::hndlUpdateStatus(ronet::pktUpdateStatus* pkt) {
	unsigned short type = pkt->getType();
	unsigned int value = pkt->getValue();

	printf("Update status \"%d\" with value \"%d\"\n", type, value);
	
}

void OpenRO::hndlServerList(ronet::pktServerList* pkt) {
	m_serverlist = pkt;
	m_gui.setDesktop(dskService);
	dskService->setEnabled(true);
	char buf[128];

	dskService->clear();
	for (unsigned int i = 0; i < pkt->getServerCount(); i++) {
		sprintf(buf, "%s (%d)", pkt->getInfo(i).name, pkt->getInfo(i).users);
		dskService->add(buf);
	}
}

void OpenRO::hndlCharList(ronet::pktCharList* pkt) {
	int count = pkt->getCount();
	printf("Received a list of %d chars\n", count);
	m_gui.setDesktop(dskChar);
	dskChar->setEnabled(true);
	
	for(int x=0;x<count;x++){
		dskChar->addChar(pkt->getChar(x));
	}
}

//[kR105]
void OpenRO::hndlLoginError(ronet::pktLoginError* pkt) {
	short errorId = pkt->getErrorId();
	char errorDesc[256];

	//Error description taken from OpenKore & eAthena
	switch(errorId){
		case 0:
			sprintf(errorDesc,"Account name doesn't exist");
			break;
		case 1:
			sprintf(errorDesc,"Password Error");
			break;
		case 3:
			sprintf(errorDesc,"Rejected from Server");
			break;
		case 4:
			sprintf(errorDesc,"Your account has been blocked");
			break;
		case 5:
			sprintf(errorDesc,"Your Game's EXE file is not the latest version");
			break;
		case 6:
			sprintf(errorDesc,"The server is temporarily blocking your connection");
			break;
		default:
			sprintf(errorDesc,"Unknown error");
			break;
	}
	printf("Login error: %s (Error number %d)\n", errorDesc, errorId);

	//We don't need the connection anymore.
	m_network.getLogin().Close();
}

//[kR105]
void OpenRO::hndlAuthFailed(ronet::pktAuthFailed* pkt) {
	short errorId = pkt->getErrorId();
	char errorDesc[256];

	//Error description taken from OpenKore & eAthena
	switch(errorId){
		case 0:
			sprintf(errorDesc,"Server shutting down.");
			break;
		case 1:
			sprintf(errorDesc,"Server connection closed.");
			break;
		case 2:
			sprintf(errorDesc,"Someone has already logged in with this ID.");
			break;
		case 3:
			sprintf(errorDesc,"You've been disconnected due to a time gap between you and the server.");
			break;
		case 4:
			sprintf(errorDesc,"Server is jammed due to over population. Please try again shortly.");
			break;
		case 5:
			sprintf(errorDesc,"You are underaged and cannot join this server.");
			break;
		case 8:
			sprintf(errorDesc,"Server still recognizes your last log-in. Please try again after a few minutes.");
			break;
		case 9:
			sprintf(errorDesc,"IP capacity of this Internet Cafe is full. Would you like to pay the personal base?");
			break;
		case 10:
			sprintf(errorDesc,"You are out of available time paid for. Game will be shut down automatically.");
			break;
		case 15:
			sprintf(errorDesc,"You have been forced to disconnect by the Game Master Team.");
			break;
		default:
			sprintf(errorDesc,"Unknown error");
			break;
	}
	printf("Auth Failed: %s (Error number %d)\n", errorDesc, errorId);

	//We don't need the connection anymore.
	CloseSockets();

	LoginScreen();
}

void OpenRO::hndlCharCreated(ronet::pktCharCreated* pkt) {
	unsigned short cid = pkt->getID();
	printf("Received the new character created with ID %d\n", cid);
	
	CharInformation newchar = pkt->getChar();
	dskChar->addChar(newchar);
}

void OpenRO::hndlCharPosition(ronet::pktCharPosition* pkt) {
	//Convert the IP to string (stored in long)
	struct in_addr addr;
	addr.s_addr = pkt->getIp();
	
	char IP[256];
	sprintf(IP,"%s",inet_ntoa(addr));

	printf("MapServer IP: %s\n",IP);
	printf("MapServer Port: %d\n",pkt->getPort());
	printf("Character Position: %s\n",pkt->getMapname());
	printf("Character ID: %d\n",pkt->getCharID());

	//Close the socket to the char server
	m_network.getChar().Close();

	//Connect to the mapserver
	m_network.getMap().Connect(IP, pkt->getPort());

	//Login to the mapserver
	m_network.MapLogin(m_serverlist->getAccountId(), pkt->getCharID(), m_serverlist->getSessionId1(), SDL_GetTicks(), m_serverlist->getSex());

	char map[32];
	strcpy(map, pkt->getMapname());

	int i = 0;
	while (map[i] != 0) {
		if (map[i] == '.') {
			map[i] = 0;
			break;
		}
		i++;
	}
	
	LoadMap(map);

	// TODO: Set desktop to the ingame desktop
	m_gui.setDesktop(NULL);
}

void OpenRO::hndlMapAcctSend(ronet::pktMapAcctSend* pkt) {
	printf("Received accountID from mapserver: %d\n", pkt->getAccountId());
}

void OpenRO::hndlMapLoginSuccess(ronet::pktMapLoginSuccess* pkt) {
	short pos_x = pkt->getPosX(); 
	short pos_y = pkt->getPosY();
	short pos_dir = pkt->getPosDir();
	unsigned int server_tick = pkt->getServerTick();

	me.map_x = pos_x;
	me.map_y = pos_y;

	printf("pos_x = %d \npos_y = %d\npos_dir = %d\nserver_tick = %d\n\n\n",pos_x,pos_y,pos_dir,server_tick);
}

void OpenRO::hndlOwnSpeech(ronet::pktOwnSpeech* pkt) {
	printf("OwnSpeech: %s \n",pkt->getText());
}

void OpenRO::hndlSkillList(ronet::pktSkillList* pkt) {
	printf("Received skill list.\n");
}

void OpenRO::CreateCharWindow(int slot) {
	dskCreate->slot = slot;
	dskCreate->readObjects();
	m_gui.setDesktop(dskCreate);
	dskCreate->setEnabled(true);
}

void OpenRO::CreateChar(const std::string& charname, const CharAttributes& attr, int slott, unsigned short color, unsigned short style) {
	printf("Creating char in slot %d\n",slott);
	dskCreate->setEnabled(false);
	m_network.CreateChar(charname, attr, slott, color, style);
	m_gui.setDesktop(dskChar);
}

void OpenRO::ParseClientInfo(const std::string& name){
	FileManager& fm = OpenRO::getFileManager();

	TiXmlDocument doc;
	FileData data;

	data = fm.getFile(name);
	if (data.blobSize() == 0) {
		std::cerr << "OpenRO::ParseClientInfo(): Error loading file " << name << std::endl;
	}
	doc.Parse((const char*)data.getBuffer());

	TiXmlElement* sclient_root = doc.FirstChildElement("clientinfo");
	if(sclient_root){
		TiXmlElement* sclient_child = sclient_root->FirstChildElement("connection");
		if(sclient_child){
			TiXmlElement* sclient_child_addr = sclient_child->FirstChildElement("address");
			if(sclient_child_addr){
				const char *ip = sclient_child_addr->GetText();
				strcpy(OpenRO::ConnectionIP, ip);
			}
			TiXmlElement* sclient_child_port = sclient_child->FirstChildElement("port");
			if(sclient_child_port){
				const char *port = sclient_child_port->GetText();
				OpenRO::ConnectionPort = atoi(port);
			}

		}
	}
}

void OpenRO::CloseSockets(){
	//Close socket to Login-Server
	m_network.getLogin().Close();

	//Close socket to Char-Server
	m_network.getChar().Close();

	//Close socket to Map-Server
	m_network.getMap().Close();
	return;
}

void OpenRO::KeepAliveChar(){
	//If the socket to charserver is closed
	if(!m_network.getChar().isConnected())
		return;

	//Send the KeepAlive packet
	m_network.KeepAliveChar(m_serverlist->getAccountId());
	printf("CharServer KeepAlive sent.\n");
}

unsigned int OpenRO::GetAccountID(){return m_serverlist->getAccountId();}
unsigned char OpenRO::GetAccountSex(){
	unsigned char male = 0x01; //m
	unsigned char female = 0x02; //f
	
	if(!m_network.getChar().isConnected())
		return 0; 
	if(m_serverlist->getSex() == male )
		return 0; 
	else if (m_serverlist->getSex() == female )
		return 1;
	else{
		printf("Error getting account sex! Defaulting to male.\n");
		return 0;
	}
}