#include "stdafx.h"

#define STRICT 1 
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include "portmidi.h"
//2020sept27, spi, begin
//#include "spiws_midiutility.h"
#include "spiwavsetlib.h"
//2020sept27, spi, end
#include <map>
#include <vector>

#include <assert.h>


//The event signaled when the app should be terminated.
HANDLE g_hTerminateEvent = NULL;
//Handles events that would normally terminate a console application. 
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
int Terminate();

//global variables and function
UINT global_TimerId=0;
PmStream* global_pPmStream = NULL; // midi output
DWORD global_dwStartTime_ms;
float global_loopduration_s;
int global_outputmidichannel=-1;

map<int, PmEvent*> global_noteonmap;
map<int, PmEvent*> global_noteoffmap;
vector<int> global_nonplayingnotesvector;
vector<int> global_nowplayingnotesvector;

map<string,int> global_midioutputdevicemap;

//2020sept27, spi, begin
int global_sequential_integer=0;
vector<int> global_sequential_notesvector;
string global_modestring = "RANDOM";
int global_outputmidichannelrange = 0;
//2020sept27, spi, end

bool PlayNextNoteOn()
{
	PmEvent* pPmEvent = NULL;
	/*
	//get random integer
	int random_integer;
	int lowest=1, highest=global_nonplayingnotesvector.size();
	int range=(highest-lowest)+1;
	random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	*/
	global_sequential_integer++;
	if(global_sequential_integer>global_sequential_notesvector.size()) global_sequential_integer=1;
	//get midinotenumber
	//int midinotenumber = global_nonplayingnotesvector.at(global_sequential_integer-1);
	int midinotenumber = global_sequential_notesvector.at(global_sequential_integer-1);
	//global_nonplayingnotesvector.erase(global_nonplayingnotesvector.begin()+(global_sequential_integer-1));
	global_nonplayingnotesvector.erase(global_nonplayingnotesvector.begin()+(0)); //0 because only 1 simultaneous note supported for now in sequential mode
	global_nowplayingnotesvector.push_back(midinotenumber);
	//find midi event
	map<int, PmEvent*>::iterator it;
	it = global_noteonmap.find(midinotenumber);
	pPmEvent = (*it).second;
	//send midi event
	Pm_Write(global_pPmStream, pPmEvent, 1);
	return true;
}

bool TurnNextNoteOff()
{
	PmEvent* pPmEvent = NULL;
	/*
	//get random integer
	int random_integer;
	int lowest=1, highest=global_nowplayingnotesvector.size();
	int range=(highest-lowest)+1;
	random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	*/
	if(global_sequential_integer<1) return false;
	if(global_sequential_integer>global_sequential_notesvector.size()) return false;
	//get midinotenumber
	//int midinotenumber = global_nowplayingnotesvector.at(global_sequential_integer-1);
	int midinotenumber = global_sequential_notesvector.at(global_sequential_integer-1);
	if(midinotenumber<0 || midinotenumber>127) assert(false);
	//global_nowplayingnotesvector.erase(global_nowplayingnotesvector.begin()+(global_sequential_integer-1));
	global_nowplayingnotesvector.erase(global_nowplayingnotesvector.begin()+(0)); //0 because only 1 simultaneous note supported for now in sequential mode
	global_nonplayingnotesvector.push_back(midinotenumber);
	//find midi event
	map<int, PmEvent*>::iterator it;
	it = global_noteoffmap.find(midinotenumber);
	pPmEvent = (*it).second;
	//send midi event
	Pm_Write(global_pPmStream, pPmEvent, 1);
	return true;
}

bool PlayRandomNoteOn()
{
	PmEvent* pPmEvent = NULL;
	//get random integer
	int random_integer;
	int lowest=1, highest=global_nonplayingnotesvector.size();
	int range=(highest-lowest)+1;
	random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	//get midinotenumber
	int midinotenumber = global_nonplayingnotesvector.at(random_integer-1);
	global_nonplayingnotesvector.erase(global_nonplayingnotesvector.begin()+(random_integer-1));
	global_nowplayingnotesvector.push_back(midinotenumber);
	//find midi event
	map<int, PmEvent*>::iterator it;
	it = global_noteonmap.find(midinotenumber);
	pPmEvent = (*it).second;
	//send midi event
	Pm_Write(global_pPmStream, pPmEvent, 1);
	return true;
}

bool TurnRandomNoteOff()
{
	PmEvent* pPmEvent = NULL;
	//get random integer
	int random_integer;
	int lowest=1, highest=global_nowplayingnotesvector.size();
	int range=(highest-lowest)+1;
	random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	//get midinotenumber
	int midinotenumber = global_nowplayingnotesvector.at(random_integer-1);
	if(midinotenumber<0 || midinotenumber>127) assert(false);
	global_nowplayingnotesvector.erase(global_nowplayingnotesvector.begin()+(random_integer-1));
	global_nonplayingnotesvector.push_back(midinotenumber);
	//find midi event
	map<int, PmEvent*>::iterator it;
	it = global_noteoffmap.find(midinotenumber);
	pPmEvent = (*it).second;
	//send midi event
	Pm_Write(global_pPmStream, pPmEvent, 1);
	return true;
}

bool TurnAllNotesOff()
{
	////////////////////////////
	//turn all playing notes off
	////////////////////////////
	/*
	vector<int>::iterator it;
	for(it=global_nowplayingnotesvector.begin(); it<global_nowplayingnotesvector.end(); it++)
	{
		int midinotenumber = *it;
		if(midinotenumber<0 || midinotenumber>127) assert(false);
		//find midi event
		map<int, PmEvent*>::iterator it2;
		it2 = global_noteoffmap.find(midinotenumber);
		PmEvent* pPmEvent = (*it2).second;
		//send midi event
		Pm_Write(global_pPmStream, pPmEvent, 1);
	}
	*/
	/*
	/////////////////////////////
	//turn all selected notes off
	/////////////////////////////
	map<int, PmEvent*>::iterator it2;
	for(it2=global_noteoffmap.begin(); it2!=global_noteoffmap.end(); it2++)
	{
		PmEvent* pPmEvent = (*it2).second;
		//send midi event
		Pm_Write(global_pPmStream, pPmEvent, 1);
	}
	*/
	//note: for the time being that the spimidisampler does not match midi notes exactly (in the case when using native instrument's samples)
	//      we are forced to send note off message to all possible notes in order to hit the ones that were falsely matched.
	/////////////////////////////
	//turn all selected notes off
	/////////////////////////////
	PmEvent myPmEvent;
	myPmEvent.timestamp = 0;
	for(int midinotenumber=0; midinotenumber<128; midinotenumber++)
	{
		myPmEvent.message = Pm_Message(0x90+global_outputmidichannel, midinotenumber, 0);
		//send midi event
		Pm_Write(global_pPmStream, &myPmEvent, 1);
	}
	return true;
}

VOID CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime) 
{
	//cout << "Time: " << dwTime << '\n';
	//cout.flush();
	float totalduration_s = (dwTime-global_dwStartTime_ms)/1000.0;
	if(global_loopduration_s>0.0f && totalduration_s>global_loopduration_s)
	{	
		KillTimer(NULL, global_TimerId);
		global_TimerId = 0;
		//exit(0);
        // Tell the main thread to exit the app 
        //::SetEvent(g_hTerminateEvent);
		HWND hWnd = ::GetConsoleWindow( ) ;
		::PostMessage(hWnd, WM_CLOSE, 0, 0 ) ;
	}
	else
	{
		cout << "Time: " << totalduration_s << " sec" << '\n';
		cout.flush();
	}
}


int main(int argc, char **argv) 
{
	int nShowCmd = false;
	//ShellExecuteA(NULL, "open", "begin.bat", "", NULL, nShowCmd);

	//string noteset = "C3,D3,E3,F3,G3,A3,B3,C4,D4,E4,F4,G4,A4,B4";
	string noteset = "C3,D3,E3,F3,G3,A3,B3";
	//string noteset = "C2,D2,E2,F2,G2,A2,B2";
	//string noteset = "A1,A2,A3,A4,A5,A6,A7,A8";
	//float notechangeperiod_s = 5.0f; //trigger change every 5.0 second
	float notechangeperiod_s = 2.0f; //trigger change every 2.0 second
	//float notechangeperiod_s = 0.5f; //trigger change every 0.5 second
	global_loopduration_s = 300.0f; //total loop duration, if negative will loop indefinitely
	//int outputmididevice = 12; //device id 12, for "Microsoft GS Wavetable Synth", when 8 yoke installed for spi
	int outputmididevice = 13; //device id 13, for "Out To MIDI Yoke:  1", when 8 yoke installed for spi
	int outputmidichannel = 1; 
	int numberofsimultaneousnotes = 5;
	int numberofnotesonchange = 2;
	if(argc>1)
	{
		//set of notes to be used
		noteset = argv[1];
		int myoctave = atoi(noteset.c_str());
		if(noteset=="ALL" || noteset=="-2") 
		{
			noteset=GetNoteNamesForOctave(); //all notes from all octaves
		}
		else if(IsNoteNoOctave(noteset))
		{
			//int noteoffset = GetNoteOffsetInFirstOctave(noteset);
			//string notenooctave = GetNoteNoOctave(noteoffset);
			noteset=GetNoteNamesForNote(noteset);
		}
		else if(myoctave>-2 && myoctave<10)
		{
			noteset=GetNoteNamesForOctave(myoctave); //all notes from single octave
		}
	}
	if(argc>2)
	{
		//period of time before note change occur
		notechangeperiod_s = atof(argv[2]);
	}
	if(argc>3)
	{
		//loop duration
		global_loopduration_s = atof(argv[3]);
	}
	string midioutputdevicename="Out To MIDI Yoke:  1";
	if(argc>4)
	{
		//outputmididevice
		//outputmididevice = atoi(argv[4]);
		midioutputdevicename=argv[4];
	}
	if(argc>5)
	{
		//outputmidichannel
		outputmidichannel = atoi(argv[5]);
	}
	if(argc>6)
	{
		//numberofsimultaneousnotes
		numberofsimultaneousnotes = atoi(argv[6]);
	}
	if(argc>7)
	{
		//numberofnotesonchange
		numberofnotesonchange = atoi(argv[7]);
	}
	if(argc>8)
	{
		global_modestring=argv[8];
		if(global_modestring != "RANDOM") global_modestring = "SEQUENTIAL";
	}
	if(argc>9)
	{
		//outputmidichannel
		global_outputmidichannelrange = atoi(argv[9]);
		if(global_outputmidichannelrange<0) global_outputmidichannelrange=0;
		if(global_outputmidichannelrange>16) global_outputmidichannelrange=16;
	}
	global_outputmidichannel = outputmidichannel;

	if(noteset.size()<numberofsimultaneousnotes)
	{
		cout << "invalid noteset, the set of notes must be at least equal to the number of sultaneous notes to be played" << endl;
		Terminate();
	}
	if(numberofnotesonchange>numberofsimultaneousnotes)
	{
		cout << "invalid numberofnotesonchange, the number of notes to be changed must be at most equal to the number of sultaneous notes to be played" << endl;
		Terminate();
	}

	if(notechangeperiod_s<=0.005)
	{
		cout << "invalid notechangeperiod_s, note change should not occur faster than 5 ms" << endl;
		Terminate();
	}
	if(outputmidichannel<0 || outputmidichannel>15)
	{
		cout << "invalid outputmidichannel, midi channel must range from 0 to 15 inclusively." << endl;
		Terminate();
	}
	
	//Auto-reset, initially non-signaled event 
    g_hTerminateEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    //Add the break handler
    ::SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);


	/////////////////////////
	//portmidi initialization
	/////////////////////////
    PmError err;
	Pm_Initialize();
    // list device information 
    cout << "MIDI output devices:" << endl;
    for (int i = 0; i < Pm_CountDevices(); i++) 
	{
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        if (info->output) 
		{
			printf("%d: %s, %s\n", i, info->interf, info->name);
			string devicename = info->name;
			global_midioutputdevicemap.insert(pair<string,int>(devicename,i));
		}
    }
	int midioutputdeviceid = 13;
	map<string,int>::iterator it;
	it = global_midioutputdevicemap.find(midioutputdevicename);
	if(it!=global_midioutputdevicemap.end())
	{
		midioutputdeviceid = (*it).second;
		printf("%s maps to %d\n", midioutputdevicename.c_str(), midioutputdeviceid);
	}
	cout << "device " << midioutputdeviceid << " selected" << endl;
    //err = Pm_OpenInput(&midi_in, inp, NULL, 512, NULL, NULL);
    err = Pm_OpenOutput(&global_pPmStream, midioutputdeviceid, NULL, 512, NULL, NULL, 0); //0 latency
    if (err) 
	{
        printf(Pm_GetErrorText(err));
        //Pt_Stop();
		Terminate();
        //mmexit(1);
    }


	/////////////////////////
	//midi message definition
	/////////////////////////
	string notename;
	stringstream stream(noteset);
	while( getline(stream, notename, ',') )
	{
		cout << notename << "\n";
		int  midinotenumber = GetMidiNoteNumberFromString(notename.c_str());
		if(midinotenumber<0 || midinotenumber>127) assert(false);
		//note on
		PmEvent* pPmEvent = new PmEvent;
		pPmEvent->timestamp = 0;
		pPmEvent->message = Pm_Message(0x90+outputmidichannel, midinotenumber, 100);
		global_noteonmap.insert(pair<int,PmEvent*>(midinotenumber, pPmEvent));
		//note off
		pPmEvent = new PmEvent;
		pPmEvent->timestamp = 0;
		pPmEvent->message = Pm_Message(0x90+outputmidichannel, midinotenumber, 0);
		global_noteoffmap.insert(pair<int,PmEvent*>(midinotenumber, pPmEvent));
		//maintain "non playing notes" list consistant
		global_nonplayingnotesvector.push_back(midinotenumber);
		//2020sept27, spi, begin
		//build sequential note list from noteset
		global_sequential_notesvector.push_back(midinotenumber);
		//2020sept27, spi, end
	}

	/////////////////////////
	//initial random notes on
	/////////////////////////
	for(int i=0; i<numberofsimultaneousnotes; i++)
	{
		if(global_modestring=="RANDOM")
		{
			PlayRandomNoteOn();
		}
		else
		{
			PlayNextNoteOn();
		}
		Sleep(notechangeperiod_s*1000);
	}
	/*
	PmEvent myPmEvent[2];
	//note on
	myPmEvent[0].timestamp = 0;
	myPmEvent[0].message = Pm_Message(0x90+outputmidichannel, 60, 100); //channel 0
	//myPmEvent[0].message = Pm_Message(0x91, 60, 100); //channel 1
	//note off
	myPmEvent[1].timestamp = 0;
	myPmEvent[1].message = Pm_Message(0x90+outputmidichannel, 60, 0); //channel 0
	//myPmEvent[1].message = Pm_Message(0x91, 60, 0); //channel 1

	PmEvent tempPmEvent;
	*/

	//////////////////
	//loop the pattern
	//////////////////
	float timerelapse_s = notechangeperiod_s;
	UINT timerelapse_ms = (UINT)(timerelapse_s*1000);
	string patterncode;
	int Counter=0;
	MSG Msg;
	global_dwStartTime_ms = GetTickCount(); //log start time
	//global_TimerId = SetTimer(NULL, 1, 1000, &TimerProc); //SetTimer(NULL, 0, 500, &TimerProc);
	global_TimerId = SetTimer(NULL, 1, timerelapse_ms, (TIMERPROC)(&TimerProc));
	cout << "TimerId: " << global_TimerId << '\n';
	if (!global_TimerId)
		return 16;
	PmEvent* prev_pPmEvent = NULL;
	while (GetMessage(&Msg, NULL, 0, 0)) 
	{
		++Counter;
		if (Msg.message == WM_TIMER)
		{
			cout << "Counter: " << Counter << "; timer message\n";

			for(int i=0; i<numberofnotesonchange ; i++)
			{
				if(global_modestring=="RANDOM")
				{
					TurnRandomNoteOff();
					PlayRandomNoteOn();
				}
				else
				{
					TurnNextNoteOff();
					PlayNextNoteOn();
				}
			}
			/*
			if(prev_pPmEvent)
			{
				//i.e. note off
				//Pm_Write(global_pPmStream, &(myPmEvent[1]), 1);
				PmEvent thisPmEvent;
				thisPmEvent.timestamp = 0;
				thisPmEvent.message = Pm_Message(0x90+outputmidichannel, Pm_MessageData1(prev_pPmEvent->message), 0); //note off, note on with velocity 0
				Pm_Write(global_pPmStream, &thisPmEvent, 1);
			}
			patterncode = midieventpattern.substr(Counter-1,1);
			if(patterncode.compare("0")!=0)
			{
				if(patterncode.compare("*")!=0)
				{
					//note on
					Pm_Write(global_pPmStream, &(myPmEvent[0]), 1);
					prev_pPmEvent = &(myPmEvent[0]);
				}
				else
				{
					//random note on
					int random_integer;
					int lowest=1, highest=20;
					int range=(highest-lowest)+1;
					random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));

					tempPmEvent.timestamp = 0;
					tempPmEvent.message = Pm_Message(0x90+outputmidichannel, 60+random_integer, 100); //note on, channel 0
					Pm_Write(global_pPmStream, &tempPmEvent, 1);
					prev_pPmEvent = &tempPmEvent;
				}
			}
			else
			{
				prev_pPmEvent = NULL;	
			}
			if(Counter==midieventpattern.size()) Counter=0;
			*/
		}
		else
		{
			cout << "Counter: " << Counter << "; message: " << Msg.message << '\n';
		}
		DispatchMessage(&Msg);
	}
	Terminate();
	return 0;
}

int Terminate()
{
	////////////
	//kill timer
	////////////
	if(global_TimerId) 
	{
		KillTimer(NULL, global_TimerId);
	}

	////////////////////
	//turn all notes off
	////////////////////
	TurnAllNotesOff();

	////////////////////
	//terminate portmidi
	////////////////////
	Pm_Close(global_pPmStream);
    //Pt_Stop();
    Pm_Terminate();

	////////////////////////////////
	//delete allocated memory object
	////////////////////////////////
	map<int, PmEvent*>::iterator it1; 
	for(it1=global_noteonmap.begin(); it1!=global_noteonmap.end(); it1++)
	{
		delete((*it1).second);
	}
	map<int, PmEvent*>::iterator it2;
	for(it2=global_noteoffmap.begin(); it2!=global_noteoffmap.end(); it2++)
	{
		delete((*it2).second);
	}

	int nShowCmd = false;
	ShellExecuteA(NULL, "open", "end.bat", "", NULL, nShowCmd);
	return 0;
}

//Called by the operating system in a separate thread to handle an app-terminating event. 
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT ||
        dwCtrlType == CTRL_BREAK_EVENT ||
        dwCtrlType == CTRL_CLOSE_EVENT)
    {
        // CTRL_C_EVENT - Ctrl+C was pressed 
        // CTRL_BREAK_EVENT - Ctrl+Break was pressed 
        // CTRL_CLOSE_EVENT - Console window was closed 
		Terminate();
        // Tell the main thread to exit the app 
        ::SetEvent(g_hTerminateEvent);
        return TRUE;
    }

    //Not an event handled by this function.
    //The only events that should be able to
	//reach this line of code are events that
    //should only be sent to services. 
    return FALSE;
}
